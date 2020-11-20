#pragma once

#include <string>

#include "IRenderTarget.hpp"
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
		GraphicsPipeline(ktw::Device& device, ktw::IRenderTarget& renderTarget, const std::string& vertexShaderFile, const std::string& fragmentShaderFile, uint32_t vertexSize, std::vector<ktw::AttributeDescription>& attributeDescriptions);

		vk::Pipeline& getPipeline();

	private:
		vk::UniquePipelineLayout pipelineLayout;
		vk::UniquePipeline pipeline;
	};
}
