#pragma once

#include <vulkan/vulkan.hpp>

#include <vector>
#include <string>

namespace ktw {
	#ifdef NDEBUG
		const bool enableValidationLayers = false;
	#else
		const bool enableValidationLayers = true;
	#endif

	class Context {
	public:
		Context(std::vector<const char *> extensionNames) {
			createInstance(extensionNames);
			setupDebugMessenger();
		}

		~Context() {
			if (enableValidationLayers) {
				auto dldi = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
				instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
			}

			instance.destroy();
		}

		void createInstance(std::vector<const char *> extensionNames) {
			if (enableValidationLayers) {
				extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			}

			const std::vector<const char*> validationLayers = {
				"VK_LAYER_KHRONOS_validation"
			};

			vk::ApplicationInfo appInfo = vk::ApplicationInfo()
				.setPApplicationName("Hello Triangle")
				.setApplicationVersion(VK_MAKE_VERSION(1,0,0))
				.setPEngineName("No Engine")
				.setEngineVersion(VK_MAKE_VERSION(1,0,0))
				.setApiVersion(VK_MAKE_VERSION(1,2,154));

			vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo()
				.setPApplicationInfo(&appInfo)
				.setPpEnabledExtensionNames(extensionNames.data())
				.setEnabledExtensionCount(static_cast<uint32_t>(extensionNames.size()));

			vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
			if (enableValidationLayers) {
				createInfo
					.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()))
					.setPpEnabledLayerNames(validationLayers.data());

				populateDebugMessengerCreateInfo(debugCreateInfo);
				createInfo.setPNext(&debugCreateInfo);
			} else {
				createInfo
					.setEnabledLayerCount(0);
			}

			if(vk::createInstance(&createInfo, nullptr, &instance) != vk::Result::eSuccess) {
				throw std::runtime_error("failed to create instance!");
			}
		}

		void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo) {
			createInfo = vk::DebugUtilsMessengerCreateInfoEXT()
				.setMessageSeverity(
					vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
					vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
					vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
				)
				.setMessageType(
					vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
					vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
					vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
				)
				.setPfnUserCallback(debugCallback);
		}

		void setupDebugMessenger() {
			if (!enableValidationLayers) return;
			vk::DebugUtilsMessengerCreateInfoEXT createInfo;
			populateDebugMessengerCreateInfo(createInfo);

			auto dldi = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);

			debugMessenger = instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dldi);
		}

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData) {

			if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
				LOG_WARN("validation layer: WARNING: {}", pCallbackData->pMessage);
			}

			if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
				LOG_ERROR("validation layer: ERROR: {}", pCallbackData->pMessage);
			}

			if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
				LOG_INFO("validation layer: VERBOSE: {}", pCallbackData->pMessage);
			}

			return VK_FALSE;
		}

	private:
		vk::Instance instance;
		vk::DebugUtilsMessengerEXT debugMessenger;
	};
} // namespace ktw