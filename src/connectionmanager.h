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

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "connection.h"

class ConnectionManager
{
    public:
        ConnectionManager() = default;
        ~ConnectionManager() = default;

        // non-copyable
        ConnectionManager(const ConnectionManager&) = delete;
        ConnectionManager& operator=(const ConnectionManager&) = delete;

        ConnectionSharedPtr createConnection(boost::asio::io_service& io_service);
        void releaseConnection(const ConnectionSharedPtr& connection);
        void closeAll();

    protected:
        std::unordered_set<ConnectionSharedPtr> connections;
        std::mutex connectionManagerLock;
};

extern ConnectionManager g_connectionManager;

#endif
