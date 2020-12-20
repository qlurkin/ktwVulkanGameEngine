cmake_minimum_required (VERSION 3.17.3)

#########################################
# GLFW
#########################################

add_subdirectory(vendor/glfw deps/glfw)


#########################################
# GLSLANG
#########################################

add_subdirectory(vendor/glslang deps/glslang)


#########################################
# KTWVULKANGAMEENGINE
#########################################

project(ktwVulkanGameEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED FATAL_ERROR)

add_executable(${PROJECT_NAME})
add_dependencies(${PROJECT_NAME} glfw)
add_dependencies(${PROJECT_NAME} SPIRV)
add_dependencies(${PROJECT_NAME} glslang)
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} SPIRV)
target_link_libraries(${PROJECT_NAME} glslang)

target_precompile_headers(${PROJECT_NAME} PUBLIC src/ktwVulkanGameEngine/pch.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC
	src
	vendor/spdlog/include
	vendor/glm
	vendor/glslang
	${GLFW_SOURCE_DIR}/include
	${Vulkan_INCLUDE_DIRS}
)

target_sources(${PROJECT_NAME} PUBLIC
	src/ktwVulkanGameEngine/Application.cpp
	src/ktwVulkanGameEngine/Buffer.cpp
	src/ktwVulkanGameEngine/GraphicsPipeline.cpp
	src/ktwVulkanGameEngine/Instance.cpp
	src/ktwVulkanGameEngine/Log.cpp
	src/ktwVulkanGameEngine/Renderer.cpp
	src/ktwVulkanGameEngine/Shader.cpp
	src/ktwVulkanGameEngine/SwapChain.cpp
	src/ktwVulkanGameEngine/UniformBuffer.cpp
	src/ktwVulkanGameEngine/DescriptorSet.cpp
	src/ktwVulkanGameEngine/CommandPool.cpp
	src/ktwVulkanGameEngine/FrameBuffer.cpp
	src/ktwVulkanGameEngine/Context.cpp
	src/ktwVulkanGameEngine/DescriptorPool.cpp
	src/ktwVulkanGameEngine/CommandBuffer.cpp
	src/main.cpp
)