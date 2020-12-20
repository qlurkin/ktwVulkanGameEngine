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
	ktw::Buffer* vertexBuffer;
	ktw::Buffer* indexBuffer;

	// std::vector<Vertex> vertices = {
	// 	{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
	// 	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	// 	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	// };

	// std::vector<Vertex> vertices = {
	// 	{{0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
	// 	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	// 	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	// 	{{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}}
	// };

	//std::vector<uint32_t> indices = {0, 2, 1, 1, 2, 3};

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	void userSetup(ktw::Renderer& renderer) override {

		vertices.push_back({{0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}});
		vertices.push_back({{0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}});
		for(int i=0; i<=360; i++) {
			float angle = i*3.141593f/180;
			vertices.push_back({{0.5f*glm::cos(angle), 0.5f*glm::sin(angle)}, {1.0f, 0.0f, 0.0f}});
			indices.push_back(0);
			indices.push_back(i+1);
			indices.push_back(i+2);
		}

		graphicsPipeline = renderer.createGraphicsPipeline(
			getSwapchain(),
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
	}

	void userUpdate(ktw::Renderer& renderer) override {
		renderer.startCommandBuffer()
			.bindPipeline(graphicsPipeline)
			.bindVertexBuffer(vertexBuffer)
			.bindIndexBuffer(indexBuffer)
			.drawIndexed(indexBuffer->getCount())
			.end();
	}

	void userCleanup(ktw::Renderer& renderer) override {
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