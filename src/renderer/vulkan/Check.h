#pragma once

#include <vulkan\vulkan.h>
#include <vulkan\vk_enum_string_helper.h>
#include <ILog.h>


#ifdef DEBUG 
/// <summery>
/// Checks a function that returns VkResult and see's if it returns VkResult::VK_SUCCESS, if not log and error and exit application.
/// Only works in debug mode. This macro is empty in release mode.
/// </summery>
#define VK_CHECK(fun, s)																											\
				{																													\
					VkResult result = fun;																							\
					if (result != VK_SUCCESS) {																						\
						I_LOG_FATAL_ERROR("Vulkan function failed! Message:%s\nError: %s", s, string_VkResult(result));				\
						exit(EXIT_FAILURE);																							\
					}																												\
				}		

#else
#define VK_CHECK(fun, s) fun;
#endif