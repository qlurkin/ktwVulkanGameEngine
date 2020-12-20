#include "pch.hpp"
#include "CommandBuffer.hpp"

namespace ktw {
	CommandBuffer::CommandBuffer(ktw::FrameBuffer& framebuffer, vk::CommandBuffer commandBuffer) : commandBuffer(commandBuffer) {
		vk::ClearValue clearColor(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}));
		vk::Rect2D renderArea = { {0, 0}, {framebuffer.getWidth(), framebuffer.getHeight()} };
		
		auto beginInfo = vk::CommandBufferBeginInfo()
			.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

		commandBuffer.begin(beginInfo);

		auto renderPassInfo = vk::RenderPassBeginInfo()
				.setRenderPass(framebuffer.getRenderPass())
				.setFramebuffer(framebuffer.getHandle())
				.setRenderArea(renderArea)
				.setClearValueCount(1)
				.setPClearValues(&clearColor);

		commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	}

	void CommandBuffer::end() {
		commandBuffer.endRenderPass();
		commandBuffer.end();
	}

	void CommandBuffer::bindPipeline(ktw::GraphicsPipeline* pipeline) {
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->getPipeline());
	}

	void CommandBuffer::bindVertexBuffer(ktw::Buffer* buffer) {
		vk::Buffer vertexBuffers[] = {buffer->getBuffer()};
		vk::DeviceSize offsets[] = {0};

		commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
	}

	void CommandBuffer::CommandBuffer::bindIndexBuffer(ktw::Buffer* buffer) {
		commandBuffer.bindIndexBuffer(buffer->getBuffer(), 0, vk::IndexType::eUint32);
	}

	void CommandBuffer::drawIndexed(uint32_t count) {
		commandBuffer.drawIndexed(count, 1, 0, 0, 0);
	}

	vk::CommandBuffer CommandBuffer::getHandle() {
		return commandBuffer;
	}
}