// This file is part of the GCEmu Project.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef GCEMU_CONFIGHANDLER_H
#define GCEMU_CONFIGHANDLER_H

#include <cstdint>
#include <mutex>
#include <nlohmann/json.hpp>

#define SConfigHandler ConfigHandler::GetInstance()

class ConfigHandler
{
public:
    static ConfigHandler& GetInstance()
    {
        static ConfigHandler instance;
        return instance;
    }

    ConfigHandler(ConfigHandler const&) = delete;
    void operator=(ConfigHandler const&) = delete;

    bool SetConfigFile(const std::string& file);
    bool Reload();

    bool IsSet(const std::string& keyName) const;
    std::string GetString(const std::string& keyName, const std::string& defaultValue) const;
    bool GetBool(const std::string& keyName, bool defaultValue) const;
    int32_t GetInt(const std::string& keyName, int32_t defaultValue) const;

private:
    ConfigHandler() {}

    std::string m_filename;
    nlohmann::json m_data;

    std::mutex m_configHandlerLock;
};

#endif //GCEMU_CONFIGHANDLER_H
