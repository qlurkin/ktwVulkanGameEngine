#include "Shader.hpp"

#include <string>
#include <fstream>
#include <streambuf>

namespace ktw {
	Shader::Shader(ktw::Context& context, const std::string& filename) : context(context) {
		createShaderModule(readFile(filename));
	}

	vk::UniqueShaderModule& Shader::getModule() {
		return module;
	}

	std::vector<char> Shader::readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t) file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	void Shader::createShaderModule(const std::vector<char>& code) {
		auto createInfo = vk::ShaderModuleCreateInfo()
			.setCodeSize(code.size())
			.setPCode(reinterpret_cast<const uint32_t*>(code.data()));

		module = context.device->createShaderModuleUnique(createInfo);
	}
}