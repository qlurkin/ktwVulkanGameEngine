#pragma once

#include "Context.hpp"

namespace ktw
{
	class CommandPool {
	public:
		CommandPool(ktw::Context& context);
		~CommandPool();
		vk::CommandBuffer getCommandBuffer();
		void freeCommandBuffer(vk::CommandBuffer buffer);

	private:
		ktw::Context& context;
		vk::UniqueCommandPool commandPool;
		std::set<vk::CommandBuffer> allocatedCommandBuffer;
		std::set<vk::CommandBuffer> lockedCommandBuffer;

		vk::CommandBuffer createCommandBuffer();
	};
} // namespace ktw
