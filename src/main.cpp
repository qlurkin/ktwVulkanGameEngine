#include <iostream>

#include <ktwVulkanGameEngine/ktwVulkanGameEngine.hpp>

class HelloTriangleApplication : public ktw::Application {
public:
	HelloTriangleApplication(uint32_t width, uint32_t height) : ktw::Application(width, height) {

	}

private:
	ktw::GraphicsPipeline* graphicsPipeline;

	void userSetup(ktw::Renderer& renderer) override {
		graphicsPipeline = renderer.createGraphicsPipeline("shaders\\vert.spv", "shaders\\frag.spv");
	}

	void userUpdate(ktw::Renderer& renderer) override {

	}

	void userCleanup(ktw::Renderer& renderer) override {
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