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

#include "MySqlConnection.h"
#include "../util/StringUtil.h"
#include <spdlog/spdlog.h>

bool MySqlConnection::Initialize(const std::string &connectionInfo)
{
    MYSQL* mySqlInit = mysql_init(nullptr);
    if (!mySqlInit)
    {
        spdlog::error("MySqlConnection::Initialize: Error: could not initialize MySQL connection.");
        return false;
    }

    std::vector<std::string> tokens = StringUtil::StringSplit(connectionInfo, ";");

    std::string host, user, password, database;
    int32_t port = 0;

    auto itr = tokens.begin();
    if (itr != tokens.end())
        host = *itr++;
    if (itr != tokens.end())
        port = static_cast<int32_t>(strtol((*itr++).c_str(), nullptr, 10));
    if (itr != tokens.end())
        user = *itr++;
    if (itr != tokens.end())
        password = *itr++;
    if (itr != tokens.end())
        database = *itr++;

    mysql_options(mySqlInit, MYSQL_SET_CHARSET_NAME, "utf8");
    m_mySql = mysql_real_connect(mySqlInit, host.c_str(), user.c_str(), password.c_str(),
                                 database.c_str(), port, nullptr, 0);
    if (!m_mySql)
    {
        spdlog::error("MySQLConnection::Initialize: Error: could not connect to MySQL database at {0}: {1}", host,
                      mysql_error(mySqlInit));
        mysql_close(mySqlInit);
        return false;
    }

    spdlog::info("MySQLConnection::Initialize: Connected to MySQL database {0} as {1}@{2}:{3}", database, user,
                 host, port);
    spdlog::info("MySQLConnection::Initialize: MySQL client library: {0}", mysql_get_client_info());
    spdlog::info("MySQLConnection::Initialize: MySQL server version: {0}", mysql_get_server_info(m_mySql));

    Execute("SET NAMES `utf8`");
    Execute("SET CHARACTER SET `utf8`");

    return true;
}

bool MySqlConnection::BeginTransaction()
{
    return TransactionCommand("START TRANSACTION");
}

bool MySqlConnection::CommitTransaction()
{
    return TransactionCommand("COMMIT");
}

bool MySqlConnection::RollbackTransaction()
{
    return TransactionCommand("ROLLBACK");
}

bool MySqlConnection::Execute(const std::string &sql)
{
    if (!m_mySql)
        return false;

    if (mysql_query(m_mySql, sql.c_str()))
    {
        spdlog::error("SQL: {0}", sql);
        spdlog::error("SQL ERROR: {0}", mysql_error(m_mySql));
        return false;
    }

    spdlog::info("SQL: {0}", sql);

    return true;
}

std::unique_ptr<QueryResult> MySqlConnection::Query(const std::string &sql)
{
    MYSQL_RES* result = nullptr;
    MYSQL_FIELD* fields = nullptr;
    uint64_t rowCount = 0;
    uint32_t fieldCount = 0;

    if (!ProcessQuery(sql, &result, &fields, &rowCount, &fieldCount))
        return nullptr;

    auto queryResult = std::make_unique<QueryResult>(result, fields, rowCount, fieldCount);

    queryResult->NextRow();
    return queryResult;
}

bool MySqlConnection::TransactionCommand(const std::string &sql)
{
    if (mysql_query(m_mySql, sql.c_str()))
    {
        spdlog::error("SQL: {0}", sql);
        spdlog::error("SQL ERROR: {0}", mysql_error(m_mySql));
        return false;
    }

    spdlog::info("SQL: {0}", sql);
    return true;
}

bool
MySqlConnection::ProcessQuery(const std::string &sql, MYSQL_RES **pResult, MYSQL_FIELD **pFields, uint64_t *pRowCount,
                              uint32_t *pFieldCount) {
    if (!m_mySql)
        return false;

    if (mysql_query(m_mySql, sql.c_str()))
    {
        spdlog::error("SQL: {0}", sql);
        spdlog::error("query ERROR: {0}", mysql_error(m_mySql));
        return false;
    }

    *pResult = mysql_store_result(m_mySql);
    *pRowCount = mysql_affected_rows(m_mySql);
    *pFieldCount = mysql_field_count(m_mySql);

    if (!*pResult)
        return false;

    if (!*pRowCount)
    {
        mysql_free_result(*pResult);
        return false;
    }

    *pFields = mysql_fetch_fields(*pResult);
    return true;
}
