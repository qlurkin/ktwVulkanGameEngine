#pragma once

#include <string>

#include "Context.hpp"
#include "Shader.hpp"

namespace ktw {
	class GraphicsPipeline {
	public:
		GraphicsPipeline(ktw::Context& context, const std::string& vertexShader, const std::string& fragmentShader);

		vk::Pipeline& getPipeline();

	private:
		ktw::Context& context;
		ktw::Shader vertexShader;
		ktw::Shader fragmentShader;
		vk::UniquePipelineLayout pipelineLayout;
		vk::UniquePipeline pipeline;
	};
}
