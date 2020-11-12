#pragma once

#include "Context.hpp"

namespace ktw {
	class Buffer {
	public:
		Buffer(ktw::Context& context, uint32_t itemSize, uint32_t count, void* data);
		uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

		vk::Buffer& getBuffer();
		uint32_t getItemSize();
		uint32_t getCount();
	private:
		ktw::Context& context;
		vk::UniqueBuffer buffer;
		vk::UniqueDeviceMemory bufferMemory;
		uint32_t count;
		uint32_t itemSize;
	};
}