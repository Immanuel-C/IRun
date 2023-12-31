#include <renderer/vulkan/Instance.h>
#include <renderer/vulkan/Device.h>

int main() {
	IWindow::Window window{};

	window.Create(800, 600, "Hello IRun!");

    IRun::VK::Instance instance{ window };
    IRun::VK::Device device{ instance };

    while (window.IsRunning()) {
        window.Update();
    }

    device.Destroy();
    instance.Destroy();
}