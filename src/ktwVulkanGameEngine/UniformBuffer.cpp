#include "pch.hpp"
#include "UniformBuffer.hpp"

namespace ktw {
	UniformBuffer::UniformBuffer(ktw::Device& device, ktw::IRenderTarget& renderTarget, uint32_t size) : renderTarget(renderTarget) {
		uniformBuffers.reserve(renderTarget.getFrameBuffers().size());
		for(size_t i = 0; i < renderTarget.getFrameBuffers().size(); i++) {
			uniformBuffers.emplace_back(device, size, 1, ktw::BufferUsage::eUniformBuffer, nullptr);
		}
	}

	void UniformBuffer::setData(void* data) {
		uniformBuffers[renderTarget.acquiredImageIndex()].setData(data);
	}
}