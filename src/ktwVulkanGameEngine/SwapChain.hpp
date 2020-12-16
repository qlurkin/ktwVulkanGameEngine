#pragma once

#include "Device.hpp"
#include "CommandBuffer.hpp"
#include "FrameBuffer.hpp"

#include <optional>

namespace ktw {
	class SwapChain {
	public:
		SwapChain(ktw::Device& device, vk::SurfaceKHR& surface, uint32_t width, uint32_t height);
		vk::Extent2D& getExtent();
		vk::RenderPass& getRenderPass();
		//void submit(std::vector<ktw::CommandBuffer>& commandBuffers) override;
		//std::vector<vk::Framebuffer>& getFrameBuffers();
		//void acquireImage();
		//uint32_t acquiredImageIndex();
		//void setDescriptorPoolSize(uint32_t size);
		//vk::DescriptorPool& getDescriptorPool();
		ktw::FrameBuffer& nextFrameBuffer();
		void present(ktw::FrameBuffer& frameBuffer);

	private:
		uint32_t width, height;
		ktw::Device& device;
		vk::UniqueSwapchainKHR swapChain;
		std::vector<vk::Image> swapChainImages;
		vk::Format swapChainImageFormat;
		vk::Extent2D swapChainExtent;
		std::vector<vk::UniqueImageView> swapChainImageViews;
		vk::UniqueRenderPass renderPass;
		std::vector<ktw::FrameBuffer> swapChainFramebuffers;
		//std::vector<vk::Framebuffer> framebuffers; // Must be a better way
		uint32_t imageIndex;
		bool imageAcquired;
		vk::UniqueFence imageAvailableFence;
		//vk::UniqueSemaphore renderFinishedSemaphore;
		//vk::UniqueDescriptorPool descriptorPool;
		//bool descriptorPoolCreated;

		void createSurface(VkSurfaceKHR surface);
		vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
		vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
		vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
		void createSwapChain(ktw::Device& device, vk::SurfaceKHR& surface);
		void createImageViews(ktw::Device& device);
		void createRenderPass(ktw::Device& device);
		void createFramebuffers(ktw::Device& device);
		void createSemaphores(ktw::Device& device);
		//void createDescriptorPool(ktw::Device& device, uint32_t size);
	};
}