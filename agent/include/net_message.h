#pragma once

#include <iostream>
#include <asio.hpp>
#include <mutex>
#include <deque>
#include <nlohmann/json.hpp>

#include "tasks.h"

namespace msg {

    inline std::string delim = "\r";

    // Define message types
    enum class MessageType {
        NetworkData,
        TaskResults,
        RegisterAgent,
        Heartbeat,
    };

    // Message header structure
    struct MessageHeader {
        std::string agent_id;
        MessageType type;
        std::uint32_t size;
    };

    struct Request {
        MessageHeader header{};
        nlohmann::json body;

        size_t size() const {
            return sizeof(MessageHeader) + body.dump().size(); // Use dump() to get the JSON string size
        }
    };

    struct Response {
        MessageHeader header{};
        nlohmann::json body;
    };

    // Function to serialize the Request struct to a JSON string
    inline std::string serializeRequest(const Request& request) {
        nlohmann::json jsonMessage;
        jsonMessage["header"]["id"] = request.header.agent_id;
        jsonMessage["header"]["type"] = request.header.type;
        jsonMessage["header"]["size"] = request.size();

        // Convert the binary body to a base64-encoded string to represent binary data in JSON
        jsonMessage["body"] = request.body;

        return jsonMessage.dump();
    }

    // Function to send the binary data over the socket using Asio
    inline void sendRequest(asio::ip::tcp::socket& socket, const Request& request) {
        std::string buffer = serializeRequest(request) + delim;
        asio::async_write(socket, asio::buffer(buffer), [&](const asio::error_code& error, std::size_t bytes_transferred) {
            if (error) {
                std::cout << "Write operation failed with error: " << error.message() << std::endl;
                std::exit(1);
            }
        });
    }

    inline Response deserializeResponse(const std::string& jsonMessage) {
        Response response;
        auto json = nlohmann::json::parse(jsonMessage);

        response.header.type = json["type"];
        response.header.size = json["size"];
        response.body = json["body"];

        return response;
    }

    inline void receiveResponse(asio::ip::tcp::socket& socket, asio::streambuf& buf, std::queue<std::string>& taskQueue) {
        asio::async_read(socket, buf, asio::transfer_at_least(1), [&](const asio::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                // Get the data from the streambuf as a sequence of buffers
                const auto buffers = buf.data();

                std::string data(asio::buffers_begin(buffers), asio::buffers_end(buffers));
                buf.consume(bytes_transferred);

                taskQueue.emplace(data);

                receiveResponse(socket, buf, taskQueue);
            } else {
                std::cout << "Error in handle_read: " << error.message() << std::endl;
            }
        });

        while (!taskQueue.empty()) {
            std::cout << taskQueue.front() << std::endl;
            taskQueue.pop();
        }
    }
}