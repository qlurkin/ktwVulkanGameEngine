#pragma once

#include <string>
#include <memory>

#include "Context.hpp"
#include "Shader.hpp"

namespace ktw {
	class GraphicsPipeline {
	public:
		GraphicsPipeline(ktw::Context& context, const std::string& vertexShader, const std::string& fragmentShader) : context(context), vertexShader(context, vertexShader), fragmentShader(context, fragmentShader) {}

	private:
		ktw::Context& context;
		ktw::Shader vertexShader;
		ktw::Shader fragmentShader;
	};
}
