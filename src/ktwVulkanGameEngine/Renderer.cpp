#include "pch.hpp"
#include "Renderer.hpp"

namespace ktw {
	Renderer::Renderer(GLFWwindow* window, uint32_t width, uint32_t height) :
		instance(getGlfwRequiredInstanceExtensions()),
		surface(vk::UniqueSurfaceKHR(getSurfaceFromGlfw(window, instance.getInstance()), instance.getInstance())),
		device(instance, *surface),
		swapChain(device, *surface, width, height)
	{
		/*instance = new ktw::Instance(getGlfwRequiredInstanceExtensions());
		surface = new vk::UniqueSurfaceKHR(getSurfaceFromGlfw(window, instance->getInstance()), instance->getInstance());
		device = new ktw::Device(*instance, **surface);
		swapChain = new ktw::SwapChain(*device, **surface, width, height);*/
		LOG_TRACE("Renderer Created");
	}

	std::vector<const char*> Renderer::getGlfwRequiredInstanceExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector<const char*> extensionNames(glfwExtensions, glfwExtensions + glfwExtensionCount);

		return extensionNames;
	}

	vk::SurfaceKHR Renderer::getSurfaceFromGlfw(GLFWwindow* window, vk::Instance& instance) {
		VkSurfaceKHR surface;
		if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}

		return surface;
	}
	
	ktw::GraphicsPipeline* Renderer::createGraphicsPipeline(std::string vertexShader, std::string fragmentShader, const std::vector<ktw::VertexBufferBinding>& vertexBufferBindings, const std::vector<ktw::UniformDescriptor>& uniformDescriptors) {
		return new ktw::GraphicsPipeline(device, swapChain, vertexShader, fragmentShader, vertexBufferBindings, uniformDescriptors);
	}

	ktw::CommandBuffer* Renderer::createCommandBuffer(ktw::GraphicsPipeline* pipeline, ktw::Buffer* vertexBuffer, ktw::Buffer* indexBuffer) {
		return new ktw::CommandBuffer(device, swapChain, *pipeline, *vertexBuffer, *indexBuffer);
	}

	ktw::Buffer* Renderer::createBuffer(uint32_t itemSize, size_t count, ktw::BufferUsage usage, void* data) {
		return new ktw::Buffer(device, itemSize, static_cast<uint32_t>(count), usage, data);
	}

	void Renderer::waitDeviceIdle() {
		device.getDevice().waitIdle();
	}

	void Renderer::startFrame() {
		postedCommandBuffers.clear();
		swapChain.acquireImage();
	}

	void Renderer::endFrame() {
		swapChain.submit(postedCommandBuffers);
	}

	void Renderer::post(ktw::CommandBuffer* commandBuffer) {
		postedCommandBuffers.push_back(commandBuffer);
	}

	ktw::Buffer* Renderer::createVertexBuffer(uint32_t itemSize, size_t count, void* data) {
		return createBuffer(itemSize, count, ktw::BufferUsage::eVertexBuffer, data);
	}

	ktw::Buffer* Renderer::createIndexBuffer(size_t count, void* data) {
		return createBuffer(sizeof(uint32_t), count, ktw::BufferUsage::eIndexBuffer, data);
	}

	ktw::UniformBuffer* Renderer::createUniformBuffer(ktw::IRenderTarget* renderTarget, uint32_t size) {
		return new ktw::UniformBuffer(device, *renderTarget, size);
	}

	void Renderer::setDescriptorPoolSize(uint32_t size) {
		swapChain.setDescriptorPoolSize(size);
	}
}