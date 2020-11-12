#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"
#include "CommandBuffer.hpp"

namespace ktw {

	class Renderer {
	public:
		Renderer(ktw::Context& context);

		ktw::GraphicsPipeline* createGraphicsPipeline(std::string vertexShader, std::string fragmentShader);
		ktw::CommandBuffer* createCommandBuffer(ktw::GraphicsPipeline* pipeline);
		void post(ktw::CommandBuffer* commandBuffer);

	private:
		ktw::Context& context;
	};
}