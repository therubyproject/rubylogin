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

#ifndef LOG_H
#define LOG_H

enum class SeveretyLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
};

class Log
{
    public:
        Log(SeveretyLevel severetyLevel, const std::string& message) :
            severetyLevel(severetyLevel), message(message) {}
        ~Log() = default;

        SeveretyLevel getSeveretyLevel() {
            return severetyLevel;
        }

        const std::string& getMessage() {
            return message;
        }

    private:
        SeveretyLevel severetyLevel;
        std::string message;
};

#endif
