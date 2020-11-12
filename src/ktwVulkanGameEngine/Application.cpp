#include "Application.hpp"

#include "Log.hpp"

#include <vector>
#include <string>
#include <set>
#include <memory>
#include <cstdint>
#include <algorithm>

namespace ktw {
	Application::Application(uint32_t width, uint32_t height) {
		this->width = width;
		this->height = height;
	}

	Application::~Application() {}

	void Application::run() {
		initWindow();
		initVulkan();
		userSetup(*renderer);
		mainLoop();
		userCleanup(*renderer);
		cleanup();
	}

	ktw::Context& Application::getContext() {
		return context;
	}

	void Application::initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		
		window = glfwCreateWindow(width, height, "ktwVulkanGameEngine", nullptr, nullptr);
	}

	void Application::initVulkan() {
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
		createRenderPass();
		createFramebuffers();
		createCommandPool();
		createSemaphores();
		renderer.swap(std::unique_ptr<Renderer>(new Renderer(context)));
	}

	void Application::mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			prepareFrame();
			userUpdate(*renderer);
			drawFrame();
		}

		context.device->waitIdle();
	}
	void Application::prepareFrame() {
		context.postedCommandBuffers.clear();
		context.imageIndex = (context.device->acquireNextImageKHR(*(context.swapChain), UINT64_MAX, *imageAvailableSemaphore, {})).value;
	}

	void Application::drawFrame() {
		vk::Semaphore waitSemaphores[] = {*imageAvailableSemaphore};
		vk::Semaphore signalSemaphores[] = {*renderFinishedSemaphore};
		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eTopOfPipe};
		auto submitInfo = vk::SubmitInfo()
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(waitSemaphores)
			.setPWaitDstStageMask(waitStages)
			.setCommandBufferCount(static_cast<uint32_t>(context.postedCommandBuffers.size()))
			.setPCommandBuffers(context.postedCommandBuffers.data())
			.setSignalSemaphoreCount(1)
			.setPSignalSemaphores(signalSemaphores);
		
		if(context.graphicsQueue.submit(1, &submitInfo, {}) != vk::Result::eSuccess) {
			throw std::runtime_error("Error while submitting command buffers");
		}

		vk::SwapchainKHR swapChains[] = {*(context.swapChain)};

		auto presentInfo = vk::PresentInfoKHR()
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(signalSemaphores)
			.setSwapchainCount(1)
			.setPSwapchains(swapChains)
			.setPImageIndices(&(context.imageIndex))
			.setPResults({}); // Optional

		if(context.presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
			throw std::runtime_error("Error while presenting image to swap chain");
		}
		
		// TODO: Change this
		context.presentQueue.waitIdle();
	}

	void Application::cleanup() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Application::createSurface() {
		VkSurfaceKHR surface;
		if(glfwCreateWindowSurface(*(context.instance), window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}

		context.surface = vk::UniqueSurfaceKHR(surface, *(context.instance));
	}

	void Application::createInstance(std::vector<const char *> extensionNames) {
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

		context.instance = vk::createInstanceUnique(createInfo);
	}

	void Application::setupDebugMessenger() {
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

		auto dldi = vk::DispatchLoaderDynamic(*(context.instance), vkGetInstanceProcAddr);

		context.instance->createDebugUtilsMessengerEXTUnique(createInfo, nullptr, dldi);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL Application::debugCallback(
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

	bool Application::checkDeviceExtensionSupport(vk::PhysicalDevice device, const std::vector<const char*> deviceExtensions) {
		std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	Application::SwapChainSupportDetails Application::querySwapChainSupport(vk::PhysicalDevice device) {
		SwapChainSupportDetails details;

		details.capabilities = device.getSurfaceCapabilitiesKHR(*(context.surface));
		details.formats = device.getSurfaceFormatsKHR(*(context.surface));
		details.presentModes = device.getSurfacePresentModesKHR(*(context.surface));

		return details;
	}

	// 0 means not suitable
	int Application::rateDeviceSuitability(vk::PhysicalDevice device) {
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

	void Application::pickPhysicalDevice() {
		auto physicalDevices = context.instance->enumeratePhysicalDevices();

		int scoreMax = 0;

		for (const auto& device : physicalDevices) {
			int score = rateDeviceSuitability(device);
			if (score > scoreMax) {
				context.physicalDevice = device;
				scoreMax = score;
				break;
			}
		}

		if(scoreMax == 0) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		LOG_INFO("Selected GPU: {}", context.physicalDevice.getProperties().deviceName);
		queueIndices = findQueueFamilies(context.physicalDevice);
	}

	std::vector<uint32_t> Application::findGraphicsQueueIndexes(vk::PhysicalDevice device) {
		auto queueFamilyProperties = device.getQueueFamilyProperties();
		std::vector<uint32_t> res;
		for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
			if(queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
				res.push_back(i);
			}
		}
		return res;
	}

	std::vector<uint32_t> Application::findPresentQueueIndexes(vk::PhysicalDevice device) {
		auto queueFamilyProperties = device.getQueueFamilyProperties();
		std::vector<uint32_t> res;
		for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
			if (device.getSurfaceSupportKHR(static_cast<uint32_t>(i), context.surface.get())) {
				res.push_back(i);
			}
		}
		return res;
	}

	Application::QueueFamilyIndices Application::findQueueFamilies(vk::PhysicalDevice device) {
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

	void Application::createLogicalDevice() {
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

		context.device = context.physicalDevice.createDeviceUnique(createInfo);
		context.graphicsQueue = context.device->getQueue(queueIndices.graphicsFamily.value(), 0);
		context.presentQueue = context.device->getQueue(queueIndices.presentFamily.value(), 0);
	}

	vk::SurfaceFormatKHR Application::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
					return availableFormat;
			}
		}

		return availableFormats[0];
	}

	vk::PresentModeKHR Application::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
					return availablePresentMode;
			}
		}

		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D Application::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
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

	void Application::createSwapChain() {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(context.physicalDevice);

		vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		auto createInfo = vk::SwapchainCreateInfoKHR()
			.setSurface(*(context.surface))
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

		context.swapChain = context.device->createSwapchainKHRUnique(createInfo);
		context.swapChainImages = context.device->getSwapchainImagesKHR(*(context.swapChain));
		context.swapChainImageFormat = surfaceFormat.format;
		context.swapChainExtent = extent;
	}

	void Application::createImageViews() {
		context.swapChainImageViews.resize(context.swapChainImages.size());

		for (size_t i = 0; i < context.swapChainImages.size(); i++) {
			auto createInfo = vk::ImageViewCreateInfo()
				.setImage(context.swapChainImages[i])
				.setViewType(vk::ImageViewType::e2D)
				.setFormat(context.swapChainImageFormat)
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
			context.swapChainImageViews[i] = context.device->createImageViewUnique(createInfo);
		}
	}

	void Application::createRenderPass() {
		auto colorAttachment = vk::AttachmentDescription()
			.setFormat(context.swapChainImageFormat)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

		auto colorAttachmentRef = vk::AttachmentReference()
			.setAttachment(0)
			.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

		auto subpass = vk::SubpassDescription()
			.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setColorAttachmentCount(1)
			.setPColorAttachments(&colorAttachmentRef);

		auto renderPassInfo = vk::RenderPassCreateInfo()
			.setAttachmentCount(1)
			.setPAttachments(&colorAttachment)
			.setSubpassCount(1)
			.setPSubpasses(&subpass);

		context.renderPass = context.device->createRenderPassUnique(renderPassInfo);
	}

	void Application::createFramebuffers() {
		context.swapChainFramebuffers.resize(context.swapChainImageViews.size());

		for (size_t i = 0; i < context.swapChainImageViews.size(); i++) {
			vk::ImageView attachments[] = {
				*(context.swapChainImageViews[i])
			};

			auto framebufferInfo = vk::FramebufferCreateInfo()
				.setRenderPass(*(context.renderPass))
				.setAttachmentCount(1)
				.setPAttachments(attachments)
				.setWidth(context.swapChainExtent.width)
				.setHeight(context.swapChainExtent.height)
				.setLayers(1);

			context.swapChainFramebuffers[i] = context.device->createFramebufferUnique(framebufferInfo);
		}
	}

	void Application::createCommandPool() {
		auto poolInfo = vk::CommandPoolCreateInfo()
			.setQueueFamilyIndex(queueIndices.graphicsFamily.value());

		context.commandPool = context.device->createCommandPoolUnique(poolInfo);
	}

	void Application::createSemaphores() {
		auto semaphoreInfo = vk::SemaphoreCreateInfo();

		imageAvailableSemaphore = context.device->createSemaphoreUnique(semaphoreInfo);
		renderFinishedSemaphore = context.device->createSemaphoreUnique(semaphoreInfo);
	}
}