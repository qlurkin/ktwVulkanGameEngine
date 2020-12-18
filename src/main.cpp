#include <iostream>

#include <ktwVulkanGameEngine/ktwVulkanGameEngine.hpp>

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class HelloTriangleApplication : public ktw::Application {
public:
	HelloTriangleApplication(uint32_t width, uint32_t height) : ktw::Application(width, height) {}

private:
	ktw::GraphicsPipeline* graphicsPipeline;
	//ktw::CommandBuffer* commandBuffer;
	ktw::Buffer* vertexBuffer;
	ktw::Buffer* indexBuffer;

	std::vector<Vertex> vertices = {
		{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	std::vector<uint32_t> indices = {0, 1, 2};

	void userSetup(ktw::Renderer& renderer) override {
		graphicsPipeline = renderer.createGraphicsPipeline(
			getSwapchain().getWidth(),
			getSwapchain().getHeight(),
			getSwapchain().getRenderPass(),
			"shaders\\shader.vert",
			"shaders\\shader.frag",
			{{
				0,
				sizeof(Vertex),
				{
					{0, ktw::Format::eFloat2, offsetof(Vertex, pos)},
					{1, ktw::Format::eFloat3, offsetof(Vertex, color)}
				}
			}},
			{
				// {
				// 	0,
				// 	ktw::ShaderStage::eVertex
				// }
			}
		);
		vertexBuffer = renderer.createVertexBuffer(sizeof(Vertex), vertices.size(), vertices.data());
		indexBuffer = renderer.createIndexBuffer(indices.size(), indices.data());
		//commandBuffer = renderer.createCommandBuffer(graphicsPipeline, vertexBuffer, indexBuffer);
	}

	void userUpdate(ktw::Renderer& renderer) override {
		//renderer.post(commandBuffer);
		renderer.startCommandBuffer();
		//LOG_WARN("Hello1");
		renderer.bindPipeline(graphicsPipeline);
		//LOG_WARN("Hello2");
		renderer.bindVertexBuffer(vertexBuffer);
		//LOG_WARN("Hello3");
		renderer.bindIndexBuffer(indexBuffer);
		//LOG_WARN("Hello4");
		renderer.drawIndexed(indexBuffer->getCount());
		//LOG_WARN("Hello5");
		renderer.endCommandBuffer();
	}

	void userCleanup(ktw::Renderer& renderer) override {
		//delete commandBuffer;
		delete vertexBuffer;
		delete indexBuffer;
		delete graphicsPipeline;
	}
};

int main() {
	HelloTriangleApplication app(800, 600);

	try {
		app.run();
	} catch (const std::exception& e) {
		LOG_ERROR("Unhandeld Exception: {}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}