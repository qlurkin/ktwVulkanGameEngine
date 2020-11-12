#pragma once

#include "GraphicsPipeline.hpp"

namespace ktw {
	class CommandBuffer {
	public:
		CommandBuffer(ktw::Context& context, ktw::GraphicsPipeline& pipeline) : context(context), pipeline(pipeline) {
			commandBuffers.resize(context.swapChainFramebuffers.size());

			auto allocInfo = vk::CommandBufferAllocateInfo()
				.setCommandPool(*(context.commandPool))
				.setLevel(vk::CommandBufferLevel::ePrimary)
				.setCommandBufferCount((uint32_t) commandBuffers.size());

			commandBuffers = context.device->allocateCommandBuffersUnique(allocInfo);

			vk::ClearValue clearColor(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}));
			vk::Rect2D renderArea = { {0, 0}, context.swapChainExtent };

			for (size_t i = 0; i < commandBuffers.size(); i++) {
				auto beginInfo = vk::CommandBufferBeginInfo()
					.setPInheritanceInfo(nullptr); // Optional

				commandBuffers[i]->begin(beginInfo);

				auto renderPassInfo = vk::RenderPassBeginInfo()
					.setRenderPass(*(context.renderPass))
					.setFramebuffer(*(context.swapChainFramebuffers[i]))
					.setRenderArea(renderArea)
					.setClearValueCount(1)
					.setPClearValues(&clearColor);

				commandBuffers[i]->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

				commandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.getPipeline());
				commandBuffers[i]->draw(3, 1, 0, 0);
				
				commandBuffers[i]->endRenderPass();

				commandBuffers[i]->end();
			}
		}
		
		vk::CommandBuffer& getCommandBuffer(size_t i) {
			return *(commandBuffers[i]);
		}
		
	private:
		ktw::Context& context;
		ktw::GraphicsPipeline& pipeline;
		std::vector<vk::UniqueCommandBuffer> commandBuffers;
	};
}