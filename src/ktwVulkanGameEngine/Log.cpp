#include "Log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace ktw {
	Log::Log() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
		logger = spdlog::stdout_color_mt("ktwVulkanGameEngine");
		logger->set_level(spdlog::level::trace);
	}
}