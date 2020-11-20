#pragma once

#include "Device.hpp"

namespace ktw {
	class Buffer {
	public:
		Buffer(ktw::Device& device, uint32_t itemSize, uint32_t count, void* data);

		vk::Buffer& getBuffer();
		uint32_t getItemSize();
		uint32_t getCount();
	private:
		vk::UniqueBuffer buffer;
		vk::UniqueDeviceMemory bufferMemory;
		uint32_t count;
		uint32_t itemSize;

		uint32_t findMemoryType(ktw::Device& device, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
	};
}