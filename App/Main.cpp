#include <IRun.h>

int main() {
	IRun::Window window{ 800, 600, "IRunVulkan App" };

	IRun::Vulkan::Renderer renderer{ window,  "C:/Users/Immu/Documents/IRun/IRun/Shaders/Bin/Basic.vert.spv", "C:/Users/Immu/Documents/IRun/IRun/Shaders/Bin/Basic.frag.spv", true };

	while (window.IsRunning()) {
		window.Update();

		renderer.Draw();

	}

	renderer.Destroy();
}