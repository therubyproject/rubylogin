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

#include "database.h"
#include "logger.h"
#include "config.h"
#include "tools.h"

Database g_database;

Database::~Database()
{
    if (handle) {
        mysql_close(handle);
    }
}

void Database::connect()
{
    handle = mysql_init(nullptr);
    if (!handle) {
        throw std::runtime_error("Failed to initialize MySQL connection handle.");
    }

    bool reconnect = true;
    mysql_options(handle, MYSQL_OPT_RECONNECT, &reconnect);

    bool result = mysql_real_connect(handle, 
                g_config.getString(ConfigString::DatabaseIP).c_str(),
                g_config.getString(ConfigString::DatabaseUser).c_str(),
                g_config.getString(ConfigString::DatabasePassword).c_str(),
                g_config.getString(ConfigString::DatabaseName).c_str(),
                g_config.getInteger(ConfigInteger::DatabasePort),
                g_config.getString(ConfigString::DatabaseSocket).c_str(), 0);

    if (!result) {
        throw std::runtime_error(std::string(mysql_error(handle)));
    }
}

DBResultSharedPtr Database::storeQuery(const std::string& query)
{
    databaseLock.lock();
    if (!!mysql_real_query(handle, query.c_str(), query.length())) {
        g_logger.error("[mysql_real_query]: " + std::string(mysql_error(handle)));
        databaseLock.unlock();
        return nullptr;
    }

    auto result = mysql_store_result(handle);
    if (!result) {
        g_logger.error("[mysql_store_result]: " + std::string(mysql_error(handle)));
        databaseLock.unlock();
        return nullptr;
    }
    databaseLock.unlock();

    DBResultSharedPtr res = std::make_shared<DBResult>(result);
    mysql_free_result(result);

    return res->hasNext() ? res : nullptr;
}

std::string Database::escapeString(const std::string& string) const
{
    // the worst case is 2n + 1
    size_t length = string.length();
    size_t maxLength = (length * 2) + 1;

    std::string escaped;
    escaped.reserve(maxLength + 2);
    escaped.push_back('\'');

    if (length != 0) {
        char* output = new char[maxLength];
        mysql_real_escape_string(handle, output, string.c_str(), length);
        escaped.append(output);
        delete[] output;
    }

    escaped.push_back('\'');
    return escaped;
}

Account Database::getAccount(const std::string& name, const std::string& password)
{
    Account account;

    auto accountInfo = getAccountInfo(name, password);
    if (!accountInfo) {
        return account;
    }

    account.name = name;
    account.password = password;
    account.id = accountInfo->getNumber<uint16_t>("id");
    account.premiumEnd = accountInfo->getNumber<uint64_t>("premend");
    account.characters = getCharacterList(account.id);

    return account;
}

DBResultSharedPtr Database::getAccountInfo(const std::string& name, const std::string& password)
{
    std::string hashPass = transformToSHA1(g_config.getString(ConfigString::EncryptionSalt) + password);
    auto result = storeQuery("SELECT `id`, `name`, `password`, `premend` FROM `accounts` WHERE `name` = " + escapeString(name) + " AND password = " + escapeString(hashPass));    
    return result;
}

std::vector<Character> Database::getCharacterList(uint16_t accountId)
{
    std::vector<Character> characters;
    
    auto result = storeQuery("SELECT `name`, `account_id`, `world_id` FROM `players` WHERE `account_id` = '" + std::to_string(accountId) + "'");
    if (result) {
        while(result->hasNext()) {
            Character character;
            character.worldId = result->getNumber<uint16_t>("world_id");
            character.name = result->getString("name");
            characters.push_back(character);
            result->next();
        }
    }
    return characters;
} 
