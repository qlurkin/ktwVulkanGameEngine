#include "pch.hpp"
#include "CommandBuffer.hpp"

namespace ktw {
	CommandBuffer::CommandBuffer(ktw::Device& device, ktw::IRenderTarget& renderTarget, ktw::GraphicsPipeline& pipeline, ktw::Buffer& vertexBuffer) {
		
		auto framebuffers = renderTarget.getFrameBuffers();
		auto allocInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool(device.getCommandPool())
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount((uint32_t) framebuffers.size());

		commandBuffers = device.getDevice().allocateCommandBuffersUnique(allocInfo);

		vk::ClearValue clearColor(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}));
		vk::Rect2D renderArea = { {0, 0}, renderTarget.getExtent() };

		for (size_t i = 0; i < commandBuffers.size(); i++) {
			auto beginInfo = vk::CommandBufferBeginInfo()
				.setPInheritanceInfo(nullptr); // Optional

			commandBuffers[i]->begin(beginInfo);

			auto renderPassInfo = vk::RenderPassBeginInfo()
				.setRenderPass(renderTarget.getRenderPass())
				.setFramebuffer(framebuffers[i])
				.setRenderArea(renderArea)
				.setClearValueCount(1)
				.setPClearValues(&clearColor);

			commandBuffers[i]->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

			commandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.getPipeline());

			vk::Buffer vertexBuffers[] = {vertexBuffer.getBuffer()};
			vk::DeviceSize offsets[] = {0};

			commandBuffers[i]->bindVertexBuffers(0, 1, vertexBuffers, offsets);

			commandBuffers[i]->draw(vertexBuffer.getCount(), 1, 0, 0);
			
			commandBuffers[i]->endRenderPass();

			commandBuffers[i]->end();
		}
		LOG_TRACE("Command Buffer Created");
	}

	vk::CommandBuffer& CommandBuffer::getCommandBuffer(size_t i) {
		return *(commandBuffers[i]);
	}
}