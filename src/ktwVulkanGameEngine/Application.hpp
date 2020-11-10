#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <set>
#include <memory>
#include <cstdint>
#include <algorithm>
#include <optional>

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

		struct QueueFamilyIndices {
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool isComplete() {
				return graphicsFamily.has_value();
			}
		};

		struct SwapChainSupportDetails {
			vk::SurfaceCapabilitiesKHR capabilities;
			std::vector<vk::SurfaceFormatKHR> formats;
			std::vector<vk::PresentModeKHR> presentModes;
		};

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
			createSwapChain();
			createImageViews();
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

		bool checkDeviceExtensionSupport(vk::PhysicalDevice device, const std::vector<const char*> deviceExtensions) {
			std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

			std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

			for (const auto& extension : availableExtensions) {
				requiredExtensions.erase(extension.extensionName);
			}

			return requiredExtensions.empty();
		}

		SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device) {
			SwapChainSupportDetails details;

			details.capabilities = device.getSurfaceCapabilitiesKHR(*surface);
			details.formats = device.getSurfaceFormatsKHR(*surface);
			details.presentModes = device.getSurfacePresentModesKHR(*surface);

			return details;
		}

		// 0 means not suitable
		int rateDeviceSuitability(vk::PhysicalDevice device) {
			QueueFamilyIndices indices = findQueueFamilies(device);

			if(!indices.isComplete()) {
				return 0;
			}

			if(!checkDeviceExtensionSupport(device, deviceExtensions)) {
				return 0;
			}
			
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			if(swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
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
			queueIndices = findQueueFamilies(physicalDevice);
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

		QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device) {
			QueueFamilyIndices indices;
			std::vector<uint32_t> graphicsQueueIndices = findGraphicsQueueIndexes(device);
			std::vector<uint32_t> presentQueueIndices = findPresentQueueIndexes(device);
			
			// We can be smarter here ?
			if(graphicsQueueIndices.size() > 0)
				indices.graphicsFamily = graphicsQueueIndices[0];

			if(presentQueueIndices.size() > 0)
				indices.presentFamily = presentQueueIndices[0];

			return indices;
		}

		void createLogicalDevice() {
			std::set<uint32_t> uniqueQueueFamilyIndices = { queueIndices.graphicsFamily.value(), queueIndices.presentFamily.value() };

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
				.setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()))
				.setPpEnabledExtensionNames(deviceExtensions.data());

			device = physicalDevice.createDeviceUnique(createInfo);
			graphicsQueue = device->getQueue(queueIndices.graphicsFamily.value(), 0);
			presentQueue = device->getQueue(queueIndices.presentFamily.value(), 0);
		}

		vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
			for (const auto& availableFormat : availableFormats) {
				if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
						return availableFormat;
				}
			}

			return availableFormats[0];
		}

		vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
			for (const auto& availablePresentMode : availablePresentModes) {
				if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
						return availablePresentMode;
				}
			}

			return vk::PresentModeKHR::eFifo;
		}

		vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
			if (capabilities.currentExtent.width != UINT32_MAX) {
				return capabilities.currentExtent;
			} else {
				int width, height;
				glfwGetFramebufferSize(window, &width, &height);

				vk::Extent2D actualExtent = {
					static_cast<uint32_t>(width),
					static_cast<uint32_t>(height)
				};

				actualExtent.width = std::min(capabilities.maxImageExtent.width, actualExtent.width);
				actualExtent.height = std::min(capabilities.maxImageExtent.height, actualExtent.height);

				actualExtent.width = std::max(capabilities.minImageExtent.width, actualExtent.width);
				actualExtent.height = std::max(capabilities.minImageExtent.height, actualExtent.height);

				return actualExtent;
			}
		}

		void createSwapChain() {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

			vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
			vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
			vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

			uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

			if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
				imageCount = swapChainSupport.capabilities.maxImageCount;
			}

			auto createInfo = vk::SwapchainCreateInfoKHR()
				.setSurface(*surface)
				.setMinImageCount(imageCount)
				.setImageFormat(surfaceFormat.format)
				.setImageColorSpace(surfaceFormat.colorSpace)
				.setImageExtent(extent)
				.setImageArrayLayers(1)
				.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

			uint32_t queueFamilyIndices[] = {queueIndices.graphicsFamily.value(), queueIndices.presentFamily.value()};

			if (queueIndices.graphicsFamily != queueIndices.presentFamily) {
				createInfo
					.setImageSharingMode(vk::SharingMode::eConcurrent)
					.setQueueFamilyIndexCount(2)
					.setPQueueFamilyIndices(queueFamilyIndices);
			} else {
				createInfo
					.setImageSharingMode(vk::SharingMode::eExclusive)
					.setQueueFamilyIndexCount(0)
					.setPQueueFamilyIndices(nullptr);
			}
			createInfo
				.setPreTransform(swapChainSupport.capabilities.currentTransform)
				.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
				.setPresentMode(presentMode)
				.setClipped(VK_TRUE);

			swapChain = device->createSwapchainKHRUnique(createInfo);
			swapChainImages = device->getSwapchainImagesKHR(*swapChain);
			swapChainImageFormat = surfaceFormat.format;
			swapChainExtent = extent;
		}

		void createImageViews() {
			swapChainImageViews.resize(swapChainImages.size());

			for (size_t i = 0; i < swapChainImages.size(); i++) {
				auto createInfo = vk::ImageViewCreateInfo()
					.setImage(swapChainImages[i])
					.setViewType(vk::ImageViewType::e2D)
					.setFormat(swapChainImageFormat)
					.setComponents({
						vk::ComponentSwizzle::eIdentity,
						vk::ComponentSwizzle::eIdentity,
						vk::ComponentSwizzle::eIdentity,
						vk::ComponentSwizzle::eIdentity,
					})
					.setSubresourceRange(vk::ImageSubresourceRange()
						.setAspectMask(vk::ImageAspectFlagBits::eColor)
						.setLevelCount(1)
						.setLayerCount(1));
				swapChainImageViews[i] = device->createImageViewUnique(createInfo);
			}
		}

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		uint32_t width;
		uint32_t height;
		GLFWwindow* window;
		vk::UniqueInstance instance;
		vk::PhysicalDevice physicalDevice;
		vk::UniqueSurfaceKHR surface;
		vk::UniqueDevice device;
		vk::Queue graphicsQueue;
		vk::Queue presentQueue;
		QueueFamilyIndices queueIndices;
		vk::UniqueSwapchainKHR swapChain;
		std::vector<vk::Image> swapChainImages;
		vk::Format swapChainImageFormat;
		vk::Extent2D swapChainExtent;
		std::vector<vk::UniqueImageView> swapChainImageViews;
	};
} // namespace ktwq