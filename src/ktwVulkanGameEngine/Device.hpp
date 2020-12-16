#pragma once

#include <vulkan/vulkan.hpp>

#include "Instance.hpp"

#include <optional>

namespace ktw {
	class Device {
	public:
		Device(ktw::Instance& instance, vk::SurfaceKHR& surface);
		vk::Device& getDevice();
		vk::PhysicalDevice& getPhysicalDevice();
		vk::Queue& getGraphicsQueue();
		vk::Queue& getPresentQueue();
		uint32_t getGraphicsQueueIndex();
		uint32_t getPresentQueueIndex();

	private:
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

		vk::PhysicalDevice physicalDevice;
		vk::UniqueDevice device;
		vk::Queue graphicsQueue;
		vk::Queue presentQueue;
		QueueFamilyIndices queueIndices;

		bool checkDeviceExtensionSupport(vk::PhysicalDevice& device, const std::vector<const char*>& deviceExtensions);
		int rateDeviceSuitability(vk::PhysicalDevice& device, vk::SurfaceKHR& surface);  // 0 means not suitable
		void pickPhysicalDevice(ktw::Instance& instance, vk::SurfaceKHR& surface);
		std::vector<uint32_t> findGraphicsQueueIndexes(vk::PhysicalDevice& device);
		std::vector<uint32_t> findPresentQueueIndexes(vk::PhysicalDevice& device, vk::SurfaceKHR& surface);
		QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice& device, vk::SurfaceKHR& surface);
		void createLogicalDevice();
	};
}