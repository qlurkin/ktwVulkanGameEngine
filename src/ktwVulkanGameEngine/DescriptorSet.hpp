#pragma once

#include "Context.hpp"

namespace ktw {
	class DescriptorSet {
	public:
		DescriptorSet(ktw::Context& context);

	private:
		vk::UniqueDescriptorSet descriptorSet;
	};
}