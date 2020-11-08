#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <set>
#include <memory>

namespace ktw {
	#ifdef NDEBUG
		const bool enableValidationLayers = false;
	#else
		const bool enableValidationLayers = true;
	#endif

	class Application {
	public:
		Application(uint32_t width, uint32_t height) {
			this->width = width;
			this->height = height;
		}

		~Application() {}

		void run() {
			initWindow();
			initVulkan();
			userSetup();
			mainLoop();
			cleanup();
		}

	private:
		virtual void userSetup() = 0;
		virtual void userUpdate() = 0;

		void initWindow() {
			glfwInit();

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
			
			window = glfwCreateWindow(width, height, "ktwVulkanGameEngine", nullptr, nullptr);
		}

		void initVulkan() {
			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions;
			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
			std::vector<const char*> extensionNames(glfwExtensions, glfwExtensions + glfwExtensionCount);

			createInstance(extensionNames);
			setupDebugMessenger();
			createSurface();
			pickPhysicalDevice();
			createLogicalDevice();
		}

		void mainLoop() {
			while (!glfwWindowShouldClose(window)) {
				glfwPollEvents();
				userUpdate();
			}
		}

		void cleanup() {
			glfwDestroyWindow(window);
			glfwTerminate();
		}

		void createSurface() {
			VkSurfaceKHR surface;
			if(glfwCreateWindowSurface(*instance, window, nullptr, &surface) != VK_SUCCESS) {
				throw std::runtime_error("failed to create window surface!");
			}

			this->surface = vk::UniqueSurfaceKHR(surface, *instance);
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

			if (enableValidationLayers) {
				createInfo
					.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()))
					.setPpEnabledLayerNames(validationLayers.data());
			} else {
				createInfo
					.setEnabledLayerCount(0);
			}

			instance = vk::createInstanceUnique(createInfo);
		}

		void setupDebugMessenger() {
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

			auto dldi = vk::DispatchLoaderDynamic(*instance, vkGetInstanceProcAddr);

			instance->createDebugUtilsMessengerEXTUnique(createInfo, nullptr, dldi);
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

		// 0 means not suitable
		int rateDeviceSuitability(vk::PhysicalDevice device) {
			// Must support GraphicsQueue
			if(findGraphicsQueueIndexes(device).size() == 0) {
				return 0;
			}

			// Must support PresentQueue
			if(findPresentQueueIndexes(device).size() == 0) {
				return 0;
			}

			vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
			vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

			int score = 0;

			// Discrete GPUs have a significant performance advantage
			if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
				score += 1000;
			}

			// Maximum possible size of textures affects graphics quality
			score += deviceProperties.limits.maxImageDimension2D;

			return score;
		}

		void pickPhysicalDevice() {
			auto physicalDevices = instance->enumeratePhysicalDevices();

			int scoreMax = 0;

			for (const auto& device : physicalDevices) {
				int score = rateDeviceSuitability(device);
				if (score > scoreMax) {
					physicalDevice = device;
					scoreMax = score;
					break;
				}
			}

			if(scoreMax == 0) {
				throw std::runtime_error("failed to find a suitable GPU!");
			}

			LOG_INFO("Selected GPU: {}", physicalDevice.getProperties().deviceName);
		}

		std::vector<uint32_t> findGraphicsQueueIndexes(vk::PhysicalDevice device) {
			auto queueFamilyProperties = device.getQueueFamilyProperties();
			std::vector<uint32_t> res;
			for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
				if(queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
					res.push_back(i);
				}
			}
			return res;
		}

		std::vector<uint32_t> findPresentQueueIndexes(vk::PhysicalDevice device) {
			auto queueFamilyProperties = device.getQueueFamilyProperties();
			std::vector<uint32_t> res;
			for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
				if (device.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface.get())) {
					res.push_back(i);
				}
			}
			return res;
		}

		void createLogicalDevice() {
			std::vector<uint32_t> graphicsQueueIndices = findGraphicsQueueIndexes(physicalDevice);
			std::vector<uint32_t> presentQueueIndices = findPresentQueueIndexes(physicalDevice);

			// We can be smarter here ?
			std::set<uint32_t> uniqueQueueFamilyIndices = { graphicsQueueIndices[0], presentQueueIndices[0] };

			std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

			float queuePriority = 1.0f;
			for (uint32_t queueFamilyIndex : uniqueQueueFamilyIndices) {
				auto queueCreateInfo = vk::DeviceQueueCreateInfo()
					.setQueueFamilyIndex(queueFamilyIndex)
					.setQueueCount(1)
					.setPQueuePriorities(&queuePriority);
				queueCreateInfos.push_back(queueCreateInfo);
			}

			vk::PhysicalDeviceFeatures deviceFeatures{};

			auto createInfo = vk::DeviceCreateInfo()
				.setPQueueCreateInfos(queueCreateInfos.data())
				.setQueueCreateInfoCount(static_cast<uint32_t>(queueCreateInfos.size()))
				.setPEnabledFeatures(&deviceFeatures)
				.setEnabledExtensionCount(0);

			device = physicalDevice.createDeviceUnique(createInfo);
			graphicsQueue = device->getQueue(graphicsQueueIndices[0], 0);
			presentQueue = device->getQueue(presentQueueIndices[0], 0);
		}

		uint32_t width;
		uint32_t height;
		GLFWwindow* window;
		vk::UniqueInstance instance;
		vk::PhysicalDevice physicalDevice;
		vk::UniqueSurfaceKHR surface;
		vk::UniqueDevice device;
		vk::Queue graphicsQueue;
		vk::Queue presentQueue;
	};
} // namespace ktwq