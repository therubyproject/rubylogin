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

#ifndef CONFIG_H
#define CONFIG_H

enum class ConfigBoolean {
    Last
};

enum class ConfigString {
    ConnectionIP,
    DatabaseIP,
    DatabaseUser,
    DatabasePassword,
    DatabaseName,
    DatabaseSocket,
    EncryptionSalt,
    MOTDMessage,
    VersionStr,

    Last
};

enum class ConfigInteger {
    DatabasePort,
    ConnectionPort,
    MOTDNumber,
    VersionMin,

    Last
};

enum class ConfigFloating {
    Last
};

struct World {
    uint16_t id;
    uint16_t port;
    std::string name;
    std::string ip;
};

class Config {
    public:
        void load();
        
        const std::string& getString(ConfigString key) const;
        int64_t getInteger(ConfigInteger key) const;
        bool getBoolean(ConfigBoolean key) const;
        float getFloat(ConfigFloating key) const;

        std::vector<World> getWorlds() const {
            return worlds;
        }

    private:
        void setString(ConfigString key, const YAML::Node& node, std::string standard);
        void setInteger(ConfigInteger key, const YAML::Node& node, int64_t standard);
        void setBoolean(ConfigBoolean key, const YAML::Node& node, bool standard);
        void setFloating(ConfigFloating key, const YAML::Node& node, float standard);

        void addWorld(World world) {
            worlds.push_back(world);
        }

        const std::string getKeyName(ConfigString key);
        const std::string getKeyName(ConfigInteger key);
        const std::string getKeyName(ConfigBoolean key);
        const std::string getKeyName(ConfigFloating key);

        std::string string[static_cast<int32_t>(ConfigString::Last)] = {};
        int32_t integer[static_cast<int32_t>(ConfigInteger::Last)] = {};
        bool boolean[static_cast<int32_t>(ConfigBoolean::Last)] = {};
        float floating[static_cast<int32_t>(ConfigFloating::Last)] = {};

        std::vector<World> worlds;
};

extern Config g_config;

#endif
