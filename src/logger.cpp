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

#include "logger.h"
#include "config.h"
#include "tools.h"

Logger g_logger;

Logger::~Logger()
{
    for (auto log : logList) {
        writeLog(log);
        delete log;
    }
}

Logger::Logger()
{
    //boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");
    boost::log::add_common_attributes();

    boost::log::add_console_log()->set_formatter(&Logger::consoleFormatter);
    boost::log::add_file_log
    (
        boost::log::keywords::file_name = "logs/ruby_%m_%d_%Y.log",
        boost::log::keywords::open_mode = std::ios_base::app|std::ios_base::out,
        boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(boost::gregorian::greg_day(1)),
        boost::log::keywords::auto_flush = true
    )->set_formatter(&Logger::fileFormatter);
}

void Logger::threadMain()
{
    std::unique_lock<std::mutex> logLockUnique(logLock, std::defer_lock);
    while (getState() != ThreadState::Terminated) {
        logLockUnique.lock();
        if (logList.empty()) {
            logSignal.wait(logLockUnique);
        }

        if (!logList.empty()) {
            Log* log = std::move(logList.front());
            logList.pop_front();
            logLockUnique.unlock();
            writeLog(log);
            delete log;
        } else {
            logLockUnique.unlock();
        }
    }
}

void Logger::addLog(Log* log)
{
    logLock.lock();

    if (getState() != ThreadState::Running) {
        delete log;
        logLock.unlock();
        return;
    }

    // if the list is empty we have to signal it
    bool do_signal = logList.empty();

    // add log to the queue
    logList.push_back(log);

    logLock.unlock();

    if (do_signal) {
        logSignal.notify_one();
    }
}

void Logger::shutdown()
{
    logLock.lock();
    setState(ThreadState::Terminated);
    logLock.unlock();
    logSignal.notify_one();
}

void Logger::writeLog(Log* log)
{
    switch (log->getSeveretyLevel()) {
        case SeveretyLevel::Trace:
            BOOST_LOG_TRIVIAL(trace) << log->getMessage();
            break;
        case SeveretyLevel::Debug:
            BOOST_LOG_TRIVIAL(debug) << log->getMessage();
            break;
        case SeveretyLevel::Info:
            BOOST_LOG_TRIVIAL(info) << log->getMessage();
            break;
        case SeveretyLevel::Warning:
            BOOST_LOG_TRIVIAL(warning) << log->getMessage();
            break;
        case SeveretyLevel::Error:
            BOOST_LOG_TRIVIAL(error) << log->getMessage();
            break;
        case SeveretyLevel::Fatal:
            BOOST_LOG_TRIVIAL(fatal) << log->getMessage();
            break;
    }
}

void Logger::trace(const std::string& message)
{
    addLog(new Log(SeveretyLevel::Trace, message));
}

void Logger::debug(const std::string& message)
{
    addLog(new Log(SeveretyLevel::Debug, message));
}

void Logger::info(const std::string& message)
{
    addLog(new Log(SeveretyLevel::Info, message));
}

void Logger::warning(const std::string& message)
{
    addLog(new Log(SeveretyLevel::Warning, message));
}

void Logger::error(const std::string& message)
{
    addLog(new Log(SeveretyLevel::Error, message));
}

void Logger::fatal(const std::string& message)
{
    addLog(new Log(SeveretyLevel::Fatal, message));
}

void Logger::fileFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm)
{
    Logger::formatter(rec, strm, false);
}

void Logger::consoleFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm)
{
    Logger::formatter(rec, strm, true);
}

void Logger::formatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm, bool colored)
{
    static auto dateTimeFormatter = boost::log::expressions::format_date_time<boost::posix_time::ptime >("TimeStamp", "%H:%M:%S.%f ");

    std::string color;
    std::string type;

    auto severity = rec[boost::log::trivial::severity];
    switch (*severity) {
        case boost::log::trivial::trace:
            color = "\033[1;36m";
            type = "TRACE";
            break;
        case boost::log::trivial::debug:
            color = "\033[1;36m";
            type = "DEBUG";
            break;
        case boost::log::trivial::info:
            color = "\033[1;37m";
            type = "INFO";
            break;
        case boost::log::trivial::warning:
            color = "\033[1;33m";
            type = "WARN";
            break;
        case boost::log::trivial::error:
            color = "\033[1;31m";
            type = "ERROR";
            break;
        case boost::log::trivial::fatal:
            color = "\033[1;31m";
            type = "FATAL";
            break;
        default:
            break;
    }

    if (!colored)
        color = "";

    strm << color
         << "["
         << dateTimeFormatter(rec, strm)
         << type
         << "]: "
         << rec[boost::log::expressions::message];
}
