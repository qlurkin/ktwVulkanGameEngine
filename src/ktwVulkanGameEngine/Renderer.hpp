#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

#include "Context.hpp"
#include "CommandBuffer.hpp"

namespace ktw {

	class Renderer {
	public:
		Renderer(ktw::Context& context) : context(context) {}
		ktw::GraphicsPipeline* createGraphicsPipeline(std::string vertexShader, std::string fragmentShader) {
			return new ktw::GraphicsPipeline(context, vertexShader, fragmentShader);
		}

		ktw::CommandBuffer* createCommandBuffer(ktw::GraphicsPipeline* pipeline) {
			return new ktw::CommandBuffer(context, *pipeline);
		}

		void post(ktw::CommandBuffer* commandBuffer) {
			context.postedCommandBuffers.push_back(commandBuffer->getCommandBuffer(context.imageIndex));
		}

	private:
		ktw::Context& context;
	};
}