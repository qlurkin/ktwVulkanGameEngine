#pragma once

#include "Device.hpp"

namespace ktw {
	class DescriptorSet {
	public:
		DescriptorSet(ktw::Device& device);

	private:
		vk::UniqueDescriptorSet descriptorSet;
	};
}