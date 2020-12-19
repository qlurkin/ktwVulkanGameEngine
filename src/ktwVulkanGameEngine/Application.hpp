#pragma once

#include "Log.hpp"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <optional>

#include "Renderer.hpp"
#include "Instance.hpp"
#include "Context.hpp"
#include "SwapChain.hpp"

namespace ktw {
	class Application {
	public:
		Application(uint32_t width, uint32_t height);
		~Application();
		void run();
		ktw::SwapChain* getSwapchain();

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
		std::unique_ptr<ktw::Instance> instance;
		std::unique_ptr<ktw::Context> context;
		std::unique_ptr<ktw::SwapChain> swapChain;
		std::unique_ptr<ktw::Renderer> renderer;
	};
}

//TODO: Check if recording commandbuffer/rendering Frame, Timing, Window(RenderTarget, SwapChain), Interfaces from Vulkan and from GLFW