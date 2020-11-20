#pragma once

#include <vulkan/vulkan.hpp>

namespace ktw {
	class Instance {
	public:
		Instance(std::vector<const char *>& extensionNames);
		vk::Instance& getInstance();

	private:
		vk::UniqueInstance instance;

		void setupDebugMessenger();
	};
}