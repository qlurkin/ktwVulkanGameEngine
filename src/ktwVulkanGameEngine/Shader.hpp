#pragma once

#include "Context.hpp"

namespace ktw {
	class Shader {
	public:
		Shader(ktw::Context& context, const std::string& filename);
		vk::UniqueShaderModule& getModule();

	private:
		vk::UniqueShaderModule module;

		static std::vector<char> readFile(const std::string& filename);
		void createShaderModule(ktw::Context& context, const std::vector<uint32_t>& code);
	};
}
