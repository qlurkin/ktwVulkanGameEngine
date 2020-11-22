#include "pch.hpp"
#include "Shader.hpp"

namespace ktw {
	Shader::Shader(ktw::Device& device, const std::string& filename) {
		createShaderModule(device, readFile(filename));
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

	void Shader::createShaderModule(ktw::Device& device, const std::vector<char>& code) {
		auto createInfo = vk::ShaderModuleCreateInfo()
			.setCodeSize(code.size())
			.setPCode(reinterpret_cast<const uint32_t*>(code.data()));

		module = device.getDevice().createShaderModuleUnique(createInfo);
	}
}