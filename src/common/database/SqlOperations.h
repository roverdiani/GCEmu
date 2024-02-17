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

#ifndef GCEMU_SQLOPERATIONS_H
#define GCEMU_SQLOPERATIONS_H

#include "MySqlConnection.h"
#include <memory>

class SqlOperation
{
public:
    virtual bool Execute(std::shared_ptr<MySqlConnection> connection) = 0;
};

class SqlTransaction : public SqlOperation
{
    bool Execute(std::shared_ptr<MySqlConnection> connection) override
    {
        if (m_queue.empty())
            return true;

        // TODO: properly lock

        connection->BeginTransaction();
        for (auto& statement : m_queue)
        {
            if (!statement->Execute(connection))
            {
                connection->RollbackTransaction();
                return false;
            }
        }
        return connection->CommitTransaction();
    }

private:
    std::vector<std::shared_ptr<SqlOperation>> m_queue;
};

#endif //GCEMU_SQLOPERATIONS_H
