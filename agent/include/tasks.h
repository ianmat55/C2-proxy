#pragma once

#include <string_view>
#include <queue>

#include "net_message.h"

struct Configuration {
    Configuration(double meanDwell, bool isRunning);
    const double meanDwell;
    const bool isRunning;
};

// Tasks
// ===========================================================================================

// PingTask
// -------------------------------------------------------------------------------------------
struct PingTask {
	PingTask(std::string_view agent_id);
	const std::string_view agent_id;
	constexpr static std::string_view key{ "ping" };
	[[nodiscard]] msg::Request run() const;
};

struct RegisterAgent {
	RegisterAgent(std::string_view agent_id);
	const std::string_view agent_id;
	constexpr static std::string_view key{ "register" };
	[[nodiscard]] msg::Request run() const;
};

using Task = std::variant<PingTask, RegisterAgent>;

[[nodiscard]] Task parseTaskFrom(const std::queue<Task>& taskQueue,
	std::function<void(const Configuration&)> setter);