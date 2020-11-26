#pragma once

#include <vulkan/vulkan.hpp>

namespace ktw {
	class CommandBuffer;
	class IRenderTarget {
	public:
		virtual vk::Extent2D& getExtent() = 0;
		virtual vk::RenderPass& getRenderPass() = 0;
		virtual std::vector<vk::Framebuffer>& getFrameBuffers() = 0;
		virtual void acquireImage() = 0;
		virtual uint32_t acquiredImageIndex() = 0;
		virtual void submit(std::vector<ktw::CommandBuffer*>& commandBuffers) = 0;
	};
}