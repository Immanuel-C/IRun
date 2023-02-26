#include <IRun.h>

int main() {
	IRun::Window window{ 800, 600, "IRunVulkan App" };


	IRun::Vulkan::Instance instance{ window }; 
	IRun::Vulkan::Surface surface{ instance, window };
	IRun::Vulkan::PhysicalDevice physicalDevice{ instance, surface };
	IRun::Vulkan::Device device{ physicalDevice, surface };
	IRun::Vulkan::SwapChain swapChain{ window, device, physicalDevice, surface, false };
	IRun::Vulkan::GraphicsPipeline pipeline{ "C:/Users/Immu/Documents/IRun/IRun/Shaders/Bin/Basic.vert.spv", "C:/Users/Immu/Documents/IRun/IRun/Shaders/Bin/Basic.frag.spv", device, swapChain };
	IRun::Vulkan::Framebuffer framebuffer{ device, swapChain, pipeline };

	while (window.IsRunning()) {
		window.Update();
	}

	framebuffer.Destroy(device);
	pipeline.Destroy(device);
	swapChain.Destroy(device);
	device.Destroy();
	surface.Destroy(instance);
	instance.Destroy();
}