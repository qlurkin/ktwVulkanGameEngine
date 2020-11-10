#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

#include "Context.hpp"
#include "GraphicsPipeline.hpp"

namespace ktw {

	class Renderer {
	public:
		Renderer(ktw::Context& context) : context(context) {}
		ktw::GraphicsPipeline* createGraphicsPipeline(std::string vertexShader, std::string fragmentShader) {
			return new ktw::GraphicsPipeline(context, vertexShader, fragmentShader);
		}

	private:
		ktw::Context& context;
	};
}