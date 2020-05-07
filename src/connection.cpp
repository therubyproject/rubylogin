/**
 * Ruby Login
 * Copyright (C) 2020 Leandro Matheus de Oliveira Sarna
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "includes.h"

#include "connection.h"
#include "connectionmanager.h"
#include "outputmessage.h"
#include "protocol.h"
#include "server.h"
#include "logger.h"

void Connection::close()
{
    std::lock_guard<std::recursive_mutex> lockClass(connectionLock);
    g_connectionManager.releaseConnection(shared_from_this());
    closeSocket();
}

void Connection::closeSocket()
{
    if (socket.is_open()) {
        try {
            readTimer.cancel();
            writeTimer.cancel();
            boost::system::error_code error;
            socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
            socket.close(error);
        } catch (boost::system::system_error& e) {
            g_logger.error("Network error: " + std::string(e.what()));
        }
    }
}

Connection::~Connection()
{
    closeSocket();
}

void Connection::accept()
{
    protocol = std::make_shared<Protocol>(shared_from_this());

    std::lock_guard<std::recursive_mutex> lockClass(connectionLock);
    try {
        readTimer.expires_from_now(boost::posix_time::seconds(CONNECTION_READ_TIMEOUT));
        readTimer.async_wait(std::bind(&Connection::handleTimeout, std::weak_ptr<Connection>(shared_from_this()), std::placeholders::_1));

        boost::asio::async_read(socket,
                                boost::asio::buffer(msg.getBuffer(), NetworkMessage::headerLength),
                                std::bind(&Connection::parseHeader, shared_from_this(), std::placeholders::_1));
    } catch (boost::system::system_error& e) {
        g_logger.error("Network error: " + std::string(e.what()));
        close();
    }
}

void Connection::parseHeader(const boost::system::error_code& error)
{
    std::lock_guard<std::recursive_mutex> lockClass(connectionLock);
    readTimer.cancel();

    if (error) {
        close();
        return;
    }

    uint16_t size = msg.getLengthHeader();
    if (size == 0 || size >= NETWORKMESSAGE_MAXSIZE - 16) {
        close();
        return;
    }

    try {
        readTimer.expires_from_now(boost::posix_time::seconds(CONNECTION_READ_TIMEOUT));
        readTimer.async_wait(std::bind(&Connection::handleTimeout, std::weak_ptr<Connection>(shared_from_this()),
                                        std::placeholders::_1));

        // Read packet content
        msg.setLength(size + NetworkMessage::HEADER_LENGTH);
        boost::asio::async_read(socket, boost::asio::buffer(msg.getBodyBuffer(), size),
                                std::bind(&Connection::parsePacket, shared_from_this(), std::placeholders::_1));
    } catch (boost::system::system_error& e) {
        g_logger.error("Network error: " + std::string(e.what()));
        close();
    }
}

void Connection::parsePacket(const boost::system::error_code& error)
{
    std::lock_guard<std::recursive_mutex> lockClass(connectionLock);
    readTimer.cancel();

    if (error) {
        close();
        return;
    }

    //Check packet checksum
	uint32_t checksum;
	int32_t len = msg.getLength() - msg.getBufferPosition() - NetworkMessage::CHECKSUM_LENGTH;
	if (len > 0) {
		checksum = adlerChecksum(msg.getBuffer() + msg.getBufferPosition() + NetworkMessage::CHECKSUM_LENGTH, len);
	} else {
		checksum = 0;
	}

	uint32_t recvChecksum = msg.get<uint32_t>();
	if (recvChecksum != checksum) {
		protocol->disconnectClient("Invalid Checksum.");
        return;
	}

    msg.skipBytes(1); // Skip protocol ID

    protocol->authenticate(msg);
    protocol->disconnect();
}

void Connection::send(OutputMessage& msg)
{
    std::lock_guard<std::recursive_mutex> lockClass(connectionLock);
    internalSend(msg);
}

void Connection::internalSend(OutputMessage& msg)
{
    protocol->encryptMessage(msg);
    try {
        writeTimer.expires_from_now(boost::posix_time::seconds(CONNECTION_WRITE_TIMEOUT));
        writeTimer.async_wait(std::bind(&Connection::handleTimeout, std::weak_ptr<Connection>(shared_from_this()),
            std::placeholders::_1));

        boost::asio::async_write(socket,
            boost::asio::buffer(msg.getOutputBuffer(), msg.getLength()),
            std::bind(&Connection::onWriteOperation, shared_from_this(), std::placeholders::_1));
    } catch (boost::system::system_error& e) {
        g_logger.error("Network error: " + std::string(e.what()));
        close();
    }
}

uint32_t Connection::getIP()
{
    std::lock_guard<std::recursive_mutex> lockClass(connectionLock);

    // IP-address is expressed in network byte order
    boost::system::error_code error;
    const boost::asio::ip::tcp::endpoint endpoint = socket.remote_endpoint(error);
    if (error) {
        return 0;
    }

    return htonl(endpoint.address().to_v4().to_ulong());
}

void Connection::onWriteOperation(const boost::system::error_code& error)
{
    std::lock_guard<std::recursive_mutex> lockClass(connectionLock);
    writeTimer.cancel();

    if (error) {
        close();
        return;
    }
}

void Connection::handleTimeout(ConnectionWeakPtr connectionWeak, const boost::system::error_code& error)
{
    if (error == boost::asio::error::operation_aborted) {
        return;
    }

    if (auto connection = connectionWeak.lock()) {
        connection->close();
    }
}
