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

#ifndef GCEMU_DATABASE_H
#define GCEMU_DATABASE_H

#include "MySqlConnection.h"
#include "SqlOperations.h"
#include <cstdint>
#include <memory>
#include <string>
#include <boost/thread/tss.hpp>

#define QUERY_CONNECTION_POOL_MIN_SIZE 1
#define QUERY_CONNECTION_POOL_MAX_SIZE 16

class Database
{
public:
    Database();

    bool Initialize(const std::string& info, uint32_t numConnections = 1);

    bool Execute(const std::string& sql);
    bool PreparedExecute(const char* format, ...);
    std::unique_ptr<QueryResult> PreparedQuery(const char* format, ...);

private:
    std::vector<std::shared_ptr<MySqlConnection>> m_queryConnections;
    uint32_t m_queryConnectionPoolSize = 1;

    std::shared_ptr<MySqlConnection> m_asyncConnection;

    boost::thread_specific_ptr<SqlTransaction> m_currentTransaction;

    static size_t m_databaseCount;
};

#endif //GCEMU_DATABASE_H
