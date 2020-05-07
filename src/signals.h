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

#ifndef SIGNALS_H
#define SIGNALS_H

#include "server.h"

class Signals
{
    boost::asio::signal_set set;
    ServerSharedPtr server;
    public:
        explicit Signals(boost::asio::io_service& io_service, ServerSharedPtr server);

    private:
        void asyncWait();
        void dispatchSignalHandler(int signal);

        void sigintHandler();
};

#endif
