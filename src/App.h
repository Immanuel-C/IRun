#pragma once

#include <IWindow.h>
#include "renderer/vulkan/Renderer.h"

namespace IRun {
	typedef std::vector<std::string> CommandLineArguments;

	class App {
	public:
		virtual void OnCreate(CommandLineArguments& args) {}
		virtual void OnUpdate(double deltaTimeMs) {}
		virtual void OnRender(double deltaTimeMs) {}
		virtual void OnUIRender(double deltaTimeMs) {}
		virtual void OnDestroy() {}

		IWindow::Window window;
		IRun::Vk::Renderer renderer;
		IRun::ECS::Helper helper;
	};


	// Defined in client
	std::shared_ptr<App> CreateApp();
}

