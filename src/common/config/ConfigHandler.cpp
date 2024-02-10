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

#include "ConfigHandler.h"
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>

bool ConfigHandler::SetConfigFile(const std::string &file)
{
    std::filesystem::path configFilePath = file;
    m_filename = file;

    return Reload();
}

bool ConfigHandler::Reload()
{
    std::ifstream ifs(m_filename, std::ifstream::in);
    if (ifs.fail())
        return false;

    std::lock_guard<std::mutex> lock(m_configHandlerLock);

    m_data = nlohmann::json::parse(ifs, nullptr, false);
    if (m_data.is_discarded())
    {
        spdlog::error("ConfigHandler::Reload: JSON parse error.");
        return false;
    }

    return true;
}

bool ConfigHandler::IsSet(const std::string &keyName) const
{
    return m_data.contains(keyName);
}

std::string ConfigHandler::GetString(const std::string &keyName, const std::string &defaultValue) const
{
    if (!IsSet(keyName))
        return defaultValue;

    return m_data[keyName];
}

bool ConfigHandler::GetBool(const std::string &keyName, bool defaultValue) const
{
    if (!IsSet(keyName))
        return defaultValue;

    return m_data[keyName];
}

int32_t ConfigHandler::GetInt(const std::string &keyName, int32_t defaultValue) const
{
    if (!IsSet(keyName))
        return defaultValue;

    return m_data[keyName];
}
