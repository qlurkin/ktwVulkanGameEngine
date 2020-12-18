#pragma once

#include "Context.hpp"
#include "FrameBuffer.hpp"
#include "RenderTarget.hpp"

#include <optional>

namespace ktw {
	class SwapChain : public RenderTarget {
	public:
		SwapChain(ktw::Context& context);
		uint32_t getWidth() override;
		uint32_t getHeight() override;
		vk::Extent2D& getExtent();
		vk::RenderPass getRenderPass() override;
		//void submit(std::vector<ktw::CommandBuffer>& commandBuffers) override;
		//std::vector<vk::Framebuffer>& getFrameBuffers();
		//void acquireImage();
		//uint32_t acquiredImageIndex();
		//void setDescriptorPoolSize(uint32_t size);
		//vk::DescriptorPool& getDescriptorPool();
		ktw::FrameBuffer& getFrameBuffer() override;
		void present(ktw::FrameBuffer& frameBuffer);

	private:
		ktw::Context& context;
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
		void createSwapChain();
		void createImageViews();
		void createRenderPass();
		void createFramebuffers();
		void createSemaphores();
		//void createDescriptorPool(ktw::Device& device, uint32_t size);
	};
}