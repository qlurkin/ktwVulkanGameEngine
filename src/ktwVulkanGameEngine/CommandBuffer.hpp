#pragma once

#include "GraphicsPipeline.hpp"
#include "Buffer.hpp"
#include "IRenderTarget.hpp"
#include "Device.hpp"

namespace ktw {
	class CommandBuffer {
	public:
		CommandBuffer(ktw::Device& device, ktw::IRenderTarget& renderTarget, ktw::GraphicsPipeline& pipeline, ktw::Buffer& vertexBuffer, ktw::Buffer& indexBuffer);
		vk::CommandBuffer& getCommandBuffer(size_t i);
		
	private:
		std::vector<vk::UniqueCommandBuffer> commandBuffers;
	};
}