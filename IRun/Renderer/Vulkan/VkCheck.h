#pragma once

#include <vulkan/vulkan.hpp>
#include <ILog.h>

#ifdef _DEBUG
	#define VK_CHECK(fun, msg)																									\
			vk::Result err = fun;																								\
			if (err != vk::Result::eSuccess) I_LOG_FATAL_ERROR(msg);															\
			vk::resultCheck(err, msg);																							
#else
	#define VK_CHECK(err, msg)
#endif