#pragma once

#include "Context.hpp"
#include "FrameBuffer.hpp"

#include <set>
#include <unordered_map>
#include <vector>

namespace ktw {
	class DescriptorPool {
	public:
		DescriptorPool(ktw::Context& context, uint32_t maxSets, uint32_t maxBuffers, uint32_t maxTextures);
		~DescriptorPool();
		vk::DescriptorSet getDescriptorSet(ktw::FrameBuffer& frameBuffer, vk::DescriptorSetLayout layout);
		void freeDescriptorPools(ktw::FrameBuffer& frameBuffer);

	private:
		ktw::Context& context;
		uint32_t maxSets;
		uint32_t maxBuffers;
		uint32_t maxTextures;
		std::set<vk::DescriptorPool> allocatedDescriptorPool;
		std::unordered_map<vk::Framebuffer, std::vector<vk::DescriptorPool>> lockedDescriptorPool;

		vk::DescriptorPool createDescriptorPool();
		bool createDescriptorSet(vk::DescriptorPool descriptorPool, vk::DescriptorSet& descriptorSet, vk::DescriptorSetLayout layout);
	};
}