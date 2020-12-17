#include "pch.hpp"
#include "Context.hpp"

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

namespace ktw {
	Context::Context(ktw::Instance& instance, VkSurfaceKHR surface, uint32_t width, uint32_t height) : instance(instance), width(width), height(height) {
		this->surface = vk::UniqueSurfaceKHR(surface, getInstance());
		pickPhysicalDevice();
		createLogicalDevice();
	}

	vk::Instance Context::getInstance() {
		return instance.getInstance();
	}

	vk::Device Context::getDevice() {
		return *device;
	}

	vk::PhysicalDevice Context::getPhysicalDevice() {
		return physicalDevice;
	}

	vk::Queue Context::getGraphicsQueue() {
		return graphicsQueue;
	}

	vk::Queue Context::getPresentQueue() {
		return presentQueue;
	}

	uint32_t Context::getGraphicsQueueIndex() {
		return graphicsQueueIndex;
	}

	uint32_t Context::getPresentQueueIndex() {
		return presentQueueIndex;
	}

	std::vector<uint32_t> findGraphicsQueueIndices(vk::PhysicalDevice device) {
		auto queueFamilyProperties = device.getQueueFamilyProperties();
		std::vector<uint32_t> res;
		res.reserve(queueFamilyProperties.size());
		for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
			if(queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
				res.push_back(i);
			}
		}
		return res;
	}

	std::vector<uint32_t> findPresentQueueIndices(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
		auto queueFamilyProperties = device.getQueueFamilyProperties();
		std::vector<uint32_t> res;
		res.reserve(queueFamilyProperties.size());
		for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
			if (device.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface)) {
				res.push_back(i);
			}
		}
		return res;
	}

	bool checkDeviceExtensionSupport(vk::PhysicalDevice device) {
		std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	uint32_t rateDeviceSuitability(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {
		
		if(!checkDeviceExtensionSupport(physicalDevice)) {
			return 0;
		}

		if(findGraphicsQueueIndices(physicalDevice).size() == 0) {
			return 0;
		}

		auto formats = physicalDevice.getSurfaceFormatsKHR(surface);
		auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
		if(formats.empty() || presentModes.empty()) {
			return 0;
		}

		auto deviceProperties = physicalDevice.getProperties();
		
		int score = 0;

		// Discrete GPUs have a significant performance advantage
		if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			score += 1000;
		}

		// Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

		return score;
	}

	void Context::pickPhysicalDevice() {
		auto physicalDevices = getInstance().enumeratePhysicalDevices();

		int scoreMax = 0;

		for (auto& device : physicalDevices) {
			int score = rateDeviceSuitability(device, *surface);
			if (score > scoreMax) {
				physicalDevice = device;
				scoreMax = score;
			}
		}

		if(scoreMax == 0) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
		LOG_INFO("Selected GPU: {}", physicalDevice.getProperties().deviceName);
	}

	void Context::createLogicalDevice() {
		auto graphicsQueueIndices = findGraphicsQueueIndices(physicalDevice);
		auto presentQueueIndices = findPresentQueueIndices(physicalDevice, *surface);

		graphicsQueueIndex = graphicsQueueIndices[0];
		presentQueueIndex = graphicsQueueIndices[0];

		std::set<uint32_t> uniqueQueueFamilyIndices = { graphicsQueueIndex, presentQueueIndex};

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		queueCreateInfos.resize(uniqueQueueFamilyIndices.size());

		float queuePriority = 1.0f;
		size_t i = 0;
		for (uint32_t queueFamilyIndex : uniqueQueueFamilyIndices) {
			queueCreateInfos[i]
				.setQueueFamilyIndex(queueFamilyIndex)
				.setQueueCount(1)
				.setPQueuePriorities(&queuePriority);
			i++;
		}

		vk::PhysicalDeviceFeatures deviceFeatures{};

		auto createInfo = vk::DeviceCreateInfo()
			.setPQueueCreateInfos(queueCreateInfos.data())
			.setQueueCreateInfoCount(static_cast<uint32_t>(queueCreateInfos.size()))
			.setPEnabledFeatures(&deviceFeatures)
			.setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()))
			.setPpEnabledExtensionNames(deviceExtensions.data());

		device = physicalDevice.createDeviceUnique(createInfo);
		graphicsQueue = device->getQueue(graphicsQueueIndex, 0);
		presentQueue = device->getQueue(presentQueueIndex, 0);
		LOG_TRACE("Logical Device Created");
	}

	uint32_t Context::getWidth() {
		return width;
	}

	uint32_t Context::getHeight() {
		return height;
	}

	vk::SurfaceKHR Context::getSurface() {
		return *surface;
	}
}