#include "Renderer.hpp"

#include <vector>

namespace ktw {
	Renderer::Renderer(ktw::Context& context) : context(context) {}
	
	ktw::GraphicsPipeline* Renderer::createGraphicsPipeline(std::string vertexShader, std::string fragmentShader, uint32_t vertexSize, std::vector<ktw::AttributeDescription> attributeDescriptions) {
		return new ktw::GraphicsPipeline(context, vertexShader, fragmentShader, vertexSize, attributeDescriptions);
	}

	ktw::CommandBuffer* Renderer::createCommandBuffer(ktw::GraphicsPipeline* pipeline, ktw::Buffer* vertexBuffer) {
		return new ktw::CommandBuffer(context, *pipeline, *vertexBuffer);
	}

	ktw::Buffer* Renderer::createBuffer(uint32_t itemSize, size_t count, void* data) {
		return new ktw::Buffer(context, itemSize, static_cast<uint32_t>(count), data);
	}

	void Renderer::post(ktw::CommandBuffer* commandBuffer) {
		context.postedCommandBuffers.push_back(commandBuffer->getCommandBuffer(context.imageIndex));
	}
}