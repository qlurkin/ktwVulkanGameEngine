#pragma once

#include "Device.hpp"
#include "IRenderTarget.hpp"
#include "Buffer.hpp"

namespace ktw {
	class UniformBuffer {
	public:
		UniformBuffer(ktw::Device& device, ktw::IRenderTarget& renderTarget, uint32_t size);
		void setData(void* data);
	private:
		uint32_t size;
		std::vector<ktw::Buffer> uniformBuffers;
		ktw::IRenderTarget& renderTarget;
	};
}