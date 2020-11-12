#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"
#include "CommandBuffer.hpp"

namespace ktw {

	class Renderer {
	public:
		Renderer(ktw::Context& context);

		ktw::GraphicsPipeline* createGraphicsPipeline(std::string vertexShader, std::string fragmentShader, uint32_t vertexSize, std::vector<ktw::AttributeDescription> attributeDescriptions);
		ktw::CommandBuffer* createCommandBuffer(ktw::GraphicsPipeline* pipeline, ktw::Buffer* vertexBuffer);
		ktw::Buffer* createBuffer(uint32_t itemSize, size_t count, void* data);
		void post(ktw::CommandBuffer* commandBuffer);

	private:
		ktw::Context& context;
	};
}