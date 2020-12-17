#include "pch.hpp"
#include "Shader.hpp"
#include "ShaderCompiler.hpp"

namespace ktw {
	Shader::Shader(ktw::Context& context, const std::string& filename) {
		createShaderModule(context, CompileGLSL(filename));
		LOG_TRACE("Shader {} created", filename);
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

	void Shader::createShaderModule(ktw::Context& context, const std::vector<uint32_t>& code) {
		auto createInfo = vk::ShaderModuleCreateInfo()
			.setCodeSize(code.size() * sizeof(uint32_t))
			.setPCode(code.data());

		module = context.getDevice().createShaderModuleUnique(createInfo);
	}
}