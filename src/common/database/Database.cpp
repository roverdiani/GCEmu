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

#include "Database.h"
#include <cstdio>
#include <spdlog/spdlog.h>

size_t Database::m_databaseCount = 0;

Database::Database()
{
    // Initialize the MySQL library if this is the first connection
    if (m_databaseCount++ == 0)
        mysql_library_init(-1, nullptr, nullptr);
}

bool Database::Initialize(const std::string& info, uint32_t numConnections)
{
    if (numConnections < QUERY_CONNECTION_POOL_MIN_SIZE)
        m_queryConnectionPoolSize = QUERY_CONNECTION_POOL_MIN_SIZE;
    else if (numConnections > QUERY_CONNECTION_POOL_MAX_SIZE)
        m_queryConnectionPoolSize = QUERY_CONNECTION_POOL_MAX_SIZE;
    else
        m_queryConnectionPoolSize = numConnections;

    for (int32_t i = 0; i < m_queryConnectionPoolSize; i++)
    {
        std::shared_ptr<MySqlConnection> connection = std::make_shared<MySqlConnection>();
        if (!connection->Initialize(info))
            return false;

        m_queryConnections.push_back(connection);
    }

    m_asyncConnection = std::make_shared<MySqlConnection>();
    if (!m_asyncConnection->Initialize(info))
        return false;

    return true;
}

bool Database::Execute(const std::string& sql)
{
    if (!m_asyncConnection)
        return false;

    return m_asyncConnection->Execute(sql);
}

bool Database::PreparedExecute(const char* format, ...)
{
    if (!format)
        return false;

    va_list ap;
    char szQuery [32 * 1024];
    va_start(ap, format);
    int res = vsnprintf(szQuery, 32 * 1024, format, ap);
    va_end(ap);

    if (res == -1)
    {
        spdlog::error("SQL Query truncated (and not execute) for format: {0}", format);
        return false;
    }

    return Execute(szQuery);
}

std::unique_ptr<QueryResult> Database::PreparedQuery(const char* format, ...)
{
    if (!format)
        return {};

    va_list ap;
    char szQuery [32 * 1024];
    va_start(ap, format);
    int res = vsnprintf(szQuery, 32 * 1024, format, ap);
    va_end(ap);

    if (res == -1)
    {
        spdlog::error("SQL Query truncated (and not execute) for format: {0}", format);
        return {};
    }

    return m_queryConnections[0]->Query(szQuery);
}
