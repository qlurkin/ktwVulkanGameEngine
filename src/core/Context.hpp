#pragma once

#include <vulkan/vulkan.hpp>

#include <vector>
#include <string>

namespace ktw {
	class Context {
	public:
		Context(std::vector<const char *> extensionNames) {
			vk::ApplicationInfo appInfo = vk::ApplicationInfo()
				.setPApplicationName("Hello Triangle")
				.setApplicationVersion(VK_MAKE_VERSION(1,0,0))
				.setPEngineName("No Engine")
				.setEngineVersion(VK_MAKE_VERSION(1,0,0))
				.setApiVersion(VK_MAKE_VERSION(1,2,154));

			vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo()
				.setPApplicationInfo(&appInfo)
				.setPpEnabledExtensionNames(extensionNames.data())
				.setEnabledExtensionCount(extensionNames.size())
				.setEnabledLayerCount(0);

			if(vk::createInstance(&createInfo, nullptr, &instance) != vk::Result::eSuccess) {
				throw std::runtime_error("failed to create instance!");
			}
		}

		~Context() {
			instance.destroy();
		}

	private:
		vk::Instance instance;
	};
} // namespace ktw