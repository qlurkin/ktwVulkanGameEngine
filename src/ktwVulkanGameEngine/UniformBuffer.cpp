#include "pch.hpp"
#include "UniformBuffer.hpp"

namespace ktw {
	UniformBuffer::UniformBuffer(ktw::Context& context, uint32_t size) : size(size), uniformBuffer(context, size, 1, ktw::BufferUsage::eUniformBuffer, nullptr) {
	}

	void UniformBuffer::setData(void* data) {
		uniformBuffer.setData(data);
	}

	uint32_t UniformBuffer::getSize() {
		return size;
	}

	vk::Buffer UniformBuffer::getBuffer() {
		return uniformBuffer.getBuffer();
	}
}