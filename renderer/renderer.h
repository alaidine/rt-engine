#pragma once

#pragma comment(linker, "/subsystem:windows")
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <ShellScalingApi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <array>
#include <unordered_map>
#include <numeric>
#include <ctime>
#include <iostream>
#include <chrono>
#include <random>
#include <algorithm>
#include <sys/stat.h>

#define GLM_FORCE_RADIANS
#define GLM_FORM_DEPTH_ZERO_TO_ONE
#define GML_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

#include "CommandLineParser.hpp"
#include "keycodes.hpp"
#include "VulkanTools.h"
#include "VulkanDebug.h"
#include "VulkanUIOverlay.h"
#include "VulkanSwapChain.h"
#include "VulkanBuffer.h"
#include "VulkanDevice.h"
#include "VulkanTexture.h"

#include "VulkanInitializers.hpp"
#include "camera.hpp"
#include "benchmark.hpp"

constexpr uint32_t maxConcurrentFrames{ 2 };

namespace rt {

	typedef struct Rectangle {
		float x;      // top-left corner position x
		float y;      // top-left corner position y
		float width;  // width
		float height; // height
	} Rectangle;

	typedef struct Color {
		float r;
		float g;
		float b;
		float a;
	} Color;

	typedef struct Position {
		float x;
		float y;
	} Position;

	typedef struct TextureVertex {
		float pos[3];
		float uv[2];
		float normal[3];
	} TextureVertex;

	// Contains all Vulkan objects that are required to store and use a texture
	typedef struct Texture {
		VkSampler sampler{ VK_NULL_HANDLE };
		VkImage image{ VK_NULL_HANDLE };
		VkImageLayout imageLayout;
		VkDeviceMemory deviceMemory{ VK_NULL_HANDLE };
		VkImageView view{ VK_NULL_HANDLE };
		uint32_t width{ 0 };
		uint32_t height{ 0 };
		uint32_t mipLevels{ 0 };
	} Texture;

	typedef struct UniformData {
		glm::mat4 projection;
		glm::mat4 modelView;
		glm::vec4 viewPos;
		// This is used to change the bias for the level-of-detail (mips) in the fragment shader
		float lodBias = 0.0f;
	} UniformData;

	class Renderer {
	public:
		Renderer();
		~Renderer();

		Texture texture;
		UniformData uniformData;
		std::array<vks::Buffer, maxConcurrentFrames> uniformBuffers{};

		vks::Buffer vertexBuffer;
		vks::Buffer indexBuffer;
		uint32_t indexCount{ 0 };

		VkPipeline pipeline{ VK_NULL_HANDLE };
		VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
		VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
		std::array<VkDescriptorSet, maxConcurrentFrames> descriptorSets{};

		// Create buffers and upload data to the GPU
		struct StagingBuffers {
			vks::Buffer vertices;
			vks::Buffer indices;
		} stagingBuffers;

		std::vector<TextureVertex> textureVertices;
		std::vector<uint32_t> textureIndices;

		bool prepared = false;
		bool resized = false;
		uint32_t width = 1280;
		uint32_t height= 720;

		vks::UIOverlay ui;
		CommandLineParser commandLineParser;

		float frameTimer = 1.0;

		vks::Benchmark benchmark;
		vks::VulkanDevice* vulkanDevice{};

		struct Settings {
			bool validation = false;
			bool fullscreen = false;
			bool vsync = false;
			bool overlay = true;
		} settings;

		struct {
			glm::vec2 axisLeft = glm::vec2(0.0f);
			glm::vec2 axisRight = glm::vec2(0.0f);
		} gamePadState;

		struct {
			struct {
				bool left = false;
				bool right = false;
				bool middle = false;
			} buttons;
			glm::vec2 position;
		} mouseState;

		VkClearColorValue defaultClearColor = { { 0.025f, 0.025f, 0.025f, 1.0f } };

		static std::vector<const char*> args;

		float timer = 0.0f;
		float timerSpeed = 0.25f;
		bool paused = false;

		Camera camera;

		std::string title = "Vulkan Application";
		std::string name = "vulkanapplication";
		uint32_t apiVersion = VK_API_VERSION_1_0;

		struct {
			VkImage image;
			VkDeviceMemory memory;
			VkImageView view;
		} depthStencil{};

		HWND window;
		HINSTANCE windowInstance;

		std::chrono::steady_clock::time_point tStart;
		std::chrono::steady_clock::time_point tEnd;
		double tDiff;

		bool initVulkan();

		void setupConsole(std::string title);
		void setupDPIAwareness();
		HWND setupWindow(HINSTANCE hinstance, WNDPROC wndproc);
		void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		VkResult createInstance();
		void render();
		void keyPressed(uint32_t);
		void mouseMoved(double x, double y, bool& handled);
		void windowResized();
		void setupDepthStencil();
		void setupFrameBuffer();
		void setupRenderPass();
		void getEnabledFeatures();
		void getEnabledExtensions();
		void prepare();

		VkPipelineShaderStageCreateInfo loadShader(std::string fileName, VkShaderStageFlagBits stage);

		void renderLoop();
		void drawUI(const VkCommandBuffer commandBuffer);

		void init();
		void windowResize();
		void prepareFrame(bool waitForFence = true);
		void submitFrame(bool skipQueueSubmit = false);
		
		void OnUpdateUIOverlay(vks::UIOverlay* overlay);
		void OnHandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void DrawTextureRec(Texture texture, Rectangle rectangle, glm::vec2 position, Color tint);
		void BeginDrawing();
		void EndDrawing();

		void setupDescriptors();
		void preparePipelines();
		void loadTexture();
		void prepareUniformBuffers();
		void updateUniformBuffers();
		void buildCommandBuffer();

	private:
		void destroyTextureImage(Texture texture);

		std::string getWindowTitle() const;
		uint32_t destWidth{};
		uint32_t destHeight{};
		bool resizing = false;
		void handleMouseMove(int32_t x, int32_t y);
		void nextFrame();
		void updateOverlay();
		void createPipelineCache();
		void createCommandPool();
		void createSynchronizationPrimitives();
		void createSurface();
		void createSwapChain();
		void createCommandBuffers();
		void destroyCommandBuffers();
		std::string shaderDir = "glsl";
		std::string getShadersPath() const;

		uint32_t frameCounter = 0;
		uint32_t lastFPS = 0;
		std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp, tPrevEnd;
		VkInstance instance{ VK_NULL_HANDLE };
		std::vector<std::string> supportedInstanceExtensions;
		VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
		VkPhysicalDeviceProperties deviceProperties{};
		VkPhysicalDeviceFeatures deviceFeatures{};
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties{};
		VkPhysicalDeviceFeatures enabledFeatures{};
		std::vector<const char*> enabledDeviceExtensions;
		std::vector<const char*> enabledInstanceExtensions;
		std::vector<VkLayerSettingEXT> enabledLayerSettings;
		void* deviceCreatepNextChain = nullptr;
		VkDevice device{ VK_NULL_HANDLE };
		VkQueue queue{ VK_NULL_HANDLE };
		VkFormat depthFormat{ VK_FORMAT_UNDEFINED };
		VkCommandPool cmdPool{ VK_NULL_HANDLE };
		std::array<VkCommandBuffer, maxConcurrentFrames> drawCmdBuffers;
		VkRenderPass renderPass{ VK_NULL_HANDLE };
		std::vector<VkFramebuffer>frameBuffers;
		VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };
		std::vector<VkShaderModule> shaderModules;
		VkPipelineCache pipelineCache{ VK_NULL_HANDLE };
		VulkanSwapChain swapChain;

		uint32_t currentImageIndex{ 0 };
		uint32_t currentBuffer{ 0 };
		std::array<VkSemaphore, maxConcurrentFrames> presentCompleteSemaphores{};
		std::vector<VkSemaphore> renderCompleteSemaphores{};
		std::array<VkFence, maxConcurrentFrames> waitFences;

		bool requiresStencil{ false };
	};

	class Application {
	public:
		Application() {}
		~Application() {}
	private:
		Renderer m_renderer;
	};

} // namespace rt
