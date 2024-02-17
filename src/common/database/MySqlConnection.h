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

#ifndef GCEMU_MYSQLCONNECTION_H
#define GCEMU_MYSQLCONNECTION_H

#include "QueryResult.h"
#include <memory>
#include <string>
#include <mysql.h>

class MySqlConnection
{
public:
    bool Initialize(const std::string& connectionInfo);

    bool BeginTransaction();
    bool CommitTransaction();
    bool RollbackTransaction();

    bool Execute(const std::string& sql);
    std::unique_ptr<QueryResult> Query(const std::string& sql);

private:
    bool TransactionCommand(const std::string& sql);
    bool ProcessQuery(const std::string &sql, MYSQL_RES **pResult, MYSQL_FIELD **pFields, uint64_t *pRowCount, uint32_t *pFieldCount);

    MYSQL* m_mySql;
};

#endif //GCEMU_MYSQLCONNECTION_H
