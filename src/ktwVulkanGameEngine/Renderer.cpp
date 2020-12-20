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

	ktw::CommandBuffer Renderer::startCommandBuffer() {
		if(!renderingFrameBuffer) {
			throw std::runtime_error("Frame not started");
		}

		vk::CommandBuffer commandBuffer = commandPool.getCommandBuffer();

		postedCommandBuffers.push_back(commandBuffer);

		return ktw::CommandBuffer(*renderingFrameBuffer, commandBuffer);
	}

}
