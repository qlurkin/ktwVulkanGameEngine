#include <iostream>

#include <ktwVulkanGameEngine/ktwVulkanGameEngine.hpp>

class HelloTriangleApplication : public ktw::Application {
public:
	HelloTriangleApplication(uint32_t width, uint32_t height) : ktw::Application(width, height) {}

private:
	ktw::GraphicsPipeline* graphicsPipeline;
	ktw::CommandBuffer* commandBuffer;

	void userSetup(ktw::Renderer& renderer) override {
		graphicsPipeline = renderer.createGraphicsPipeline("shaders\\vert.spv", "shaders\\frag.spv");
		commandBuffer = renderer.createCommandBuffer(graphicsPipeline);
	}

	void userUpdate(ktw::Renderer& renderer) override {
		renderer.post(commandBuffer);
	}

	void userCleanup(ktw::Renderer& renderer) override {
		delete commandBuffer;
		delete graphicsPipeline;
	}
};

int main() {
	HelloTriangleApplication app(640, 480);

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}