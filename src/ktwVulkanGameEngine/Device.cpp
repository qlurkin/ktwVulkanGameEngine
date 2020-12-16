#include "pch.hpp"
#include "Device.hpp"

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

namespace ktw {
	Device::Device(ktw::Instance& instance, vk::SurfaceKHR& surface) {
		pickPhysicalDevice(instance, surface);
		createLogicalDevice();
		LOG_TRACE("Device Created");
	}

	vk::Device& Device::getDevice() {
		return *device;
	}

	vk::PhysicalDevice& Device::getPhysicalDevice() {
		return physicalDevice;
	}

	vk::Queue& Device::getGraphicsQueue() {
		return graphicsQueue;
	}

	vk::Queue& Device::getPresentQueue() {
		return presentQueue;
	}

	bool Device::checkDeviceExtensionSupport(vk::PhysicalDevice& device, const std::vector<const char*>& deviceExtensions) {
		std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	// 0 means not suitable
	int Device::rateDeviceSuitability(vk::PhysicalDevice& device, vk::SurfaceKHR& surface) {
		QueueFamilyIndices indices = findQueueFamilies(device, surface);

		if(!indices.isComplete()) {
			return 0;
		}

		if(!checkDeviceExtensionSupport(device, deviceExtensions)) {
			return 0;
		}
		
		auto formats = device.getSurfaceFormatsKHR(surface);
		auto presentModes = device.getSurfacePresentModesKHR(surface);
		if(formats.empty() || presentModes.empty()) {
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

	void Device::pickPhysicalDevice(ktw::Instance& instance, vk::SurfaceKHR& surface) {
		auto physicalDevices = instance.getInstance().enumeratePhysicalDevices();

		int scoreMax = 0;

		for (auto& device : physicalDevices) {
			int score = rateDeviceSuitability(device, surface);
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
		queueIndices = findQueueFamilies(physicalDevice, surface);
	}

	std::vector<uint32_t> Device::findGraphicsQueueIndexes(vk::PhysicalDevice& device) {
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

	std::vector<uint32_t> Device::findPresentQueueIndexes(vk::PhysicalDevice& device, vk::SurfaceKHR& surface) {
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

	Device::QueueFamilyIndices Device::findQueueFamilies(vk::PhysicalDevice& device, vk::SurfaceKHR& surface) {
		QueueFamilyIndices indices;
		std::vector<uint32_t> graphicsQueueIndices = findGraphicsQueueIndexes(device);
		std::vector<uint32_t> presentQueueIndices = findPresentQueueIndexes(device, surface);
		
		// We can be smarter here ?
		if(graphicsQueueIndices.size() > 0)
			indices.graphicsFamily = graphicsQueueIndices[0];

		if(presentQueueIndices.size() > 0)
			indices.presentFamily = presentQueueIndices[0];

		return indices;
	}

	void Device::createLogicalDevice() {
		std::set<uint32_t> uniqueQueueFamilyIndices = { queueIndices.graphicsFamily.value(), queueIndices.presentFamily.value() };

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
		graphicsQueue = device->getQueue(queueIndices.graphicsFamily.value(), 0);
		presentQueue = device->getQueue(queueIndices.presentFamily.value(), 0);
		LOG_TRACE("Logical Device Created");
	}

	uint32_t Device::getGraphicsQueueIndex() {
		return queueIndices.graphicsFamily.value();
	}

	uint32_t Device::getPresentQueueIndex() {
		return queueIndices.presentFamily.value();
	}
}