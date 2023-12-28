#include <renderer/vulkan/Instance.h>

int main() {
	IWindow::Window window{};

	window.Create(800, 600, "Hello IRun!");

    IRun::VK::Instance instance{ window };

    while (window.IsRunning()) {
        window.Update();
    }

    instance.Destroy();
}