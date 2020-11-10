#pragma once

#include <vulkan/vulkan.hpp>

namespace ktw {
	struct Context {
		vk::UniqueInstance instance;
		vk::PhysicalDevice physicalDevice;
		vk::UniqueSurfaceKHR surface;
		vk::UniqueDevice device;
		vk::Queue graphicsQueue;
		vk::Queue presentQueue;
		vk::UniqueSwapchainKHR swapChain;
		std::vector<vk::Image> swapChainImages;
		vk::Format swapChainImageFormat;
		vk::Extent2D swapChainExtent;
		std::vector<vk::UniqueImageView> swapChainImageViews;
	};
}