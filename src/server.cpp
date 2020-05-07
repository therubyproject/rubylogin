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

#include "outputmessage.h"
#include "server.h"
#include "connectionmanager.h"
#include "logger.h"

Server::~Server()
{
    close();
}

void Server::accept()
{
    if (!acceptor) {
        return;
    }

    auto connection = g_connectionManager.createConnection(io_service);
    acceptor->async_accept(connection->getSocket(), std::bind(&Server::onAccept, shared_from_this(), connection, std::placeholders::_1));
}

void Server::onAccept(ConnectionSharedPtr connection, const boost::system::error_code& error)
{
    if (!error) {
        auto remote_ip = connection->getIP();
        if (remote_ip != 0) {
            connection->accept();
        } else {
            connection->close();
        }

        accept();
    }
}

void Server::open(const std::string& ip, int32_t port)
{
    try {
        acceptor.reset(new boost::asio::ip::tcp::acceptor(io_service, boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::address(boost::asio::ip::address_v4::from_string(ip)), port)));

        acceptor->set_option(boost::asio::ip::tcp::no_delay(true));

        g_logger.info("Listening on tcp://" + ip + ":" + std::to_string(port));

        accept();
        io_service.run();
    } catch (boost::system::system_error& e) {
        g_logger.info("Failed to bind at address tcp://" + ip + ":" + std::to_string(port) + ": " + e.what());
    }
}

void Server::close()
{
    if (acceptor && acceptor->is_open()) {
        boost::system::error_code error;
        acceptor->close(error);
    }
    io_service.stop();
}
