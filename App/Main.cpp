#include <IRun.h>

int main() {
	IRun::Window window{ 800, 600, "IRunVulkan App", IRun::WindowAPI::GLFW };
	// IWindow is in its alpha. GLFW is recommended right now.
	// IRV::Window window{ 800, 600, "IRunVulkan App", IRV::WindowAPI::IWindow };

	IRun::Vulkan::Instance instance{ window }; 
	IRun::Vulkan::Surface surface{ instance, window };
	IRun::Vulkan::PhysicalDevice physicalDevice{ instance, surface };
	IRun::Vulkan::Device device{ physicalDevice, surface };
	 
	while (window.IsRunning()) {
		window.Update();
	}
}