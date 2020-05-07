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

#ifndef LOGGER_H
#define LOGGER_H

#include "threadholder.h"
#include "log.h"

class Logger : public ThreadHolder<Logger> {
    public:
        Logger();
        ~Logger();

        // non-copyable
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        void shutdown();
        void writeLog(Log* log);

        void addLog(Log* log);

        void threadMain();

        void log(const std::string& message);
        void trace(const std::string& message);
        void debug(const std::string& message);
        void info(const std::string& message);
        void warning(const std::string& message);
        void error(const std::string& message);
        void fatal(const std::string& message);

    private:
        static void fileFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm);
        static void consoleFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm);
        static void formatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm, bool colored);

        std::mutex logLock;
        std::mutex coutLock;
        std::condition_variable logSignal;

        std::list<Log*> logList;

        typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;
        boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
        boost::log::sources::severity_logger<boost::log::trivial::severity_level> lg;
};

extern Logger g_logger;

#endif