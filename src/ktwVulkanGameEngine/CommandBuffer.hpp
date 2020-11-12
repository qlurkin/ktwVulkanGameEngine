#pragma once

#include "GraphicsPipeline.hpp"

namespace ktw {
	class CommandBuffer {
	public:
		CommandBuffer(ktw::Context& context, ktw::GraphicsPipeline& pipeline);
		vk::CommandBuffer& getCommandBuffer(size_t i);
		
	private:
		ktw::Context& context;
		ktw::GraphicsPipeline& pipeline;
		std::vector<vk::UniqueCommandBuffer> commandBuffers;
	};
}