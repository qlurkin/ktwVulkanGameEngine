#pragma once

#include <vulkan/vulkan.hpp>

namespace ktw {
	class CommandBuffer;

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
		vk::UniqueRenderPass renderPass;
		std::vector<vk::UniqueFramebuffer> swapChainFramebuffers;
		vk::UniqueCommandPool commandPool;
		uint32_t imageIndex;
		std::vector<vk::CommandBuffer> postedCommandBuffers;
	};
}