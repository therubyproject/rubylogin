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

#ifndef SERVER_H
#define SERVER_H

#include "connection.h"

class Server : public std::enable_shared_from_this<Server>
{
    public:
        explicit Server(boost::asio::io_service& io_service) : io_service(io_service) {}
        ~Server();

        // non-copyable
        Server(const Server&) = delete;
        Server& operator=(const Server&) = delete;

        void open(const std::string& ip, int32_t port);
        void close();

        void onAccept(ConnectionSharedPtr connection, const boost::system::error_code& error);

    protected:
        void accept();

        boost::asio::io_service& io_service;
        std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
};

#endif
