#pragma once

#include "IRenderTarget.hpp"
#include "Device.hpp"
#include "CommandBuffer.hpp"

#include <optional>

namespace ktw {
	class SwapChain : public IRenderTarget {
	public:
		SwapChain(ktw::Device& device, vk::SurfaceKHR& surface, int width, int height);
		vk::Extent2D& getExtent() override;
		vk::RenderPass& getRenderPass() override;
		void submit(std::vector<ktw::CommandBuffer*>& commandBuffers) override;
		std::vector<vk::Framebuffer>& getFrameBuffers() override;
		void acquireImage() override;
		uint32_t acquiredImageIndex() override;
		void setDescriptorPoolSize(uint32_t size) override;
		vk::DescriptorPool& getDescriptorPool() override;

	private:
		int width, height;
		ktw::Device& device;
		vk::UniqueSwapchainKHR swapChain;
		std::vector<vk::Image> swapChainImages;
		vk::Format swapChainImageFormat;
		vk::Extent2D swapChainExtent;
		std::vector<vk::UniqueImageView> swapChainImageViews;
		vk::UniqueRenderPass renderPass;
		std::vector<vk::UniqueFramebuffer> swapChainFramebuffers;
		std::vector<vk::Framebuffer> framebuffers; // Must be a better way
		uint32_t imageIndex;
		bool imageAcquired;
		vk::UniqueSemaphore imageAvailableSemaphore;
		vk::UniqueSemaphore renderFinishedSemaphore;
		vk::UniqueDescriptorPool descriptorPool;
		bool descriptorPoolCreated;

		void createSurface(VkSurfaceKHR surface);
		
		vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
		vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
		vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
		void createSwapChain(ktw::Device& device, vk::SurfaceKHR& surface);
		void createImageViews(ktw::Device& device);
		void createRenderPass(ktw::Device& device);
		void createFramebuffers(ktw::Device& device);
		void createSemaphores(ktw::Device& device);
		void createDescriptorPool(ktw::Device& device, uint32_t size);
	};
}