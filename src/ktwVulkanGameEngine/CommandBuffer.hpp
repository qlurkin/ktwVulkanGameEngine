#pragma once

#include "GraphicsPipeline.hpp"
#include "Buffer.hpp"
#include "IRenderTarget.hpp"
#include "Device.hpp"

namespace ktw {
	class CommandBuffer {
	public:
		CommandBuffer(ktw::Device& device, ktw::IRenderTarget& renderTarget, ktw::GraphicsPipeline& pipeline, ktw::Buffer& vertexBuffer);
		vk::CommandBuffer& getCommandBuffer(size_t i);
		
	private:
		//ktw::GraphicsPipeline& pipeline;
		//ktw::Buffer& vertexBuffer;
		std::vector<vk::UniqueCommandBuffer> commandBuffers;
	};
}