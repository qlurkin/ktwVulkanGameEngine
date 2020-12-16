#include "pch.hpp"
#include "Renderer.hpp"

namespace ktw {
	Renderer::Renderer(GLFWwindow* window, uint32_t width, uint32_t height) :
		instance(getGlfwRequiredInstanceExtensions()),
		surface(vk::UniqueSurfaceKHR(getSurfaceFromGlfw(window, instance.getInstance()), instance.getInstance())),
		device(instance, *surface),
		swapChain(device, *surface, width, height),
		commandPool(device)
	{
		auto fenceInfo = vk::FenceCreateInfo();
		renderFinishedFence = device.getDevice().createFenceUnique(fenceInfo);

		LOG_TRACE("Renderer Created");
	}

	std::vector<const char*> Renderer::getGlfwRequiredInstanceExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector<const char*> extensionNames(glfwExtensions, glfwExtensions + glfwExtensionCount);

		return extensionNames;
	}

	vk::SurfaceKHR Renderer::getSurfaceFromGlfw(GLFWwindow* window, vk::Instance& instance) {
		VkSurfaceKHR surface;
		if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}

		return surface;
	}
	
	ktw::GraphicsPipeline* Renderer::createGraphicsPipeline(uint32_t width, uint32_t height, vk::RenderPass renderPass, std::string vertexShader, std::string fragmentShader, const std::vector<ktw::VertexBufferBinding>& vertexBufferBindings, const std::vector<ktw::UniformDescriptor>& uniformDescriptors) {
		return new ktw::GraphicsPipeline(device, width, height, renderPass, vertexShader, fragmentShader, vertexBufferBindings, uniformDescriptors);
	}

	/*ktw::CommandBuffer* Renderer::createCommandBuffer(ktw::GraphicsPipeline* pipeline, ktw::Buffer* vertexBuffer, ktw::Buffer* indexBuffer) {
		return new ktw::CommandBuffer(device, swapChain, *pipeline, *vertexBuffer, *indexBuffer);
	}*/

	ktw::Buffer* Renderer::createBuffer(uint32_t itemSize, size_t count, ktw::BufferUsage usage, void* data) {
		return new ktw::Buffer(device, itemSize, static_cast<uint32_t>(count), usage, data);
	}

	void Renderer::waitDeviceIdle() {
		device.getDevice().waitIdle();
	}

	void Renderer::startFrame(vk::Framebuffer frameBuffer) {
		renderingFrameBuffer = frameBuffer;
		renderingFrame = true;
		postedCommandBuffers.clear();
	}

	void Renderer::endFrame() {
		//swapChain.submit(postedCommandBuffers);

		//vk::Semaphore signalSemaphores[] = {*renderFinishedSemaphore};
		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eTopOfPipe};
		auto submitInfo = vk::SubmitInfo()
			//.setWaitSemaphoreCount(1)
			//.setPWaitSemaphores(waitSemaphores)
			.setPWaitDstStageMask(waitStages)
			.setCommandBufferCount(static_cast<uint32_t>(postedCommandBuffers.size()))
			.setPCommandBuffers(postedCommandBuffers.data());
			//.setSignalSemaphoreCount(1)
			//.setPSignalSemaphores(signalSemaphores);
		
		if(device.getGraphicsQueue().submit(1, &submitInfo, *renderFinishedFence) != vk::Result::eSuccess) {
			throw std::runtime_error("Error while submitting command buffers");
		}
	}

	void Renderer::waitEndOfRender() {
		device.getDevice().waitForFences(1, &(*renderFinishedFence), true, UINT64_MAX);
		renderingFrame = false;
	}

	// void Renderer::post(ktw::CommandBuffer* commandBuffer) {
	// 	postedCommandBuffers.push_back(commandBuffer);
	// }

	ktw::Buffer* Renderer::createVertexBuffer(uint32_t itemSize, size_t count, void* data) {
		return createBuffer(itemSize, count, ktw::BufferUsage::eVertexBuffer, data);
	}

	ktw::Buffer* Renderer::createIndexBuffer(size_t count, void* data) {
		return createBuffer(sizeof(uint32_t), count, ktw::BufferUsage::eIndexBuffer, data);
	}

	ktw::UniformBuffer* Renderer::createUniformBuffer(uint32_t size) {
		return new ktw::UniformBuffer(device, size);
	}

	// void Renderer::setDescriptorPoolSize(uint32_t size) {
	// 	swapChain.setDescriptorPoolSize(size);
	// }

	void Renderer::startCommandBuffer() {
		if(recordingCommand) {
			throw std::runtime_error("Already recording a command buffer");
		}

		recordingCommandBuffer = commandPool.getCommandBuffer();
		recordingCommand = true;

		vk::ClearValue clearColor(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}));
		vk::Rect2D renderArea = { {0, 0}, swapChain.getExtent() };
		
		auto beginInfo = vk::CommandBufferBeginInfo()
			.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

		recordingCommandBuffer.begin(beginInfo);

		auto renderPassInfo = vk::RenderPassBeginInfo()
				.setRenderPass(swapChain.getRenderPass())
				.setFramebuffer(renderingFrameBuffer)
				.setRenderArea(renderArea)
				.setClearValueCount(1)
				.setPClearValues(&clearColor);

		recordingCommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	}

	void Renderer::endCommandBuffer() {
		if(!recordingCommand) {
			throw std::runtime_error("No command buffer started");
		}
		recordingCommandBuffer.endRenderPass();
		recordingCommandBuffer.end();
		postedCommandBuffers.push_back(recordingCommandBuffer);
		recordingCommand = false;
	}

}
