#pragma once

#include <iostream>
#include <asio.hpp>

namespace msg {
    // Define message types
    enum class MessageType {
        NetworkData,
        TaskResults,
        RegisterAgent,
    };

    // Message header structure
    struct MessageHeader {
        MessageType type;
        std::size_t size;
    };

    struct Request {
        MessageHeader header{};
        std::vector<uint8_t> body; 

        size_t size() const {
            return sizeof(MessageHeader) + body.size();
        } 
    };

    // Function to serialize the Request struct to binary data
    inline std::vector<char> serializeRequest(const Request& request) {
        std::vector<char> buffer(sizeof(MessageHeader));
        std::memcpy(buffer.data(), &request.header, sizeof(MessageHeader));

        // Append the binary body to the buffer
        buffer.insert(buffer.end(), request.body.begin(), request.body.end());

        return buffer;
    }

    // Function to send the binary data over the socket using Asio
    inline void sendRequest(asio::ip::tcp::socket& socket, const Request& request) {
        std::vector<char> buffer = serializeRequest(request);
        asio::write(socket, asio::buffer(buffer));
    }
}