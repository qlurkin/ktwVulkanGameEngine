#pragma once

#include "Device.hpp"
#include "IRenderTarget.hpp"
#include "Buffer.hpp"

namespace ktw {
	class UniformBuffer {
	public:
		UniformBuffer(ktw::Device& device, uint32_t size);
		void setData(void* data);
		uint32_t getSize();
		vk::Buffer getBuffer();
	private:
		uint32_t size;
		ktw::Buffer uniformBuffer;
	};
}