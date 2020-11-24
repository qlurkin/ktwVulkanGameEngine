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

	struct VertexBufferBinding {
		uint32_t binding;
		uint32_t size;
		std::vector<ktw::AttributeDescription> attributeDescriptions;
	};

	struct UniformDescriptor {
		uint32_t binding;
		uint32_t size;
		void* data;
	};

	class GraphicsPipeline {
	public:
		GraphicsPipeline(ktw::Device& device, ktw::IRenderTarget& renderTarget, const std::string& vertexShaderFile, const std::string& fragmentShaderFile, const std::vector<ktw::VertexBufferBinding>& vertexBufferBindings);
		vk::Pipeline& getPipeline();
		
		template<typename T>
		void setUniformBufferOject(uint32_t binding) {
			uniformBufferObject[typeid(T)] = {
				sizeof(T),
				binding,
				new T
			};
		}
		template<typename T>
		T* getUniformBufferOject() {
			return uniformBufferObject[typeid(T)].data;
		}

	private:
		vk::UniquePipelineLayout pipelineLayout;
		vk::UniquePipeline pipeline;
		std::unordered_map<std::type_index, UniformDescriptor> uniformBufferObject;
	};
}
