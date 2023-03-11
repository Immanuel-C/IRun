#include "Instance.h"

namespace IRun {
    namespace Vulkan {
        Instance::Instance(Window& window) {
            vk::ApplicationInfo appInfo{};
            appInfo.pApplicationName = "IRunVulkanGame";
            appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
            appInfo.pEngineName = "IRunVulkan";
            appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
            appInfo.apiVersion = VK_API_VERSION_1_3;

            vk::InstanceCreateInfo createInfo{};
            createInfo.pApplicationInfo = &appInfo;

            {
                uint32_t extensionCount = 0;
                const char** extensions;

                extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

                createInfo.enabledExtensionCount = extensionCount;
                createInfo.ppEnabledExtensionNames = extensions;
            }

            uint32_t extensionCount = 0;
            vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
            
            std::vector<vk::ExtensionProperties> extensions{ extensionCount };

            vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

            uint32_t supportedExtensionCount = 0;
            for (const vk::ExtensionProperties& extension : extensions) {
                for (int i = 0; i < createInfo.enabledExtensionCount; i++) {
                    if (strcmp(extension.extensionName, createInfo.ppEnabledExtensionNames[i]) == 0) {
                        supportedExtensionCount++;
                    }

                    if (supportedExtensionCount == createInfo.enabledExtensionCount) break;
                }
            }

            if (supportedExtensionCount != createInfo.enabledExtensionCount) 
                I_ASSERT_FATAL_ERROR(true, "Instance extensions not supported!");

#ifdef DEBUG
            const std::vector<const char*> layers = {
                "VK_LAYER_KHRONOS_validation",
                // Shows fps
                "VK_LAYER_LUNARG_monitor"
            };

            uint32_t layerCount = 0;
            vk::enumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<vk::LayerProperties> availableLayers{ layerCount };
            vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            uint32_t supportedLayerCount = 0;
            for (const vk::LayerProperties& layer : availableLayers) {
                for (const char* layerName : layers) {
                    if (strcmp(layer.layerName, layerName) == 0) {
                        supportedLayerCount++;
                    }

                    if (supportedLayerCount == layers.size()) break;
                }
            }

            if (supportedLayerCount == layers.size()) {
                createInfo.enabledLayerCount = layers.size();
                createInfo.ppEnabledLayerNames = layers.data();
            }
            else {
                I_LOG_ERROR("Validation layers not supported! App will run with no validation layers.");
            }
#else
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;
#endif
            VK_CHECK(vk::createInstance(&createInfo, nullptr, &m_instance), "Failed to create vulkan instance!");

            I_DEBUG_LOG_INFO("Instance: %p", (VkInstance)m_instance);

        }

        void Instance::Destroy() {
            I_DEBUG_LOG_INFO("Instance Destroyed: %p", (VkInstance)m_instance);
            m_instance.destroy();
        }


        vk::Instance Instance::Get() const {
            return m_instance;
        }
    }
}