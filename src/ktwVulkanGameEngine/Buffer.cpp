#include "Buffer.hpp"

namespace ktw {
	Buffer::Buffer(ktw::Context& context, uint32_t itemSize, uint32_t count, void* data) : context(context), itemSize(itemSize), count(count) {
		auto bufferInfo = vk::BufferCreateInfo()
			.setSize(itemSize * count)
			.setUsage(vk::BufferUsageFlagBits::eVertexBuffer)
			.setSharingMode(vk::SharingMode::eExclusive);

		buffer = context.device->createBufferUnique(bufferInfo);

		vk::MemoryRequirements memRequirements = context.device->getBufferMemoryRequirements(*buffer);

		auto allocInfo = vk::MemoryAllocateInfo()
			.setAllocationSize(memRequirements.size)
			.setMemoryTypeIndex(findMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
		
		bufferMemory = context.device->allocateMemoryUnique(allocInfo);

		context.device->bindBufferMemory(*buffer, *bufferMemory, 0);

		void* bufferData = context.device->mapMemory(*bufferMemory, 0, bufferInfo.size);
		memcpy(bufferData, data, (size_t) bufferInfo.size);
		context.device->unmapMemory(*bufferMemory);
	}

	uint32_t Buffer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
		vk::PhysicalDeviceMemoryProperties memProperties = context.physicalDevice.getMemoryProperties();
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	vk::Buffer& Buffer::getBuffer() {
		return *buffer;
	}

	uint32_t Buffer::getItemSize() {
		return itemSize;
	}

	uint32_t Buffer::getCount() {
		return count;
	}
}