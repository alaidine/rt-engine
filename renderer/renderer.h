#pragma once

#pragma comment(linker, "/subsystem:windows")
#include <ShellScalingApi.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>

#include <algorithm>
#include <array>
#include <assert.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <numeric>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORM_DEPTH_ZERO_TO_ONE
#define GML_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

#include "CommandLineParser.hpp"
#include "VulkanBuffer.h"
#include "VulkanDebug.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanTexture.h"
#include "VulkanTools.h"
#include "VulkanUIOverlay.h"
#include "keycodes.hpp"

#include "VulkanInitializers.hpp"
#include "benchmark.hpp"
#include "camera.hpp"

namespace rt {

typedef struct Rectangle {
    float x;      // top-left corner position x
    float y;      // top-left corner position y
    float width;  // width
    float height; // height
} Rectangle;

typedef struct Position {
    float x;
    float y;
} Position;

// Contains all Vulkan objects that are required to store and use a texture
typedef struct Texture {
    VkSampler sampler{VK_NULL_HANDLE};
    VkImage image{VK_NULL_HANDLE};
    VkImageLayout imageLayout;
    VkDeviceMemory deviceMemory{VK_NULL_HANDLE};
    VkImageView view{VK_NULL_HANDLE};
    uint32_t width{0};
    uint32_t height{0};
    uint32_t mipLevels{0};
} Texture;

typedef struct UniformData {
    glm::mat4 projection;
    glm::mat4 modelView;
    glm::vec4 viewPos;
    // This is used to change the bias for the level-of-detail (mips) in the fragment shader
    float lodBias = 0.0f;
} UniformData;

constexpr uint32_t maxConcurrentFrames{2};

class VulkanBase {
  private:
    std::string getWindowTitle() const;
    bool resizing = false;
    void handleMouseMove(int32_t x, int32_t y);
    void updateOverlay();
    void createPipelineCache();
    void createCommandPool();
    void createSynchronizationPrimitives();
    void createSurface();
    void createSwapChain();
    void createCommandBuffers();
    void destroyCommandBuffers();
    std::string shaderDir = "glsl";

  protected:
    uint32_t destWidth{};
    uint32_t destHeight{};
    void nextFrame();

    // Returns the path to the root of the glsl, hlsl or slang shader directory.
    std::string getShadersPath() const;

    // Frame counter to display fps
    uint32_t frameCounter = 0;
    uint32_t lastFPS = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp, tPrevEnd;
    // Vulkan instance, stores all per-application states
    VkInstance instance{VK_NULL_HANDLE};
    std::vector<std::string> supportedInstanceExtensions;
    // Physical device (GPU) that Vulkan will use
    VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
    // Stores physical device properties (for e.g. checking device limits)
    VkPhysicalDeviceProperties deviceProperties{};
    // Stores the features available on the selected physical device (for e.g.
    // checking if a feature is available)
    VkPhysicalDeviceFeatures deviceFeatures{};
    // Stores all available memory (type) properties for the physical device
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties{};
    /** @brief Set of physical device features to be enabled for this example
     * (must be set in the derived constructor) */
    VkPhysicalDeviceFeatures enabledFeatures{};
    /** @brief Set of device extensions to be enabled for this example (must be
     * set in the derived constructor) */
    std::vector<const char *> enabledDeviceExtensions;
    /** @brief Set of instance extensions to be enabled for this example (must be
     * set in the derived constructor) */
    std::vector<const char *> enabledInstanceExtensions;
    /** @brief Set of layer settings to be enabled for this example (must be set
     * in the derived constructor) */
    std::vector<VkLayerSettingEXT> enabledLayerSettings;
    /** @brief Optional pNext structure for passing extension structures to device
     * creation */
    void *deviceCreatepNextChain = nullptr;
    /** @brief Logical device, application's view of the physical device (GPU) */
    VkDevice device{VK_NULL_HANDLE};
    // Handle to the device graphics queue that command buffers are submitted to
    VkQueue queue{VK_NULL_HANDLE};
    // Depth buffer format (selected during Vulkan initialization)
    VkFormat depthFormat{VK_FORMAT_UNDEFINED};
    // Command buffer pool
    VkCommandPool cmdPool{VK_NULL_HANDLE};
    // Command buffers used for rendering
    std::array<VkCommandBuffer, maxConcurrentFrames> drawCmdBuffers;
    // Global render pass for frame buffer writes
    VkRenderPass renderPass{VK_NULL_HANDLE};
    // List of available frame buffers (same as number of swap chain images)
    std::vector<VkFramebuffer> frameBuffers;
    // Descriptor set pool
    VkDescriptorPool descriptorPool{VK_NULL_HANDLE};
    // List of shader modules created (stored for cleanup)
    std::vector<VkShaderModule> shaderModules;
    // Pipeline cache object
    VkPipelineCache pipelineCache{VK_NULL_HANDLE};
    // Wraps the swap chain to present images (framebuffers) to the windowing
    // system
    VulkanSwapChain swapChain;

    // Synchronization related objects and variables
    // These are used to have multiple frame buffers "in flight" to get some
    // CPU/GPU parallelism
    uint32_t currentImageIndex{0};
    uint32_t currentBuffer{0};
    std::array<VkSemaphore, maxConcurrentFrames> presentCompleteSemaphores{};
    std::vector<VkSemaphore> renderCompleteSemaphores{};
    std::array<VkFence, maxConcurrentFrames> waitFences;

    bool requiresStencil{false};

  public:
    bool prepared = false;
    bool resized = false;
    uint32_t width = 1280;
    uint32_t height = 720;

    vks::UIOverlay ui;
    CommandLineParser commandLineParser;

    /** @brief Last frame time measured using a high performance timer (if
     * available) */
    float frameTimer = 1.0f;

    vks::Benchmark benchmark;

    /** @brief Encapsulated physical and logical vulkan device */
    vks::VulkanDevice *vulkanDevice{};

    /** @brief Example settings that can be changed e.g. by command line arguments
     */
    struct Settings {
        /** @brief Activates validation layers (and message output) when set to true
         */
        bool validation = false;
        /** @brief Set to true if fullscreen mode has been requested via command
         * line */
        bool fullscreen = false;
        /** @brief Set to true if v-sync will be forced for the swapchain */
        bool vsync = false;
        /** @brief Enable UI overlay */
        bool overlay = true;
    } settings;

    /** @brief State of gamepad input (only used on Android) */
    struct {
        glm::vec2 axisLeft = glm::vec2(0.0f);
        glm::vec2 axisRight = glm::vec2(0.0f);
    } gamePadState;

    /** @brief State of mouse/touch input */
    struct {
        struct {
            bool left = false;
            bool right = false;
            bool middle = false;
        } buttons;
        glm::vec2 position;
    } mouseState;

    VkClearColorValue defaultClearColor = {{0.025f, 0.025f, 0.025f, 1.0f}};

    static std::vector<const char *> args;

    // Defines a frame rate independent timer value clamped from -1.0...1.0
    // For use in animations, rotations, etc.
    float timer = 0.0f;
    // Multiplier for speeding up (or slowing down) the global timer
    float timerSpeed = 0.25f;
    bool paused = false;

    Camera camera;

    std::string title = "Vulkan Example";
    std::string name = "vulkanExample";
    uint32_t apiVersion = VK_API_VERSION_1_0;

    /** @brief Default depth stencil attachment used by the default render pass */
    struct {
        VkImage image;
        VkDeviceMemory memory;
        VkImageView view;
    } depthStencil{};

    HWND window;
    HINSTANCE windowInstance;

    /** @brief Default base class constructor */
    VulkanBase();
    virtual ~VulkanBase();
    /** @brief Setup the vulkan instance, enable required extensions and connect
     * to the physical device (GPU) */
    bool initVulkan();

    void setupConsole(std::string title);
    void setupDPIAwareness();
    HWND setupWindow(HINSTANCE hinstance, WNDPROC wndproc);
    void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /** @brief (Virtual) Creates the application wide Vulkan instance */
    virtual VkResult createInstance();
    /** @brief (Pure virtual) Render function to be implemented by the sample
     * application */
    virtual void render() = 0;
    /** @brief (Virtual) Called after a key was pressed, can be used to do custom
     * key handling */
    virtual void keyPressed(uint32_t);
    /** @brief (Virtual) Called after the mouse cursor moved and before internal
     * events (like camera rotation) is handled */
    virtual void mouseMoved(double x, double y, bool &handled);
    /** @brief (Virtual) Called when the window has been resized, can be used by
     * the sample application to recreate resources */
    virtual void windowResized();
    /** @brief (Virtual) Setup default depth and stencil views */
    virtual void setupDepthStencil();
    /** @brief (Virtual) Setup default framebuffers for all requested swapchain
     * images */
    virtual void setupFrameBuffer();
    /** @brief (Virtual) Setup a default renderpass */
    virtual void setupRenderPass();
    /** @brief (Virtual) Called after the physical device features have been read,
     * can be used to set features to enable on the device */
    virtual void getEnabledFeatures();
    /** @brief (Virtual) Called after the physical device extensions have been
     * read, can be used to enable extensions based on the supported extension
     * listing*/
    virtual void getEnabledExtensions();

    /** @brief Prepares all Vulkan resources and functions required to run the
     * sample */
    virtual void prepare();

    /** @brief Loads a SPIR-V shader file for the given shader stage */
    VkPipelineShaderStageCreateInfo loadShader(std::string fileName, VkShaderStageFlagBits stage);

    void windowResize();

    /** @brief Entry point for the main render loop */
    void renderLoop();

    /** @brief Adds the drawing commands for the ImGui overlay to the given
     * command buffer */
    void drawUI(const VkCommandBuffer commandBuffer);

    /** Prepare the next frame for workload submission by acquiring the next swap
     * chain image and waiting for the previous command buffer to finish */
    void prepareFrame(bool waitForFence = true);
    /** @brief Presents the current image to the swap chain */
    void submitFrame(bool skipQueueSubmit = false);

    /** @brief (Virtual) Called when the UI overlay is updating, can be used to
     * add custom elements to the overlay */
    virtual void OnUpdateUIOverlay(vks::UIOverlay *overlay);

    virtual void OnHandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

// Vertex layout
struct Vertex {
    float pos[3];
    float uv[2];
    float normal[3];
};

class VulkanRenderer : public VulkanBase {
  public:
    // Contains all Vulkan objects that are required to store and use a texture
    struct Texture {
        VkSampler sampler{VK_NULL_HANDLE};
        VkImage image{VK_NULL_HANDLE};
        VkImageLayout imageLayout;
        VkDeviceMemory deviceMemory{VK_NULL_HANDLE};
        VkImageView view{VK_NULL_HANDLE};
        uint32_t width{0};
        uint32_t height{0};
        uint32_t mipLevels{0};
    } texture;

    vks::Buffer vertexBuffer;
    vks::Buffer indexBuffer;
    uint32_t indexCount{0};

    struct UniformData {
        glm::mat4 projection;
        glm::mat4 modelView;
        glm::vec4 viewPos;
        // This is used to change the bias for the level-of-detail (mips) in the
        // fragment shader
        float lodBias = 0.0f;
    } uniformData;
    std::array<vks::Buffer, maxConcurrentFrames> uniformBuffers;

    VkPipeline pipeline{VK_NULL_HANDLE};
    VkPipelineLayout pipelineLayout{VK_NULL_HANDLE};
    VkDescriptorSetLayout descriptorSetLayout{VK_NULL_HANDLE};
    std::array<VkDescriptorSet, maxConcurrentFrames> descriptorSets{};

    std::vector<Vertex> textureVertices;
    std::vector<uint32_t> textureIndices;

    VulkanRenderer();
    ~VulkanRenderer();

    // Enable physical device features required for this example
    virtual void getEnabledFeatures();
    /*
            Upload texture image data to the GPU

            Vulkan offers two types of image tiling (memory layout):

            Linear tiled images:
                    These are stored as is and can be copied directly to. But due
       to the linear nature they're not a good match for GPUs and format and
       feature support is very limited. It's not advised to use linear tiled
       images for anything else than copying from host to GPU if buffer copies are
       not an option. Linear tiling is thus only implemented for learning
       purposes, one should always prefer optimal tiled image.

            Optimal tiled images:
                    These are stored in an implementation specific layout matching
       the capability of the hardware. They usually support more formats and
       features and are much faster. Optimal tiled images are stored on the device
       and not accessible by the host. So they can't be written directly to (like
       liner tiled images) and always require some sort of data copy, either from
       a buffer or	a linear tiled image.

            In Short: Always use optimal tiled images for rendering.
    */
    void loadTexture();
    // Free all Vulkan resources used by a texture object
    void destroyTextureImage(Texture texture);
    // Creates a vertex and index buffer for a quad made of two triangles
    // This is used to display the texture on
    void generateQuad();
    void setupDescriptors();
    void preparePipelines();
    // Prepare and initialize uniform buffer containing shader uniforms
    void prepareUniformBuffers();
    void updateUniformBuffers();
    void prepare();
    void buildCommandBuffer();
    virtual void render();
    virtual void OnUpdateUIOverlay(vks::UIOverlay *overlay);

    void Init();
    void StartDrawing();
    void EndDrawing();
    void DrawTextureRec(Rectangle rectangle, glm::vec2 position);
};

} // namespace rt
