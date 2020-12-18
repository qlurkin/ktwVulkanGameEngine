#include "pch.hpp"
#include "FrameBuffer.hpp"

namespace ktw {
	FrameBuffer::FrameBuffer(ktw::Context& context, vk::ImageView imageView, vk::RenderPass renderPass) : width(context.getWidth()), height(context.getHeight()), renderPass(renderPass) {
		auto framebufferInfo = vk::FramebufferCreateInfo()
			.setRenderPass(renderPass)
			.setAttachmentCount(1)
			.setPAttachments(&imageView)
			.setWidth(width)
			.setHeight(height)
			.setLayers(1);

		frameBuffer = context.getDevice().createFramebufferUnique(framebufferInfo);
		
		LOG_TRACE("Framebuffer Created");
	}

	uint32_t FrameBuffer::getWidth() {
		return width;
	}

	uint32_t FrameBuffer::getHeight() {
		return height;
	}

	vk::Framebuffer FrameBuffer::getHandle() {
		return *frameBuffer;
	}

	ktw::FrameBuffer& FrameBuffer::getFrameBuffer() {
		return *this;
	}

	vk::RenderPass FrameBuffer::getRenderPass() {
		return renderPass;
	}
}