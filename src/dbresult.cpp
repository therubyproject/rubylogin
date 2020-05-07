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

#include "includes.h"

#include "dbresult.h"

DBResult::DBResult(MYSQL_RES* res)
{
    size_t i = 0;

    auto field = mysql_fetch_field(res);
    while (field) {
        fields[field->name] = i++;
        field = mysql_fetch_field(res);
    }

    auto row = mysql_fetch_row(res);
    while (row) {
        rows.push_back(row);
        row = mysql_fetch_row(res);
    }
}

std::string DBResult::getString(const std::string& field)
{
    if (rows.empty() || rows.size() < index) {
        return "";
    }

    auto it = fields.find(field);
    if (it == fields.end()) {
        // field name doesnt exist
        return "";
    }

    auto row = rows[index];
    if (row[it->second] == nullptr) {
        return "";
    }

    return std::string(row[it->second]);
}
