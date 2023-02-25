#include <IRun.h>

int main() {
	IRun::Window window{ 800, 600, "IRunVulkan App" };


	IRun::Vulkan::Instance instance{ window }; 
	IRun::Vulkan::Surface surface{ instance, window };
	IRun::Vulkan::PhysicalDevice physicalDevice{ instance, surface };
	IRun::Vulkan::Device device{ physicalDevice, surface };
	IRun::Vulkan::SwapChain swapChain{ window, device, physicalDevice, surface, false };
	 
	while (window.IsRunning()) {
		window.Update();
	}

	swapChain.Destroy(device);
	device.Destroy();
	surface.Destroy(instance);
	instance.Destroy();
}