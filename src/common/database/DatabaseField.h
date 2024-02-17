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

#ifndef GCEMU_DATABASEFIELD_H
#define GCEMU_DATABASEFIELD_H

#include <cstdint>
#include <string>

class DatabaseField
{
public:
    enum DataTypes
    {
        DB_TYPE_UNKNOWN = 0x00,
        DB_TYPE_STRING  = 0x01,
        DB_TYPE_INTEGER = 0x02,
        DB_TYPE_FLOAT   = 0x03,
        DB_TYPE_BOOL    = 0x04,
    };

    DatabaseField() = default;
    DatabaseField(const char* value, enum DataTypes type) : m_value(value), m_type(type)
    {
    }

    DatabaseField(DatabaseField const&) = delete;
    DatabaseField& operator =(DatabaseField const&) = delete;

    enum DataTypes GetType() const
    {
        return m_type;
    }

    bool IsNull() const
    {
        return m_value == nullptr;
    }

    const char* GetString() const
    {
        // Null check here to avoid further null checks when using this method
        return m_value ? m_value : "";
    }

    std::string GetCppString() const
    {
        return m_value ? m_value : std::string();
    }

    float GetFloat() const
    {
        return m_value ? static_cast<float>(strtod(m_value, nullptr)) : 0.0f;
    }

    bool GetBool() const
    {
        return m_value != nullptr && strtol(m_value, nullptr, 10) > 0;
    }

    uint8_t GetUInt8() const
    {
        return m_value ? static_cast<uint8_t>(strtol(m_value, nullptr, 10)) : (uint8_t) 0;
    }

    int16_t GetInt16() const
    {
        return m_value ? static_cast<int16_t >(strtol(m_value, nullptr, 10)) : (int16_t) 0;
    }

    uint16_t GetUInt16() const
    {
        return m_value ? static_cast<uint16_t>(strtol(m_value, nullptr, 10)) : (uint16_t) 0;
    }

    int32_t GetInt32() const
    {
        return m_value ? static_cast<int32_t>(strtol(m_value, nullptr, 10)) : (int32_t) 0;
    }

    uint32_t GetUInt32() const
    {
        return m_value ? static_cast<uint32_t>(strtol(m_value, nullptr, 10)) : (uint32_t) 0;
    }

    int64_t GetInt64() const
    {
        return m_value ? static_cast<int64_t>(strtoll(m_value, nullptr, 10)) : (int64_t) 0;
    }

    uint64_t GetUInt64() const
    {
        return m_value ? static_cast<uint64_t>(strtoll(m_value, nullptr, 10)) : (uint64_t) 0;
    }

    void SetType(enum DataTypes type)
    {
        m_type = type;
    }

    void SetValue(const char* value)
    {
        m_value = value;
    }

private:
    const char* m_value = nullptr;
    enum DataTypes m_type = DB_TYPE_UNKNOWN;
};

#endif //GCEMU_DATABASEFIELD_H
