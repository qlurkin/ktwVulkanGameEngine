#pragma once

#include "Context.hpp"
#include "RenderTarget.hpp"

namespace ktw {
	class FrameBuffer : public RenderTarget {
	public:
		FrameBuffer(ktw::Context& context, vk::ImageView imageView, vk::RenderPass renderPass);
		uint32_t getWidth();
		uint32_t getHeight();
		vk::Framebuffer getHandle();
		ktw::FrameBuffer& getFrameBuffer();
		vk::RenderPass getRenderPass();

	private:
		vk::UniqueFramebuffer frameBuffer;
		vk::RenderPass renderPass;
		uint32_t width;
		uint32_t height;
	};
}