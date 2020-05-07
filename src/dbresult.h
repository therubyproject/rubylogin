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

#ifndef DB_RESULT
#define DB_RESULT

class DBResult {
	public:
		explicit DBResult(MYSQL_RES* res);
		~DBResult() = default;

		// non-copyable
		DBResult(const DBResult&) = delete;
		DBResult& operator=(const DBResult&) = delete;

        void next() {
            index++;
        }
        bool hasNext() {
            return (index < rows.size());
        }

        std::string getString(const std::string& field);

        template<typename T>
        T getNumber(const std::string& field)
        {
            if (rows.empty() || rows.size() < index) {
                return static_cast<T>(0);
            }

            auto it = fields.find(field);
            if (it == fields.end()) {
                // field name doesnt exist
                return static_cast<T>(0);
            }

            auto row = rows[index];
            T value = boost::lexical_cast<T>(row[it->second]);
            if (!value) {
                return static_cast<T>(0);
            }

            return value;
        }

    private:
        std::map<std::string, size_t> fields;
        std::vector<MYSQL_ROW> rows;        
        size_t index = 0;
};

#endif
