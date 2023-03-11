#pragma once

#include <vulkan/vulkan.hpp>
#include <ILog.h>

#ifdef DEBUG
	#define VK_CHECK(fun, msg)																									\
			vk::Result err = fun;																								\
			std::string finalMsg = msg + std::string{"\nError Msg: "} + vk::to_string(err);													\
			if (err != vk::Result::eSuccess) I_LOG_FATAL_ERROR(finalMsg.c_str());												\
			vk::resultCheck(err, msg);																							
#else
	#define VK_CHECK(fun, msg) fun;
#endif