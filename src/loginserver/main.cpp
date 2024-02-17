// This file is part of the GCEmu Project.
// This is the main file for the loginserver project.
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

#include "../common/config/ConfigHandler.h"
#include "../common/database/Database.h"
#include "../common/crypto/Security.h"
#include "../common/network/TcpListener.h"
#include "server/LoginSocket.h"
#include <memory>
#include <openssl/opensslv.h>
#include <boost/version.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

bool IsRunning = false;
Database database;

bool InitLogger()
{
    try
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/loginserver.txt", true);
        file_sink->set_level(spdlog::level::trace);
        std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list { console_sink, file_sink });
        spdlog::set_default_logger(logger);
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        return false;
    }

    return true;
}

void OnKillSignal(int i)
{
    spdlog::warn("OnKillSignal: abort requested!");
    spdlog::default_logger()->flush();
    IsRunning = false;
}

int main(int argc, char* argv[])
{
    if (!InitLogger())
        return -1;

    signal(SIGINT, OnKillSignal);
    signal(SIGABRT, OnKillSignal);
    signal(SIGTERM, OnKillSignal);

    spdlog::set_pattern("%v");
    spdlog::info(R"(   ______ ______ ______                 )");
    spdlog::info(R"(  / ____// ____// ____/____ ___   __  __)");
    spdlog::info(R"( / / __ / /    / __/  / __ `__ \ / / / /)");
    spdlog::info(R"(/ /_/ // /___ / /___ / / / / / // /_/ / )");
    spdlog::info(R"(\____/ \____//_____//_/ /_/ /_/ \____/  )");
    spdlog::info("");

    spdlog::info("GCEmu Login Server - <Ctrl-C> to stop");
    spdlog::info("{0}, Boost {1}", OPENSSL_VERSION_TEXT, BOOST_LIB_VERSION);
    spdlog::info("");

    spdlog::set_pattern("[%m/%d/%Y %H:%M:%S] [%l] %v");

    spdlog::info("Server initializing...");

    // Set and parse the configuration file.
    spdlog::info("Loading config file...");
    const std::string configFilename = "loginserver.conf.json";
    if (!SConfigHandler.SetConfigFile(configFilename))
    {
        spdlog::error("Could not load the config file ({0}) - check if it is correct.", configFilename);
        return -1;
    }
    spdlog::info("Config file ({0}) loaded.", configFilename);

    spdlog::info("Initializing OpenSSL...");
    if (!Security::InitOpenSSL())
    {
        spdlog::error("Failed to initialize OpenSSL.");
        return -1;
    }
    spdlog::info("OpenSSL initialized.");

    spdlog::info("Initializing the database...");
    if (!database.Initialize(SConfigHandler.GetString("database_info", "127.0.0.1;3306;gcemu;gcemu;gcemu"),
                             SConfigHandler.GetInt("database_connections", 1)))
    {
        spdlog::error("Failed to initialize the database.");
        return -1;
    }
    spdlog::info("Database initialized.");

    spdlog::info("Initializing TcpListener...");
    TcpListener<LoginSocket> listener("",
                                      SConfigHandler.GetInt("port", 9501),
                                      SConfigHandler.GetInt("network_threads", 1));
    spdlog::info("TcpListener initialized.");

    IsRunning = true;

    while (IsRunning);

    return 0;
}
