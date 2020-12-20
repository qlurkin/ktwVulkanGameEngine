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

		LOG_INFO("New vk::DescriptorPool Created");
		return descriptorPool;
	}

	bool DescriptorPool::createDescriptorSet(vk::DescriptorPool descriptorPool, vk::DescriptorSet& descriptorSet, vk::DescriptorSetLayout layout) {
		auto allocInfo = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool(descriptorPool)
			.setDescriptorSetCount(1)
			.setPSetLayouts(&layout);

		try {
			descriptorSet = context.getDevice().allocateDescriptorSets(allocInfo)[0];
			return true;
		}
		catch(std::exception e) {
			LOG_WARN(e.what());
			return false;
		}
	}

	vk::DescriptorSet DescriptorPool::getDescriptorSet(ktw::FrameBuffer& frameBuffer, vk::DescriptorSetLayout layout) {
		vk::DescriptorPool pool;
		if(lockedDescriptorPool.find(frameBuffer.getHandle()) == lockedDescriptorPool.end()) {
			if(allocatedDescriptorPool.empty()) {
				pool = createDescriptorPool();
			}
			else {
				pool = *(allocatedDescriptorPool.begin());
				allocatedDescriptorPool.erase(allocatedDescriptorPool.begin());
			}
			lockedDescriptorPool[frameBuffer.getHandle()] = {pool};
		}
		else {
			pool = lockedDescriptorPool[frameBuffer.getHandle()].back();
		}

		vk::DescriptorSet set;
		if(!createDescriptorSet(pool, set, layout)) {
			pool = createDescriptorPool();
			lockedDescriptorPool[frameBuffer.getHandle()].push_back(pool);
			createDescriptorSet(pool, set, layout);
		}
		return set;
	}

	void DescriptorPool::freeDescriptorPools(ktw::FrameBuffer& frameBuffer) {
		for(auto descriptorPool : lockedDescriptorPool[frameBuffer.getHandle()]) {
			context.getDevice().resetDescriptorPool(descriptorPool);
			allocatedDescriptorPool.insert(descriptorPool);
		}

		lockedDescriptorPool.erase(frameBuffer.getHandle());
	}
}