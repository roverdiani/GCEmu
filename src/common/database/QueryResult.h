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

#ifndef GCEMU_QUERYRESULT_H
#define GCEMU_QUERYRESULT_H

#include "DatabaseField.h"
#include <cstdint>
#include <vector>
#include <mysql/mysql.h>

class QueryResult
{
public:
    QueryResult(MYSQL_RES* result, MYSQL_FIELD* fields, uint64_t rowCount, uint32_t fieldCount) : m_rowCount(rowCount),
    m_fieldCount(fieldCount), m_result(result)
    {
        m_currentRow = std::vector<DatabaseField>(m_fieldCount);
        for (uint32_t i = 0; i < m_fieldCount; i++)
            m_currentRow[i].SetType(ConvertNativeType(fields[i].type));
    }

    bool NextRow()
    {
        if (!m_result)
            return false;

        MYSQL_ROW row = mysql_fetch_row(m_result);
        if (!row)
        {
            EndQuery();
            return false;
        }

        for (uint32_t i = 0; i < m_fieldCount; i++)
            m_currentRow[i].SetValue(row[i]);

        return true;
    }

private:
    void EndQuery()
    {
        m_currentRow.clear();
        mysql_free_result(m_result);
        m_result = nullptr;
    }

    static enum DatabaseField::DataTypes ConvertNativeType(enum_field_types mysqlType)
    {
        switch (mysqlType)
        {
            case FIELD_TYPE_TIMESTAMP:
            case FIELD_TYPE_DATE:
            case FIELD_TYPE_TIME:
            case FIELD_TYPE_DATETIME:
            case FIELD_TYPE_YEAR:
            case FIELD_TYPE_STRING:
            case FIELD_TYPE_VAR_STRING:
            case FIELD_TYPE_BLOB:
            case FIELD_TYPE_SET:
            case FIELD_TYPE_NULL:
                return DatabaseField::DB_TYPE_STRING;
            case FIELD_TYPE_TINY:
            case FIELD_TYPE_SHORT:
            case FIELD_TYPE_LONG:
            case FIELD_TYPE_INT24:
            case FIELD_TYPE_LONGLONG:
            case FIELD_TYPE_ENUM:
                return DatabaseField::DB_TYPE_INTEGER;
            case FIELD_TYPE_DECIMAL:
            case FIELD_TYPE_FLOAT:
            case FIELD_TYPE_DOUBLE:
                return DatabaseField::DB_TYPE_FLOAT;
            default:
                return DatabaseField::DB_TYPE_UNKNOWN;
        }
    }

    std::vector<DatabaseField> m_currentRow;
    uint32_t m_fieldCount;
    uint64_t m_rowCount;
    MYSQL_RES* m_result;
};

#endif //GCEMU_QUERYRESULT_H
