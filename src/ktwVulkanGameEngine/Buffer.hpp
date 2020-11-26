#pragma once

#include "Device.hpp"

namespace ktw {
	enum BufferUsage {
		eVertexBuffer = vk::BufferUsageFlagBits::eVertexBuffer,
		eIndexBuffer = vk::BufferUsageFlagBits::eIndexBuffer,
		eUniformBuffer = vk::BufferUsageFlagBits::eUniformBuffer
	};
	
	class Buffer {
	public:
		Buffer(ktw::Device& device, uint32_t itemSize, uint32_t count, ktw::BufferUsage usage, void* data);

		vk::Buffer& getBuffer();
		uint32_t getItemSize();
		uint32_t getCount();
		void setData(void* data);
	private:
		ktw::Device& device;
		vk::UniqueBuffer buffer;
		vk::UniqueDeviceMemory bufferMemory;
		uint32_t count;
		uint32_t itemSize;

		uint32_t findMemoryType(ktw::Device& device, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
	};
}