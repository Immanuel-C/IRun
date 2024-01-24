#include "Instance.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		I_DEBUG_LOG_WARNING("%s", pCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		I_DEBUG_LOG_ERROR("%s", pCallbackData->pMessage);
		break;
	default:
		break;
	}

	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto fun = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (!fun)
		return VK_ERROR_EXTENSION_NOT_PRESENT;

	return fun(instance, pCreateInfo, nullptr, pDebugMessenger);
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger) {
	auto fun = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (fun) fun(instance, debugMessenger, nullptr);
}

namespace IRun {
	namespace Vk {
		Instance::Instance(IWindow::Window& window) {
			CreateInstance(window);
			CreateDebugMessenger();
		}

		void Instance::Destroy() {
			I_DEBUG_LOG_TRACE("Destroyed Vulkan instance: 0x%p", m_instance);
			I_DEBUG_LOG_TRACE("Destroyed Vulkan debug utils messenger: 0x%p", m_debugMessenger);
			DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger);
			vkDestroyInstance(m_instance, nullptr);
		}

		void Instance::CreateInstance(IWindow::Window& window) {
			if (m_enableValidationLayers && !CheckValidationLayerSupport()) {
				I_DEBUG_LOG_ERROR("Validation layers are not supported! Program will continue with no validation.");
				m_enableValidationLayers = false;
			}

			VkApplicationInfo appInfo{};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "IRun::Application";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "IRun::Engine";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_3;

			VkInstanceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;

			std::vector<const char*> requiredInstanceExtensions{};
			IWindow::Vk::GetRequiredInstanceExtensions(requiredInstanceExtensions);
			if (m_enableValidationLayers)
				requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			I_ASSERT_FATAL_ERROR(!extensionCount, "No extensions supported! Is Vulkan supported on you machine? Abort!");

			std::vector<VkExtensionProperties> supportedExtensions(extensionCount);

			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedExtensions.data());

			uint32_t supportedCount = 0;
			for (const char* requiredExtension : requiredInstanceExtensions)
				for (const VkExtensionProperties& supportedExtension : supportedExtensions)
					if (strcmp(requiredExtension, supportedExtension.extensionName) == '\0')
						supportedCount++;

			I_ASSERT_FATAL_ERROR(supportedCount != requiredInstanceExtensions.size(), "Not all required extensions supported! Abort!");

			createInfo.enabledExtensionCount = (uint32_t)requiredInstanceExtensions.size();
			createInfo.ppEnabledExtensionNames = requiredInstanceExtensions.data();

			if (m_enableValidationLayers) {
				createInfo.enabledLayerCount = (uint32_t)m_validationLayers.size();
				createInfo.ppEnabledLayerNames = m_validationLayers.data();
			}
			else {
				createInfo.enabledLayerCount = 0;
				createInfo.ppEnabledLayerNames = nullptr;
			}

			VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_instance), "Failed to create instance!");

			I_DEBUG_LOG_TRACE("Created Vulkan instance: 0x%p", m_instance);
		}

		void Instance::CreateDebugMessenger() {
			VkDebugUtilsMessengerCreateInfoEXT createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = debugCallback;
			createInfo.pUserData = nullptr;

			VK_CHECK(CreateDebugUtilsMessengerEXT(m_instance, &createInfo, &m_debugMessenger), "Failed to create debug messenger!");
			I_DEBUG_LOG_TRACE("Created Vulkan debug utils messenger: 0x%p", m_debugMessenger);
		}

		bool Instance::CheckValidationLayerSupport()
		{
			uint32_t layerCount = 0;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			if (!layerCount) {
				I_DEBUG_LOG_ERROR("No validation layers supported! Program will run with no validation!");
				return false;
			}

			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

			uint32_t layersSupported = 0;
			for (const char* layerName : m_validationLayers) 
				for (const VkLayerProperties& layerProps : availableLayers) 
					if (strcmp(layerName, layerProps.layerName) == '\0') 
						layersSupported++;

			if (m_validationLayers.size() != layersSupported)
				return false;

			return true;
		}
	}
}
