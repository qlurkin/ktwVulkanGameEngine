#pragma once

#define NOMINMAX

#include <spdlog/spdlog.h>

namespace ktw {
	class Log {
		public:
			inline static std::shared_ptr<spdlog::logger>& getLogger() {
				// static local variable are initialized the first time the
				// function is called
				static Log log;
				return log.logger;
			}

		private:
			std::shared_ptr<spdlog::logger> logger;

			Log();
	};
}

#define LOG_INFO(...)       ::ktw::Log::getLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)       ::ktw::Log::getLogger()->warn(__VA_ARGS__)
#define LOG_TRACE(...)      ::ktw::Log::getLogger()->trace(__VA_ARGS__)
#define LOG_ERROR(...)      ::ktw::Log::getLogger()->error(__VA_ARGS__)
#define LOG_FATAL(...)      ::ktw::Log::getLogger()->fatal(__VA_ARGS__)