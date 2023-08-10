#pragma once

#include <iostream>
#include <asio.hpp>
#include <nlohmann/json.hpp>

namespace msg {
    // Define message types
    enum class MessageType {
        NetworkData,
        TaskResults,
        RegisterAgent,
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

    struct ResponseHeader {
        MessageType type;
        std::uint32_t size;
    };

    struct Response {
        ResponseHeader header{};
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

        std::string buffer = serializeRequest(request);
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

    // TODO: Add lambda func param to decouple message handling and lib, just import here for now
    inline void receiveResponse(asio::ip::tcp::socket& socket, asio::streambuf& buf) {
        asio::async_read(socket, buf, asio::transfer_at_least(1), [&](const asio::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                // Get the data from the streambuf as a sequence of buffers
                const auto buffers = buf.data();

                // Convert the sequence of buffers to a string
                std::string data(asio::buffers_begin(buffers), asio::buffers_end(buffers));
                std::cout << "DATA: " << data << std::endl;

                // Clear the buffer to prepare for the next read
                buf.consume(bytes_transferred);

                // Start a new read operation to receive the next message
                receiveResponse(socket, buf);
            } else {
                std::cout << "Error in handle_read: " << error.message() << std::endl;
            }
        });
    }
}