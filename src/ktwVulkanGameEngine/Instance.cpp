#include "pch.hpp"
#include "Instance.hpp"

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
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

namespace ktw {
	Instance::Instance(std::vector<const char *>& extensionNames) {
		if (enableValidationLayers) {
			extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

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

		if (enableValidationLayers) {
			createInfo
				.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()))
				.setPpEnabledLayerNames(validationLayers.data());
		} else {
			createInfo
				.setEnabledLayerCount(0);
		}

		instance = vk::createInstanceUnique(createInfo);

		if (enableValidationLayers) {
			setupDebugMessenger();
		}

		LOG_TRACE("Instance Created");
	}

	vk::Instance& Instance::getInstance() {
		return *instance;
	}

	void Instance::setupDebugMessenger() {
		if (!enableValidationLayers) return;
		vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT()
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

		auto dldi = vk::DispatchLoaderDynamic(*(instance), vkGetInstanceProcAddr);

		messenger = instance->createDebugUtilsMessengerEXTUnique(createInfo, nullptr, dldi);
		LOG_INFO("Debug Messenger Setted Up");
	}
}