#include "pch.hpp"
#include "Buffer.hpp"

namespace ktw {
	Buffer::Buffer(ktw::Device& device, uint32_t itemSize, uint32_t count, ktw::BufferUsage usage, void* data) : itemSize(itemSize), count(count) {
		auto bufferInfo = vk::BufferCreateInfo()
			.setSize(itemSize * count)
			.setUsage((vk::BufferUsageFlagBits) usage)
			.setSharingMode(vk::SharingMode::eExclusive);

		buffer = device.getDevice().createBufferUnique(bufferInfo);

		vk::MemoryRequirements memRequirements = device.getDevice().getBufferMemoryRequirements(*buffer);

		auto allocInfo = vk::MemoryAllocateInfo()
			.setAllocationSize(memRequirements.size)
			.setMemoryTypeIndex(findMemoryType(device, memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
		
		bufferMemory = device.getDevice().allocateMemoryUnique(allocInfo);

		device.getDevice().bindBufferMemory(*buffer, *bufferMemory, 0);

		void* bufferData = device.getDevice().mapMemory(*bufferMemory, 0, bufferInfo.size);
		memcpy(bufferData, data, (size_t) bufferInfo.size);
		device.getDevice().unmapMemory(*bufferMemory);
		LOG_TRACE("Buffer Created");
	}

	uint32_t Buffer::findMemoryType(ktw::Device& device, uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
		vk::PhysicalDeviceMemoryProperties memProperties = device.getPhysicalDevice().getMemoryProperties();
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