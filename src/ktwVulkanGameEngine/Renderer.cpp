#include "Renderer.hpp"

#include <vector>

namespace ktw {
	Renderer::Renderer(ktw::Context& context) : context(context) {}
	
	ktw::GraphicsPipeline* Renderer::createGraphicsPipeline(std::string vertexShader, std::string fragmentShader) {
		return new ktw::GraphicsPipeline(context, vertexShader, fragmentShader);
	}

	ktw::CommandBuffer* Renderer::createCommandBuffer(ktw::GraphicsPipeline* pipeline) {
		return new ktw::CommandBuffer(context, *pipeline);
	}

	void Renderer::post(ktw::CommandBuffer* commandBuffer) {
		context.postedCommandBuffers.push_back(commandBuffer->getCommandBuffer(context.imageIndex));
	}
}