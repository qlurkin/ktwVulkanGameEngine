#pragma once

#include <string>

#include "Context.hpp"
#include "Shader.hpp"

namespace ktw {
	enum Format {
		eFloat4 = vk::Format::eR32G32B32A32Sfloat,
		eFloat3 = vk::Format::eR32G32B32Sfloat,
		eFloat2 = vk::Format::eR32G32Sfloat,
		eFloat = vk::Format::eR32Sfloat,
		eInt = vk::Format::eR32Sint,
		eUInt = vk::Format::eR32Uint
	};

	struct AttributeDescription {
		uint32_t location;
		ktw::Format format;
		uint32_t offset;
	};

	class GraphicsPipeline {
	public:
		GraphicsPipeline(ktw::Context& context, const std::string& vertexShader, const std::string& fragmentShader, uint32_t vertexSize, std::vector<ktw::AttributeDescription> attributeDescriptions);

		vk::VertexInputBindingDescription getBindingDescription(uint32_t size);
		std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions(std::vector<ktw::AttributeDescription> attributeDescriptions);
		vk::Pipeline& getPipeline();

	private:
		ktw::Context& context;
		ktw::Shader vertexShader;
		ktw::Shader fragmentShader;
		vk::UniquePipelineLayout pipelineLayout;
		vk::UniquePipeline pipeline;
	};
}
