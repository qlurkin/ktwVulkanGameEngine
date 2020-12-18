#pragma once

#include <vulkan/vulkan.hpp>

namespace ktw {
	class FrameBuffer;
	class RenderTarget {
	public:
		virtual uint32_t getWidth() = 0;
		virtual uint32_t getHeight() = 0;
		virtual ktw::FrameBuffer& getFrameBuffer() = 0;
		virtual vk::RenderPass getRenderPass() = 0;
	};
}