#include <iostream>

#include <ktwVulkanGameEngine/ktwVulkanGameEngine.hpp>

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
};

class HelloTriangleApplication : public ktw::Application {
public:
	HelloTriangleApplication(uint32_t width, uint32_t height) : ktw::Application(width, height) {}

private:
	ktw::GraphicsPipeline* graphicsPipeline;
	ktw::CommandBuffer* commandBuffer;
	ktw::Buffer* vertexBuffer;

	std::vector<Vertex> vertices = {
		{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	void userSetup(ktw::Renderer& renderer) override {
		std::vector<ktw::AttributeDescription> attributeDescriptions = {
			{0, ktw::Format::eFloat2, offsetof(Vertex, pos)},
			{1, ktw::Format::eFloat3, offsetof(Vertex, color)}
		};
		graphicsPipeline = renderer.createGraphicsPipeline("shaders\\vert.spv", "shaders\\frag.spv", sizeof(Vertex), attributeDescriptions);
		vertexBuffer = renderer.createBuffer(sizeof(Vertex), vertices.size(), vertices.data());
		commandBuffer = renderer.createCommandBuffer(graphicsPipeline, vertexBuffer);
	}

	void userUpdate(ktw::Renderer& renderer) override {
		renderer.post(commandBuffer);
	}

	void userCleanup(ktw::Renderer& renderer) override {
		delete commandBuffer;
		delete vertexBuffer;
		delete graphicsPipeline;
	}
};

int main() {
	HelloTriangleApplication app(800, 600);

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}