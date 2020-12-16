#pragma once

#include "Device.hpp"

namespace ktw {
	class FrameBuffer {
	public:
		FrameBuffer(ktw::Device& device, vk::ImageView imageView, vk::RenderPass renderPass, uint32_t width, uint32_t height);
		uint32_t getWidth();
		uint32_t getHeight();
		vk::Framebuffer getFrameBuffer();
		vk::RenderPass getRenderPass();

	private:
		vk::UniqueFramebuffer frameBuffer;
		vk::RenderPass renderPass;
		uint32_t width;
		uint32_t height;
	};
}