#include <iostream>
#include <string_view>

#include "tasks.h"

using json = nlohmann::json;

PingTask::PingTask(std::string_view agent_id) : agent_id{agent_id} {}

msg::Request PingTask::run() const {
    std::string message = "pong";

    // build request obj
    msg::MessageHeader header;
    header.agent_id = agent_id;
    header.size = message.size();
    header.type = msg::MessageType::Heartbeat;

    msg::Request request;
    request.header = header;

    json body;
    body["message"] = message;
    request.body = body;

    return request;
}

RegisterAgent::RegisterAgent(std::string_view agent_id) : agent_id{agent_id} {}

msg::Request RegisterAgent::run() const {
    // check if registered, if not register
    std::string message = "Agent registered";

    // build request obj
    msg::MessageHeader header;
    header.agent_id = agent_id;
    header.size = message.size();
    header.type = msg::MessageType::RegisterAgent;

    msg::Request request;
    request.header = header;
    json body;
    body["message"] = message;

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        std::cerr << "Error getting the hostname" << std::endl;
    }

    body["hostname"] = hostname;
    request.body = body;

    return request;
}