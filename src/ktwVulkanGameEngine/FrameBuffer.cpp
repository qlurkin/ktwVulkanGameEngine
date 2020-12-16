#include "pch.hpp"
#include "FrameBuffer.hpp"

namespace ktw {
	FrameBuffer::FrameBuffer(ktw::Device& device, vk::ImageView imageView, vk::RenderPass renderPass, uint32_t width, uint32_t height) : width(width), height(height), renderPass(renderPass) {
		auto framebufferInfo = vk::FramebufferCreateInfo()
			.setRenderPass(renderPass)
			.setAttachmentCount(1)
			.setPAttachments(&imageView)
			.setWidth(width)
			.setHeight(height)
			.setLayers(1);

		frameBuffer = device.getDevice().createFramebufferUnique(framebufferInfo);
		
		LOG_TRACE("Framebuffer Created");
	}

	uint32_t FrameBuffer::getWidth() {
		return width;
	}

	uint32_t FrameBuffer::getHeight() {
		return height;
	}

	vk::Framebuffer FrameBuffer::getFrameBuffer() {
		return *frameBuffer;
	}

	vk::RenderPass FrameBuffer::getRenderPass() {
		return renderPass;
	}
}