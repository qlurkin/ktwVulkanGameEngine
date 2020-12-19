#include "pch.hpp"
#include "DescriptorPool.hpp"

namespace ktw {
	DescriptorPool::DescriptorPool(ktw::Context& context, uint32_t maxSets, uint32_t maxBuffers, uint32_t maxTextures) : context(context), maxSets(maxSets), maxBuffers(maxBuffers), maxTextures(maxTextures) {

	}

	DescriptorPool::~DescriptorPool() {
		for(auto descriptorPool : allocatedDescriptorPool) {
			context.getDevice().destroyDescriptorPool(descriptorPool);
		}
	}

	vk::DescriptorPool DescriptorPool::createDescriptorPool() {
		auto poolSize = vk::DescriptorPoolSize()
			.setDescriptorCount(maxBuffers);

		auto poolInfo = vk::DescriptorPoolCreateInfo()
			.setPoolSizeCount(1)
			.setPPoolSizes(&poolSize)
			.setMaxSets(maxSets);

		vk::DescriptorPool descriptorPool = context.getDevice().createDescriptorPool(poolInfo);

		return descriptorPool;
	}

	vk::DescriptorSet DescriptorPool::getDescriptorSet(ktw::FrameBuffer& frameBuffer) {

	}

	void DescriptorPool::freeDescriptorPools(ktw::FrameBuffer& frameBuffer) {
		for(auto descriptorPool : lockedDescriptorPool[frameBuffer.getHandle()]) {
			context.getDevice().resetDescriptorPool(descriptorPool);
		}

		lockedDescriptorPool.erase(frameBuffer.getHandle());
	}
}