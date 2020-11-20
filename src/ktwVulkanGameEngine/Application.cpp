#include "pch.hpp"
#include "Application.hpp"

namespace ktw {
	Application::Application(uint32_t width, uint32_t height) {
		this->width = width;
		this->height = height;
		LOG_TRACE("Application Created");
	}

	Application::~Application() {}

	void Application::run() {
		initWindow();
		initVulkan();
		userSetup(*renderer);
		mainLoop();
		userCleanup(*renderer);
		cleanup();
	}

	void Application::initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		
		window = glfwCreateWindow(width, height, "ktwVulkanGameEngine", nullptr, nullptr);
	}

	void Application::initVulkan() {
		renderer = std::make_unique<ktw::Renderer>(window, width, height);
	}

	void Application::mainLoop() {
		auto start = std::chrono::steady_clock::now();
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double, std::milli> frameTime = end - start;
			if(frameTime.count() > 1000.0/60) {
				std::stringstream ss;
				ss << "ktwVulkanGameEngine" << " [" << 1000.0/frameTime.count() << " FPS]";
				glfwSetWindowTitle(window, ss.str().c_str());
				start = end;
				renderer->startFrame();
				userUpdate(*renderer);
				renderer->endFrame();
			}
			
		}

		renderer->waitDeviceIdle();
	}

	void Application::cleanup() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}