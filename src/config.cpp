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

#include "config.h"
#include "logger.h"

Config g_config;

void Config::load()
{
    YAML::Node config = YAML::LoadFile("config.yaml");

    const YAML::Node& connection = config["connection"];
    const YAML::Node& database = config["database"];
    const YAML::Node& encryption = config["encryption"];
    const YAML::Node& logging = config["logging"];
    const YAML::Node& motd = config["motd"];
    const YAML::Node& worlds = config["worlds"];

    setString(ConfigString::ConnectionIP, connection["ip"], "0.0.0.0");
    setString(ConfigString::DatabaseIP, database["ip"], "127.0.0.1");
    setString(ConfigString::DatabaseUser, database["user"], "root");
    setString(ConfigString::DatabasePassword, database["password"], "");
    setString(ConfigString::DatabaseName, database["name"], "ruby");
    setString(ConfigString::DatabaseSocket, database["socket"], "");
    setString(ConfigString::EncryptionSalt, encryption["salt"], "");
    setString(ConfigString::MOTDMessage, motd["message"], "");
    setString(ConfigString::VersionStr, connection["version_str"], "");

    setInteger(ConfigInteger::ConnectionPort, connection["port"], 7171);
    setInteger(ConfigInteger::DatabasePort, database["port"], 3306);
    setInteger(ConfigInteger::MOTDNumber, motd["number"], 0);
    setInteger(ConfigInteger::VersionMin, connection["version_min"], 0);

    if (!worlds.IsSequence()) {
        throw std::runtime_error("worlds must be a list!");
    }

    for (auto it = worlds.begin(); it != worlds.end(); ++it) {
        const YAML::Node& worldNode = *it;

        World world;
        world.id = worldNode["id"].as<uint16_t>();
        world.port = worldNode["port"].as<uint16_t>();
        world.name = worldNode["name"].as<std::string>();
        world.ip = worldNode["ip"].as<std::string>();

        addWorld(world);
    }
}

static std::string dummy;

const std::string& Config::getString(ConfigString key) const
{
    return string[static_cast<int32_t>(key)];
}

int64_t Config::getInteger(ConfigInteger key) const
{
    return integer[static_cast<int32_t>(key)];
}

bool Config::getBoolean(ConfigBoolean key) const
{
    return boolean[static_cast<int32_t>(key)];
}

float Config::getFloat(ConfigFloating key) const
{
    return floating[static_cast<int32_t>(key)];
}

void Config::setString(ConfigString key, const YAML::Node& node, std::string standard) {
    try {
        string[static_cast<int32_t>(key)] = (node ? node.as<std::string>() : standard);
    } catch (YAML::TypedBadConversion<std::string> &e) {
        g_logger.warning("Failed to set config " + getKeyName(key) + ": " + e.what());
    }
}

void Config::setInteger(ConfigInteger key, const YAML::Node& node, int64_t standard) {
    try {
        integer[static_cast<int32_t>(key)] = (node ? node.as<int64_t>() : standard);
    } catch (YAML::TypedBadConversion<int64_t> &e) {
        g_logger.warning("Failed to set config " + getKeyName(key) + ": " + e.what());
    }
}

void Config::setBoolean(ConfigBoolean key, const YAML::Node& node, bool standard) {
    try {
        boolean[static_cast<int32_t>(key)] = (node ? node.as<bool>() : standard);
    } catch (YAML::TypedBadConversion<bool> &e) {
        g_logger.warning("Failed to set config " + getKeyName(key) + ": " + e.what());
    }
}

void Config::setFloating(ConfigFloating key, const YAML::Node& node, float standard) {
    try {
        floating[static_cast<int32_t>(key)] = (node ? node.as<float>() : standard);
    } catch (YAML::TypedBadConversion<float> &e) {
        g_logger.warning("Failed to set config " + getKeyName(key) + ": " + e.what());
    }
}

const std::string Config::getKeyName(ConfigString key)
{
    switch(key) {
        case ConfigString::ConnectionIP:
            return "connection ip";
        case ConfigString::DatabaseIP:
            return "database ip";
        case ConfigString::DatabaseUser:
            return "database user";
        case ConfigString::DatabasePassword:
            return "database password";
        case ConfigString::DatabaseName:
            return "database name";
        case ConfigString::DatabaseSocket:
            return "database socket";
        case ConfigString::EncryptionSalt:
            return "encryption salt";
        case ConfigString::VersionStr:
            return "connection version str";
        case ConfigString::MOTDMessage:
            return "motd message";
        default:
            return "unknown";
    }
}

const std::string Config::getKeyName(ConfigInteger key)
{
    switch(key) {
        case ConfigInteger::DatabasePort:
            return "database port";
        case ConfigInteger::ConnectionPort:
            return "connection port";
        case ConfigInteger::VersionMin:
            return "connection version min";
        case ConfigInteger::MOTDNumber:
            return "motd number";
        default:
            return "unknown";
    }
}

const std::string Config::getKeyName(ConfigBoolean key)
{
    switch(key) {
        default:
            return "unknown";
    }
}

const std::string Config::getKeyName(ConfigFloating key)
{
    switch(key) {
        default:
            return "unknown";
    }
}
