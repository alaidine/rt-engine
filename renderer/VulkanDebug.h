#pragma once

#include "vulkan/vulkan.h"

#include <assert.h>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <io.h>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <windows.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vks {
namespace debug {
extern bool logToFile;
extern std::string logFileName;

// Default debug callback
VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                         VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                         const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                         void *pUserData);

// Load debug function pointers and set debug callback
void setupDebugging(VkInstance instance);
// Clear debug callback
void freeDebugCallback(VkInstance instance);
// Used to populate a VkDebugUtilsMessengerCreateInfoEXT with our example
// messenger function and desired flags
void setupDebugingMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &debugUtilsMessengerCI);
void log(std::string message);
} // namespace debug

// Wrapper for the VK_EXT_debug_utils extension
// These can be used to name Vulkan objects for debugging tools like RenderDoc
namespace debugutils {
void setup(VkInstance instance);
void cmdBeginLabel(VkCommandBuffer cmdbuffer, std::string caption, glm::vec4 color);
void cmdEndLabel(VkCommandBuffer cmdbuffer);
} // namespace debugutils
} // namespace vks
