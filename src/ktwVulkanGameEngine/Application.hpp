#pragma once

#include "Log.hpp"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <optional>

#include "Renderer.hpp"

namespace ktw {
	class Application {
	public:
		Application(uint32_t width, uint32_t height);
		~Application();
		void run();

	private:
		virtual void userSetup(ktw::Renderer& renderer) = 0;
		virtual void userUpdate(ktw::Renderer& renderer) = 0;
		virtual void userCleanup(ktw::Renderer& renderer) = 0;

		void initWindow();
		void initVulkan();
		void mainLoop();
		void cleanup();

		uint32_t width;
		uint32_t height;
		GLFWwindow* window;
		std::unique_ptr<Renderer> renderer;
	};
} // namespace ktw