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

#include "connectionmanager.h"

ConnectionManager g_connectionManager;

ConnectionSharedPtr ConnectionManager::createConnection(boost::asio::io_service& io_service)
{
    std::lock_guard<std::mutex> lockClass(connectionManagerLock);

    auto connection = std::make_shared<Connection>(io_service);
    connections.insert(connection);
    return connection;
}

void ConnectionManager::releaseConnection(const ConnectionSharedPtr& connection)
{
    std::lock_guard<std::mutex> lockClass(connectionManagerLock);

    connections.erase(connection);
}

void ConnectionManager::closeAll()
{
    std::lock_guard<std::mutex> lockClass(connectionManagerLock);

    for (const auto& connection : connections) {
        try {
            boost::system::error_code error;
            connection->socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
            connection->socket.close(error);
        } catch (boost::system::system_error&) {
        }
    }
    connections.clear();
}
