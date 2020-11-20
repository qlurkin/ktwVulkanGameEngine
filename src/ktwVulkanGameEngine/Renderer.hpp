#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "Instance.hpp"
#include "Device.hpp"
#include "SwapChain.hpp"
#include "GraphicsPipeline.hpp"
#include "CommandBuffer.hpp"
#include "Buffer.hpp"

namespace ktw {
	class Renderer {
	public:
		Renderer(GLFWwindow* window, uint32_t width, uint32_t height);

		ktw::GraphicsPipeline* createGraphicsPipeline(std::string vertexShader, std::string fragmentShader, uint32_t vertexSize, std::vector<ktw::AttributeDescription>& attributeDescriptions);
		ktw::CommandBuffer* createCommandBuffer(ktw::GraphicsPipeline* pipeline, ktw::Buffer* vertexBuffer);
		ktw::Buffer* createBuffer(uint32_t itemSize, size_t count, void* data);
		void waitDeviceIdle();
		void startFrame();
		void endFrame();
		void post(ktw::CommandBuffer* commandBuffer);

	private:
		ktw::Instance instance;
		vk::UniqueSurfaceKHR surface;
		ktw::Device device;
		ktw::SwapChain swapChain;
		std::vector<ktw::CommandBuffer*> postedCommandBuffers;

		static std::vector<const char*> getGlfwRequiredInstanceExtensions();
		static vk::SurfaceKHR getSurfaceFromGlfw(GLFWwindow* window, vk::Instance& instance);
	};
}