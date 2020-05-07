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

#ifndef DATABASE_H
#define DATABASE_H

#include "dbresult.h"
#include "types.h"

struct Character {
    uint16_t worldId;
    std::string name;
};

struct Account {
    uint16_t id = 0;
    std::string name;
    std::string password;
    uint64_t premiumEnd;
    std::vector<Character> characters;
};

class Database
{
    public:
        Database() = default;
        ~Database();

        // non-copyable
        Database(const Database&) = delete;
        Database& operator=(const Database&) = delete;

        void connect();
        void disconnect();
        DBResultSharedPtr storeQuery(const std::string& query);

        std::string getVersion() const {
            return mysql_get_client_info();
        }

        std::string escapeString(const std::string& string) const;

        Account getAccount(const std::string& name, const std::string& password);
    
    private:
        DBResultSharedPtr getAccountInfo(const std::string& name, const std::string& password);
        std::vector<Character> getCharacterList(uint16_t accountId);

        MYSQL* handle = nullptr;
        std::recursive_mutex databaseLock;
};

extern Database g_database;

#endif
