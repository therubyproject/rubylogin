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

#include "signals.h"
#include "logger.h"
#include "database.h"
#include "connectionmanager.h"

Signals::Signals(boost::asio::io_service& io_service, ServerSharedPtr server) :
    set(io_service),
    server(server)
{
    set.add(SIGINT);

    asyncWait();
}

void Signals::asyncWait()
{
    set.async_wait([this] (boost::system::error_code err, int signal) {
        if (err) {
            g_logger.error("Signal handling error: " + err.message());
            return;
        }
        dispatchSignalHandler(signal);
        asyncWait();
    });
}

void Signals::dispatchSignalHandler(int signal)
{
    switch(signal) {
        case SIGINT: //Shuts the server down
            sigintHandler();
            break;
        default:
            break;
    }
}

void Signals::sigintHandler()
{
    g_logger.info("Gracefully stopping...");
    server.get()->close();
    g_connectionManager.closeAll();
    g_logger.shutdown();
}
