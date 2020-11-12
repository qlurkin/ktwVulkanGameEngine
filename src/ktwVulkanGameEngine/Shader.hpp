#pragma once

#include <vulkan/vulkan.hpp>

#include "Context.hpp"

namespace ktw {
	class Shader {
	public:
		Shader(ktw::Context& context, const std::string& filename);
		vk::UniqueShaderModule& getModule();

	private:
		ktw::Context& context;
		vk::UniqueShaderModule module;

		static std::vector<char> readFile(const std::string& filename);
		void createShaderModule(const std::vector<char>& code);
	};
}
