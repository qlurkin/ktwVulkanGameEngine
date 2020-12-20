#pragma once

#include "FrameBuffer.hpp"
#include "GraphicsPipeline.hpp"
#include "Buffer.hpp"

namespace ktw {
	class CommandBuffer {
	public:
		CommandBuffer(ktw::FrameBuffer& framebuffer, vk::CommandBuffer commandBuffer);
		void end();
		void bindPipeline(ktw::GraphicsPipeline* pipeline);
		void bindVertexBuffer(ktw::Buffer* buffer);
		void bindIndexBuffer(ktw::Buffer* buffer);
		void drawIndexed(uint32_t count);
		vk::CommandBuffer getHandle();

	private:
		vk::CommandBuffer commandBuffer;
	};
}