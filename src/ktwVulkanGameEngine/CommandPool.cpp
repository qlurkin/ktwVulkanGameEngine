#include "pch.hpp"
#include "CommandPool.hpp"

namespace ktw
{
	CommandPool::CommandPool(ktw::Context& context) : context(context) {
		auto poolInfo = vk::CommandPoolCreateInfo()
			.setQueueFamilyIndex(context.getGraphicsQueueIndex())
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

		commandPool = context.getDevice().createCommandPoolUnique(poolInfo);
		LOG_TRACE("Command Pool Created");
	}

	vk::CommandBuffer CommandPool::getCommandBuffer() {
		if(!allocatedCommandBuffer.empty()) {
			vk::CommandBuffer buffer = *allocatedCommandBuffer.begin();
			allocatedCommandBuffer.erase(allocatedCommandBuffer.begin());
			lockedCommandBuffer.insert(buffer);
			return buffer;
		}

		return createCommandBuffer();
	}

	void CommandPool::freeCommandBuffer(vk::CommandBuffer buffer) {
		lockedCommandBuffer.erase(buffer);
		allocatedCommandBuffer.insert(buffer);
	}

	vk::CommandBuffer CommandPool::createCommandBuffer() {
		auto allocInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool(*commandPool)
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount(1);
		
		vk::CommandBuffer buffer = context.getDevice().allocateCommandBuffers(allocInfo)[0];
		lockedCommandBuffer.insert(buffer);

		LOG_TRACE("Total Command Buffer Allocated: {}", lockedCommandBuffer.size()+allocatedCommandBuffer.size());

		return buffer;
	}
}
