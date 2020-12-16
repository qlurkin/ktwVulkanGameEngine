#include "pch.hpp"
#include "SwapChain.hpp"

namespace ktw {
	SwapChain::SwapChain(ktw::Device& device, vk::SurfaceKHR& surface, uint32_t width, uint32_t height) : width(width), height(height), device(device), imageAcquired(false) {
		createSwapChain(device, surface);
		createImageViews(device);
		createRenderPass(device);
		createFramebuffers(device);
		createSemaphores(device);
	}

	vk::SurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
					return availableFormat;
			}
		}

		return availableFormats[0];
	}

	vk::PresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
					return availablePresentMode;
			}
		}

		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D SwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		} else {
			//int width, height;
			//glfwGetFramebufferSize(window, &width, &height);

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

	void SwapChain::createSwapChain(ktw::Device& device, vk::SurfaceKHR& surface) {
		vk::SurfaceCapabilitiesKHR capabilities = device.getPhysicalDevice().getSurfaceCapabilitiesKHR(surface);
		std::vector<vk::SurfaceFormatKHR> formats = device.getPhysicalDevice().getSurfaceFormatsKHR(surface);
		std::vector<vk::PresentModeKHR> presentModes = device.getPhysicalDevice().getSurfacePresentModesKHR(surface);

		vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(formats);
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(presentModes);
		vk::Extent2D extent = chooseSwapExtent(capabilities);

		uint32_t imageCount = capabilities.minImageCount + 1;

		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
			imageCount = capabilities.maxImageCount;
		}

		auto createInfo = vk::SwapchainCreateInfoKHR()
			.setSurface(surface)
			.setMinImageCount(imageCount)
			.setImageFormat(surfaceFormat.format)
			.setImageColorSpace(surfaceFormat.colorSpace)
			.setImageExtent(extent)
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

		uint32_t queueFamilyIndices[] = {device.getGraphicsQueueIndex(), device.getPresentQueueIndex()};

		if (device.getGraphicsQueueIndex() != device.getPresentQueueIndex()) {
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
			.setPreTransform(capabilities.currentTransform)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setPresentMode(presentMode)
			.setClipped(VK_TRUE);

		swapChain = device.getDevice().createSwapchainKHRUnique(createInfo);
		swapChainImages = device.getDevice().getSwapchainImagesKHR(*swapChain);
		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
		LOG_TRACE("SwapChain Created");
	}

	void SwapChain::createImageViews(ktw::Device& device) {
		swapChainImageViews.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); i++) {
			auto createInfo = vk::ImageViewCreateInfo()
				.setImage(swapChainImages[i])
				.setViewType(vk::ImageViewType::e2D)
				.setFormat(swapChainImageFormat)
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
			swapChainImageViews[i] = device.getDevice().createImageViewUnique(createInfo);
		}
		LOG_TRACE("Image Views ({}) Created", swapChainImageViews.size());
	}

	void SwapChain::createRenderPass(ktw::Device& device) {
		auto colorAttachment = vk::AttachmentDescription()
			.setFormat(swapChainImageFormat)
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

		renderPass = device.getDevice().createRenderPassUnique(renderPassInfo);
		LOG_TRACE("RenderPass Created");
	}

	void SwapChain::createFramebuffers(ktw::Device& device) {
		swapChainFramebuffers.reserve(swapChainImageViews.size());

		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			swapChainFramebuffers.emplace_back(device, swapChainImageViews[i], renderPass, width, height);
		}

		LOG_TRACE("Framebuffers ({}) Created", swapChainFramebuffers.size());
	}

	void SwapChain::createSemaphores(ktw::Device& device) {
		//auto semaphoreInfo = vk::SemaphoreCreateInfo();
		auto fenceInfo = vk::FenceCreateInfo();

		imageAvailableFence = device.getDevice().createFenceUnique(fenceInfo);
		//renderFinishedSemaphore = device.getDevice().createSemaphoreUnique(semaphoreInfo);
		LOG_TRACE("Semaphore/Fence Created");
	}

	vk::Extent2D& SwapChain::getExtent() {
		return swapChainExtent;
	}

	vk::RenderPass& SwapChain::getRenderPass() {
		return *renderPass;
	}

	ktw::FrameBuffer& SwapChain::nextFrameBuffer() {
		imageIndex = (device.getDevice().acquireNextImageKHR(*swapChain, UINT64_MAX, {}, *imageAvailableFence)).value;
		device.getDevice().waitForFences(1, &(*imageAvailableFence), true, UINT64_MAX);
		return swapChainFramebuffers[imageIndex];
	}

	void SwapChain::present(ktw::FrameBuffer& frameBuffer) {
		vk::SwapchainKHR swapChains[] = {*swapChain};

		uint32_t index = 0;
		for(; index < swapChainFramebuffers.size(); index++) {
			if(swapChainFramebuffers[index].getFrameBuffer() == frameBuffer.getFrameBuffer()) {
				break;
			}
		}
		if(index == swapChainFramebuffers.size()) {
			throw std::runtime_error("Cannot present a framebuffer that not comes from the swapChain");
		}

		auto presentInfo = vk::PresentInfoKHR()
			.setSwapchainCount(1)
			.setPSwapchains(swapChains)
			.setPImageIndices(&index);

		if(device.getPresentQueue().presentKHR(presentInfo) != vk::Result::eSuccess) {
			throw std::runtime_error("Error while presenting image to swap chain");
		}
		
		// TODO: Change this
		device.getPresentQueue().waitIdle();
	}

	// std::vector<vk::Framebuffer>& SwapChain::getFrameBuffers() {
	// 	// std::vector<vk::Framebuffer> framebuffers(swapChainFramebuffers.size());
	// 	// for(size_t i = 0; i < swapChainFramebuffers.size(); i++) {
	// 	// 	framebuffers[i] = *(swapChainFramebuffers[i]);
	// 	// }
	// 	return framebuffers;
	// }

	// void SwapChain::acquireImage() {
	// 	if(imageAcquired) {
	// 		throw std::runtime_error("Image already acquired");
	// 	}
	// 	imageIndex = (device.getDevice().acquireNextImageKHR(*swapChain, UINT64_MAX, *imageAvailableSemaphore, {})).value;
	// 	imageAcquired = true;
	// }

	// void SwapChain::submit(std::vector<ktw::CommandBuffer>& commandBuffers) {
	// 	if(!imageAcquired) {
	// 		throw std::runtime_error("No image acquired");
	// 	}
	// 	std::vector<vk::CommandBuffer> vulkanCommandBuffers(commandBuffers.size());
	// 	for(size_t i = 0; i < commandBuffers.size(); i++) {
	// 		vulkanCommandBuffers[i] = commandBuffers[i]->getCommandBuffer(imageIndex);
	// 	}

	// 	vk::Semaphore waitSemaphores[] = {*imageAvailableSemaphore};
	// 	vk::Semaphore signalSemaphores[] = {*renderFinishedSemaphore};
	// 	vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eTopOfPipe};
	// 	auto submitInfo = vk::SubmitInfo()
	// 		.setWaitSemaphoreCount(1)
	// 		.setPWaitSemaphores(waitSemaphores)
	// 		.setPWaitDstStageMask(waitStages)
	// 		.setCommandBufferCount(static_cast<uint32_t>(vulkanCommandBuffers.size()))
	// 		.setPCommandBuffers(vulkanCommandBuffers.data())
	// 		.setSignalSemaphoreCount(1)
	// 		.setPSignalSemaphores(signalSemaphores);
		
	// 	if(device.getGraphicsQueue().submit(1, &submitInfo, {}) != vk::Result::eSuccess) {
	// 		throw std::runtime_error("Error while submitting command buffers");
	// 	}

	// 	vk::SwapchainKHR swapChains[] = {*swapChain};

	// 	auto presentInfo = vk::PresentInfoKHR()
	// 		.setWaitSemaphoreCount(1)
	// 		.setPWaitSemaphores(signalSemaphores)
	// 		.setSwapchainCount(1)
	// 		.setPSwapchains(swapChains)
	// 		.setPImageIndices(&imageIndex)
	// 		.setPResults({}); // Optional

	// 	if(device.getPresentQueue().presentKHR(presentInfo) != vk::Result::eSuccess) {
	// 		throw std::runtime_error("Error while presenting image to swap chain");
	// 	}
		
	// 	// TODO: Change this
	// 	device.getPresentQueue().waitIdle();
	// 	imageAcquired = false;
	// }

	// uint32_t SwapChain::acquiredImageIndex() {
	// 	if(imageAcquired)
	// 		return imageIndex;
	// 	throw std::runtime_error("No acquired Image");
	// }

	// void SwapChain::createDescriptorPool(ktw::Device& device, uint32_t size) {
	// 	auto poolSize = vk::DescriptorPoolSize()
	// 		.setType(vk::DescriptorType::eUniformBuffer)
	// 		.setDescriptorCount(size * static_cast<uint32_t>(swapChainImages.size()));

	// 	auto poolInfo = vk::DescriptorPoolCreateInfo()
	// 		.setPoolSizeCount(1)
	// 		.setPPoolSizes(&poolSize)
	// 		.setMaxSets(size * static_cast<uint32_t>(swapChainImages.size()));

	// 	descriptorPool = device.getDevice().createDescriptorPoolUnique(poolInfo);
	// 	descriptorPoolCreated = true;
	// 	LOG_TRACE("Descriptor Pool Created");
	// }

	// void SwapChain::setDescriptorPoolSize(uint32_t size) {
	// 	if(descriptorPoolCreated) {
	// 		throw std::runtime_error("Descriptor Pool already created");
	// 	}
	// 	createDescriptorPool(device, size);
	// }

	// vk::DescriptorPool& SwapChain::getDescriptorPool() {
	// 	if(!descriptorPoolCreated) {
	// 		setDescriptorPoolSize(2);
	// 	}
	// 	return *descriptorPool;
	// }
}