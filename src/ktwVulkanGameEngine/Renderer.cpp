#include "pch.hpp"
#include "Renderer.hpp"

namespace ktw {
	Renderer::Renderer(ktw::Context& context) :
		context(context),
		commandPool(context),
		descriptorPool(context, 2, 2, 2)
	{
		auto fenceInfo = vk::FenceCreateInfo();
		renderFinishedFence = context.getDevice().createFenceUnique(fenceInfo);

		LOG_TRACE("Renderer Created");
	}
	
	ktw::GraphicsPipeline* Renderer::createGraphicsPipeline(ktw::RenderTarget* renderTarget, std::string vertexShader, std::string fragmentShader, const std::vector<ktw::VertexBufferBinding>& vertexBufferBindings, const std::vector<ktw::UniformDescriptor>& uniformDescriptors) {
		return new ktw::GraphicsPipeline(context, *renderTarget, vertexShader, fragmentShader, vertexBufferBindings, uniformDescriptors);
	}

	ktw::Buffer* Renderer::createBuffer(uint32_t itemSize, size_t count, ktw::BufferUsage usage, void* data) {
		return new ktw::Buffer(context, itemSize, static_cast<uint32_t>(count), usage, data);
	}

	void Renderer::waitDeviceIdle() {
		context.getDevice().waitIdle();
	}

	void Renderer::startFrame(ktw::FrameBuffer& frameBuffer) {
		renderingFrameBuffer = &frameBuffer;
	}

	void Renderer::endFrame() {
		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eTopOfPipe};
		auto submitInfo = vk::SubmitInfo()
			.setPWaitDstStageMask(waitStages)
			.setCommandBufferCount(static_cast<uint32_t>(postedCommandBuffers.size()))
			.setPCommandBuffers(postedCommandBuffers.data());
		
		context.getDevice().resetFences(*renderFinishedFence);
		if(context.getGraphicsQueue().submit(1, &submitInfo, *renderFinishedFence) != vk::Result::eSuccess) {
			throw std::runtime_error("Error while submitting command buffers");
		}
	}

	void Renderer::waitEndOfRender() {
		auto result = context.getDevice().waitForFences(1, &(*renderFinishedFence), true, UINT64_MAX);
		renderingFrameBuffer = nullptr;
		for(auto buffer: postedCommandBuffers) {
			commandPool.freeCommandBuffer(buffer);
		}
		postedCommandBuffers.clear();
	}

	ktw::Buffer* Renderer::createVertexBuffer(uint32_t itemSize, size_t count, void* data) {
		return createBuffer(itemSize, count, ktw::BufferUsage::eVertexBuffer, data);
	}

	ktw::Buffer* Renderer::createIndexBuffer(size_t count, void* data) {
		return createBuffer(sizeof(uint32_t), count, ktw::BufferUsage::eIndexBuffer, data);
	}

	// ktw::UniformBuffer* Renderer::createUniformBuffer(uint32_t size) {
	// 	return new ktw::UniformBuffer(context, size);
	// }

	// void Renderer::setDescriptorPoolSize(uint32_t size) {
	// 	swapChain.setDescriptorPoolSize(size);
	// }

	ktw::CommandBuffer Renderer::startCommandBuffer() {
		if(!renderingFrameBuffer) {
			throw std::runtime_error("Frame not started");
		}

		vk::CommandBuffer commandBuffer = commandPool.getCommandBuffer();

		/*vk::ClearValue clearColor(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}));
		vk::Rect2D renderArea = { {0, 0}, {renderingFrameBuffer->getWidth(), renderingFrameBuffer->getHeight()} };
		
		auto beginInfo = vk::CommandBufferBeginInfo()
			.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

		recordingCommandBuffer.begin(beginInfo);

		auto renderPassInfo = vk::RenderPassBeginInfo()
				.setRenderPass(renderingFrameBuffer->getRenderPass())
				.setFramebuffer(renderingFrameBuffer->getHandle())
				.setRenderArea(renderArea)
				.setClearValueCount(1)
				.setPClearValues(&clearColor);

		recordingCommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);*/

		return ktw::CommandBuffer(*renderingFrameBuffer, commandBuffer);
	}

	void Renderer::postCommandBuffer(ktw::CommandBuffer& commandBuffer) {
		postedCommandBuffers.push_back(commandBuffer.getHandle());
	}

	/*void Renderer::endCommandBuffer() {
		if(!recordingCommand) {
			throw std::runtime_error("No command buffer started");
		}
		recordingCommandBuffer.endRenderPass();
		recordingCommandBuffer.end();
		postedCommandBuffers.push_back(recordingCommandBuffer);
		recordingCommand = false;
	}

	void Renderer::bindPipeline(ktw::GraphicsPipeline* pipeline) {
		if(!recordingCommand) {
			throw std::runtime_error("No command buffer started");
		}
		recordingCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->getPipeline());
	}

	void Renderer::bindVertexBuffer(ktw::Buffer* buffer) {
		if(!recordingCommand) {
			throw std::runtime_error("No command buffer started");
		}
		vk::Buffer vertexBuffers[] = {buffer->getBuffer()};
		vk::DeviceSize offsets[] = {0};

		recordingCommandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
	}

	void Renderer::bindIndexBuffer(ktw::Buffer* buffer) {
		if(!recordingCommand) {
			throw std::runtime_error("No command buffer started");
		}
		recordingCommandBuffer.bindIndexBuffer(buffer->getBuffer(), 0, vk::IndexType::eUint32);
	}

	void Renderer::drawIndexed(uint32_t count) {
		if(!recordingCommand) {
			throw std::runtime_error("No command buffer started");
		}
		recordingCommandBuffer.drawIndexed(count, 1, 0, 0, 0);
	}*/

}
