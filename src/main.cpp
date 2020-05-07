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

#include "definitions.h"
#include "server.h"
#include "logger.h"
#include "rsa.h"
#include "database.h"
#include "config.h"
#include "signals.h"

[[noreturn]] void badAllocationHandler() {
    // Use functions that only use stack allocation
    puts("Allocation failed, server out of memory.\nTry to compile in 64 bits mode.\n");
    getchar();
    exit(-1);
}

bool mainLoader();

int main(int argc, char* argv[]) {
    // Setup bad allocation handler
    std::set_new_handler(badAllocationHandler);

    g_logger.start();

    if (mainLoader()) {
        const auto ip = g_config.getString(ConfigString::ConnectionIP);
        const auto port = g_config.getInteger(ConfigInteger::ConnectionPort);

        boost::asio::io_service io_service;
        ServerSharedPtr server = std::make_shared<Server>(io_service);
        Signals signals(io_service, server);
        server.get()->open(ip, port);
    } else {
        g_logger.fatal("The login server IS NOT online!");
        g_logger.shutdown();
    }

    g_logger.join();

    return 0;
}

bool mainLoader() {

#ifdef _WIN32
    SetConsoleTitle(SERVER_NAME);
#endif
    g_logger.info("Starting " + std::string(SERVER_NAME) + " Version " + SERVER_VERSION);
    g_logger.info("Compiled with " + std::string(BOOST_COMPILER));
    g_logger.info("Compiled on " + std::string(__DATE__) + ' ' + __TIME__);

#if defined(__amd64__) || defined(_M_X64)
    g_logger.info("Platform x64");
#elif defined(__i386__) || defined(_M_IX86) || defined(_X86_)
    g_logger.info("Platform x86");
#elif defined(__arm__)
    g_logger.info("Platform ARM");
#else
    g_logger.info("Platform unknown");
#endif

    g_logger.info("Loading config.yaml");
    try {
        g_config.load();
    } catch(const std::exception& e) {
        g_logger.error("Failed to load config.yaml: " + std::string(e.what()));
        return false;
    }

    g_logger.info("Loading key.pem");
    try {
        g_RSA.loadPEM();
    } catch(const std::exception& e) {
        g_logger.error("Failed to load key.pem: " + std::string(e.what()));
        return false;
    }

    g_logger.info("Establishing database connection...");
    try {
        g_database.connect();
        g_logger.info("MySQL " + g_database.getVersion());
    } catch (const std::exception& e) {
        g_logger.error("Failed to connect to database: " + std::string(e.what()));
        return false;
    }

    return true;
}
