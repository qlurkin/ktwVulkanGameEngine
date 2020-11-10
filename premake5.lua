workspace "ktwVulkanGameEngine"
	architecture "x64"

	configurations {
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "vendor/glfw"
--include "vendor/glslang"

project "ktwVulkanGameEngine"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp"
	}

	includedirs {
		"src",
		"vendor/spdlog/include",
		"vendor/glfw/include",
		"vendor/glm",
		"$(VULKAN_SDK)/include"
	}

	libdirs {
		"$(VULKAN_SDK)/Lib"
	}

	links {
		"GLFW",
		"vulkan-1"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		symbols "on"

	filter "configurations:Release"
		optimize "on"