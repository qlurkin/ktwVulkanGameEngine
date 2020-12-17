#include "pch.hpp"
#include "Buffer.hpp"

namespace ktw {
	Buffer::Buffer(ktw::Context& context, uint32_t itemSize, uint32_t count, ktw::BufferUsage usage, void* data) : itemSize(itemSize), count(count), context(context) {
		auto bufferInfo = vk::BufferCreateInfo()
			.setSize(itemSize * count)
			.setUsage((vk::BufferUsageFlagBits) usage)
			.setSharingMode(vk::SharingMode::eExclusive);

		buffer = context.getDevice().createBufferUnique(bufferInfo);

		vk::MemoryRequirements memRequirements = context.getDevice().getBufferMemoryRequirements(*buffer);

		auto allocInfo = vk::MemoryAllocateInfo()
			.setAllocationSize(memRequirements.size)
			.setMemoryTypeIndex(findMemoryType(context, memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
		
		bufferMemory = context.getDevice().allocateMemoryUnique(allocInfo);

		context.getDevice().bindBufferMemory(*buffer, *bufferMemory, 0);

		if(data) {
			setData(data);
		}
		
		LOG_TRACE("Buffer Created");
	}

	uint32_t Buffer::findMemoryType(ktw::Context& context, uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
		vk::PhysicalDeviceMemoryProperties memProperties = context.getPhysicalDevice().getMemoryProperties();
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

	void Buffer::setData(void* data) {
		void* bufferData = context.getDevice().mapMemory(*bufferMemory, 0, itemSize*count);
		memcpy(bufferData, data, (size_t) itemSize*count);
		context.getDevice().unmapMemory(*bufferMemory);
	}
}