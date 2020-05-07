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

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "connection.h"
#include "database.h"

class Protocol : public std::enable_shared_from_this<Protocol>
{
    public:
        explicit Protocol(ConnectionSharedPtr connection) : connection(connection) {}
        ~Protocol() = default;

        // non-copyable
        Protocol(const Protocol&) = delete;
        Protocol& operator=(const Protocol&) = delete;

        void encryptMessage(OutputMessage& msg);
        void authenticate(NetworkMessage& msg);

        ConnectionSharedPtr getConnection() const {
            return connection.lock();
        }

        uint32_t getIP() const;

        void send(OutputMessage& msg) const {
            if (auto connection = getConnection()) {
                connection->send(msg);
            }
        }

        void disconnectClient(const std::string& message) const;

    private:
        void addMOTD(OutputMessage& msg);
        void addSessionKey(OutputMessage& msg);
        void addCharacterList(OutputMessage& msg);
        void disconnect() const {
            if (auto connection = getConnection()) {
                connection->close();
            }
        }
        void setXTEAKey(const uint32_t* key) {
            memcpy(this->key, key, sizeof(*key) * 4);
        }

        friend class Connection;
    private:
        const ConnectionWeakPtr connection;
        uint32_t key[4] = {};
        Account account;
};

#endif
