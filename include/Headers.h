// all the headers including Vulkan headers are included here.s

#pragma once

/*********** COMPILER SPECIFIC PREPROCESSORS ***********/
#ifdef _WIN32
#pragma comment(linker, "/subsystem:console")
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define APP_NAME_STR_LEN 80
#define _CRT_SECURE_NO_WARNINGS
#else // _WIN32
#define VK_USE_PLATFORM_XCB_KHR
#include <unistd.h>
#endif // _WIN32

#include <iostream>
#include <vector>
#include <cassert>

// Header files for signleton
#include <memory>
#include <mutex>


#include <vulkan/vulkan.h> // The very first thing. It contains commonly used
						   // Vulkan APIs and structure declarations.
