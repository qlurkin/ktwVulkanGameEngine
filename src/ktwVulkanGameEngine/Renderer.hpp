#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "GraphicsPipeline.hpp"
#include "Buffer.hpp"
#include "CommandPool.hpp"
#include "Context.hpp"
#include "FrameBuffer.hpp"

namespace ktw {
	class Renderer {
	public:
		Renderer(ktw::Context& context);

		ktw::GraphicsPipeline* createGraphicsPipeline(ktw::RenderTarget* renderTarget, std::string vertexShader, std::string fragmentShader, const std::vector<ktw::VertexBufferBinding>& vertexBufferBindings, const std::vector<ktw::UniformDescriptor>& uniformDescriptors);
		ktw::Buffer* createBuffer(uint32_t itemSize, size_t count, ktw::BufferUsage usage, void* data);
		ktw::Buffer* createVertexBuffer(uint32_t itemSize, size_t count, void* data);
		ktw::Buffer* createIndexBuffer(size_t count, void* data);
		//ktw::UniformBuffer* createUniformBuffer(uint32_t size);
		void waitDeviceIdle();
		void startFrame(ktw::FrameBuffer& frameBuffer);
		void endFrame();
		void waitEndOfRender();
		void setDescriptorPoolSize(uint32_t size);
		void startCommandBuffer();
		void endCommandBuffer();
		void bindPipeline(ktw::GraphicsPipeline* pipeline);
		void bindVertexBuffer(ktw::Buffer* buffer);
		void bindIndexBuffer(ktw::Buffer* buffer);
		void drawIndexed(uint32_t count);

	private:
		ktw::Context& context;
		std::vector<vk::CommandBuffer> postedCommandBuffers;
		vk::CommandBuffer recordingCommandBuffer;
		ktw::FrameBuffer* renderingFrameBuffer = nullptr;
		//bool renderingFrame = false;
		bool recordingCommand = false;
		ktw::CommandPool commandPool;
		vk::UniqueFence renderFinishedFence;
	};
}