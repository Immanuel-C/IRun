#pragma once

#include <vulkan\vulkan.h>
#include <vulkan\vk_enum_string_helper.h>
#include <ILog.h>

#ifdef DEBUG 
#define VK_CHECK(x, s) VkResult result = x;                                                                 \
					if (result != VK_SUCCESS) {															 \
						I_LOG_FATAL_ERROR("Vulkan function failed! Message:%s\nError: %s", s, string_VkResult(result)); \
					}																					 
#else
#define VK_CHECK(x, s) x; s;
#endif