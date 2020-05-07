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

#ifndef CONNECTION_H
#define CONNECTION_H

#include "outputmessage.h"
#include "types.h"

static constexpr int32_t CONNECTION_WRITE_TIMEOUT = 30;
static constexpr int32_t CONNECTION_READ_TIMEOUT = 30;

class Connection : public std::enable_shared_from_this<Connection>
{
    public:
        // non-copyable
        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;

        Connection(boost::asio::io_service& io_service) :
            readTimer(io_service),
            writeTimer(io_service),
            socket(io_service) {}
        ~Connection();

        void close();
        void accept();

        void send(OutputMessage& msg);

        uint32_t getIP();

    private:
        void parseHeader(const boost::system::error_code& error);
        void parsePacket(const boost::system::error_code& error);

        void onWriteOperation(const boost::system::error_code& error);

        static void handleTimeout(ConnectionWeakPtr connectionWeak, const boost::system::error_code& error);

        void closeSocket();
        void internalSend(OutputMessage& msg);

        boost::asio::ip::tcp::socket& getSocket() {
            return socket;
        }

        NetworkMessage msg;

        boost::asio::deadline_timer readTimer;
        boost::asio::deadline_timer writeTimer;

        std::recursive_mutex connectionLock;

        ProtocolSharedPtr protocol;

        boost::asio::ip::tcp::socket socket;

        friend class ConnectionManager;
        friend class Server;
};

#endif
