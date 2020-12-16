#pragma once

#include "Device.hpp"

namespace ktw
{
	class CommandPool {
	public:
		CommandPool(ktw::Device& device);
		vk::CommandBuffer getCommandBuffer();
		void freeCommandBuffer(vk::CommandBuffer& buffer);

	private:
		ktw::Device& device;
		vk::UniqueCommandPool commandPool;
		std::set<vk::CommandBuffer> allocatedCommandBuffer;
		std::set<vk::CommandBuffer> lockedCommandBuffer;

		vk::CommandBuffer createCommandBuffer();
	};
} // namespace ktw
