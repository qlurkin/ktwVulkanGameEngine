#pragma once

#include "Device.hpp"

namespace ktw {
	class Shader {
	public:
		Shader(ktw::Device& device, const std::string& filename);
		vk::UniqueShaderModule& getModule();

	private:
		vk::UniqueShaderModule module;

		static std::vector<char> readFile(const std::string& filename);
		void createShaderModule(ktw::Device& device, const std::vector<uint32_t>& code);
	};
}
