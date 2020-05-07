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

#include "protocol.h"
#include "outputmessage.h"
#include "rsa.h"
#include "xtea.h"
#include "database.h"
#include "config.h"
#include "logger.h"

void Protocol::addMOTD(OutputMessage& msg)
{
    msg.addByte(0x14);
    std::ostringstream ss;
    ss << g_config.getInteger(ConfigInteger::MOTDNumber) << "\n";
    ss << g_config.getString(ConfigString::MOTDMessage);
    msg.addString(ss.str());
}

void Protocol::addSessionKey(OutputMessage& msg)
{
    msg.addByte(0x28);
    msg.addString(account.name + "\n" + account.password);
}

void Protocol::addCharacterList(OutputMessage& msg)
{
    msg.addByte(0x64);

    auto worlds = g_config.getWorlds();
    msg.addByte(worlds.size()); // number of worlds
    for (auto world : worlds) {
        msg.addByte(world.id); // world id
        msg.addString(world.name);
        msg.addString(world.ip);
        msg.add<uint16_t>(world.port);
        msg.addByte(0); // beta flag
    }

    auto characters = account.characters;
    msg.addByte(characters.size()); // number of chars
    for (auto character : characters) {
        msg.addByte(character.worldId); // world id
        msg.addString(character.name);
    }

     //Add premium days
    msg.add<uint32_t>(account.premiumEnd); // time left in seconds
}

void Protocol::authenticate(NetworkMessage& msg)
{
    msg.skipBytes(2); // operating system

    uint16_t version = msg.get<uint16_t>();

    msg.skipBytes(17);
    /*
     * Skipped bytes:
     * 4 bytes: protocolVersion
     * 12 bytes: dat, spr, pic signatures (4 bytes each)
     * 1 byte: 0
     */

    if (!g_RSA.decrypt(msg)) {
        disconnect();
        return;
    }

    uint32_t key[4];
    key[0] = msg.get<uint32_t>();
    key[1] = msg.get<uint32_t>();
    key[2] = msg.get<uint32_t>();
    key[3] = msg.get<uint32_t>();
    setXTEAKey(key);

    uint16_t versionMin = g_config.getInteger(ConfigInteger::VersionMin);
    if (version < versionMin) {
        std::string versionStr = g_config.getString(ConfigString::VersionStr);
        disconnectClient("Only clients with protocol " + versionStr + " allowed!");
        return;
    }

    std::string name = msg.getString();
    if (name.empty()) {
        disconnectClient("Invalid account name.");
        return;
    }

    std::string password = msg.getString();
    if (password.empty()) {
        disconnectClient("Invalid password.");
        return;
    }

    account = g_database.getAccount(name, password);
    if (!account.id) {
        disconnectClient("Invalid account name or password.");
        return;
    }

    if (account.characters.empty()) {
        disconnectClient("Please create a character.");
        return;
    }

    OutputMessage output;

    addMOTD(output);
    addSessionKey(output);
    addCharacterList(output);

    send(output);
}

void Protocol::disconnectClient(const std::string& message) const
{
    OutputMessage msg;
    msg.addByte(0x0B);
    msg.addString(message);
    send(msg);
    disconnect();
}

void Protocol::encryptMessage(OutputMessage& msg)
{
    msg.writeMessageLength();
    g_XTEA.encrypt(key, msg);
    msg.addCryptoHeader();
}

uint32_t Protocol::getIP() const
{
    if (auto connection = getConnection()) {
        return connection->getIP();
    }

    return 0;
}
