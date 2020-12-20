#pragma once

#include "FrameBuffer.hpp"
#include "GraphicsPipeline.hpp"
#include "Buffer.hpp"

namespace ktw {
	class CommandBuffer {
	public:
		CommandBuffer(ktw::FrameBuffer& framebuffer, vk::CommandBuffer commandBuffer);
		ktw::CommandBuffer& end();
		ktw::CommandBuffer& bindPipeline(ktw::GraphicsPipeline* pipeline);
		ktw::CommandBuffer& bindVertexBuffer(ktw::Buffer* buffer);
		ktw::CommandBuffer& bindIndexBuffer(ktw::Buffer* buffer);
		ktw::CommandBuffer& drawIndexed(uint32_t count);
		vk::CommandBuffer getHandle();

	private:
		vk::CommandBuffer commandBuffer;
	};
}