#pragma once

#include <string>

#include "RenderTarget.hpp"
#include "Shader.hpp"
#include "UniformBuffer.hpp"
#include "Context.hpp"

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

	struct VertexBufferBinding {
		uint32_t binding;
		uint32_t size;
		std::vector<ktw::AttributeDescription> attributeDescriptions;
	};

	enum ShaderStage {
		eVertex = vk::ShaderStageFlagBits::eVertex,
		eFragment = vk::ShaderStageFlagBits::eFragment
	};

	struct UniformDescriptor {
		uint32_t binding;
		ktw::ShaderStage stage;
		//ktw::UniformBuffer& buffer;
	};

	class GraphicsPipeline {
	public:
		GraphicsPipeline(ktw::Context& context, ktw::RenderTarget& renderTarget, const std::string& vertexShaderFile, const std::string& fragmentShaderFile, const std::vector<ktw::VertexBufferBinding>& vertexBufferBindings, const std::vector<ktw::UniformDescriptor>& uniformDescriptors);
		vk::Pipeline& getPipeline();

	private:
		vk::UniqueDescriptorSetLayout descriptorSetLayout;
		vk::UniquePipelineLayout pipelineLayout;
		vk::UniquePipeline pipeline;
		//std::vector<ktw::UniformBuffer*> uniformBuffers;
		//std::vector<vk::DescriptorSet> descriptorSets;
	};
}
