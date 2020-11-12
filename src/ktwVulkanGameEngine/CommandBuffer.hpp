#pragma once

#include "GraphicsPipeline.hpp"
#include "Buffer.hpp"

namespace ktw {
	class CommandBuffer {
	public:
		CommandBuffer(ktw::Context& context, ktw::GraphicsPipeline& pipeline, ktw::Buffer& vertexBuffer);
		vk::CommandBuffer& getCommandBuffer(size_t i);
		
	private:
		ktw::Context& context;
		ktw::GraphicsPipeline& pipeline;
		ktw::Buffer& vertexBuffer;
		std::vector<vk::UniqueCommandBuffer> commandBuffers;
	};
}