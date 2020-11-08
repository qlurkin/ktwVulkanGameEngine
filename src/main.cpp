#include <iostream>

#include <ktwVulkanGameEngine/ktwVulkanGameEngine.hpp>

class HelloTriangleApplication : public ktw::Application {
	public:
	HelloTriangleApplication(uint32_t width, uint32_t height) : ktw::Application(width, height) {

	}

	private:
	void userSetup() {

	}

	void userUpdate() {

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