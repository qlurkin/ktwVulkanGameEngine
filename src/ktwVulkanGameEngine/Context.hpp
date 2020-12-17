#pragma once

#include <vulkan/vulkan.hpp>

#include "Instance.hpp"

namespace ktw {
	class Context {
	public:
		Context(ktw::Instance& instance, VkSurfaceKHR surface, uint32_t width, uint32_t height);
		vk::Instance getInstance();
		vk::Device getDevice();
		vk::PhysicalDevice getPhysicalDevice();
		vk::Queue getGraphicsQueue();
		vk::Queue getPresentQueue();
		uint32_t getGraphicsQueueIndex();
		uint32_t getPresentQueueIndex();
		uint32_t getWidth();
		uint32_t getHeight();
		vk::SurfaceKHR getSurface();

	private:
		ktw::Instance& instance;
		vk::UniqueSurfaceKHR surface;
		vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> messenger;
		vk::PhysicalDevice physicalDevice;
		vk::UniqueDevice device;
		vk::Queue graphicsQueue;
		vk::Queue presentQueue;
		uint32_t graphicsQueueIndex;
		uint32_t presentQueueIndex;
		uint32_t width;
		uint32_t height;

		void pickPhysicalDevice();
		void createLogicalDevice();
	};
}