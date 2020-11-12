#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <optional>

#include "Renderer.hpp"

namespace ktw {
	#ifdef NDEBUG
		const bool enableValidationLayers = false;
	#else
		const bool enableValidationLayers = true;
	#endif

	class Application {
	public:
		Application(uint32_t width, uint32_t height);
		~Application();
		void run();
		ktw::Context& getContext();

	private:
		virtual void userSetup(ktw::Renderer& renderer) = 0;
		virtual void userUpdate(ktw::Renderer& renderer) = 0;
		virtual void userCleanup(ktw::Renderer& renderer) = 0;

		struct QueueFamilyIndices {
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool isComplete() {
				return graphicsFamily.has_value();
			}
		};

		struct SwapChainSupportDetails {
			vk::SurfaceCapabilitiesKHR capabilities;
			std::vector<vk::SurfaceFormatKHR> formats;
			std::vector<vk::PresentModeKHR> presentModes;
		};

		void initWindow();
		void initVulkan();
		void mainLoop();
		void prepareFrame();
		void drawFrame();
		void cleanup();
		void createSurface();
		void createInstance(std::vector<const char *> extensionNames);
		void setupDebugMessenger();
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
		bool checkDeviceExtensionSupport(vk::PhysicalDevice device, const std::vector<const char*> deviceExtensions);
		SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device);
		int rateDeviceSuitability(vk::PhysicalDevice device);  // 0 means not suitable
		void pickPhysicalDevice();
		std::vector<uint32_t> findGraphicsQueueIndexes(vk::PhysicalDevice device);
		std::vector<uint32_t> findPresentQueueIndexes(vk::PhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
		void createLogicalDevice();
		vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
		vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
		vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
		void createSwapChain();
		void createImageViews();
		void createRenderPass();
		void createFramebuffers();
		void createCommandPool();
		void createSemaphores();

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		uint32_t width;
		uint32_t height;
		GLFWwindow* window;
		QueueFamilyIndices queueIndices;
		ktw::Context context;
		std::unique_ptr<Renderer> renderer;
		vk::UniqueSemaphore imageAvailableSemaphore;
		vk::UniqueSemaphore renderFinishedSemaphore;
	};
} // namespace ktwq