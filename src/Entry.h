#pragma once

#include "App.h"


int main(int argc, char** argv) {

	IRun::CommandLineArguments args{};

	for (int i = 0; i < argc; i++) 
		args.push_back(argv[i]);

	IWindow::Initialize(IWindow::CurrentVersion);

	std::shared_ptr app = IRun::CreateApp();

	app->helper.index<IRun::ECS::Shader, IRun::ECS::VertexData, IRun::ECS::IndexData>("Shader", "VertexData", "IndexData");

	app->OnCreate(args);

	double currentTime = app->window.GetTime();
	double lastTime = currentTime;
	while (app->window.IsRunning()) {
		currentTime = app->window.GetTime();
		double dt = currentTime - lastTime;

		// Does nothing until imgui support is added to the Vulkan renderer
		// app->OnUIRender(dt);

		app->OnRender(dt);

		app->OnUpdate(dt);

		app->OnUpdate((currentTime - lastTime));
		app->window.Update();
		lastTime = currentTime;
	}

	app->OnDestroy();
	app->renderer.Destroy();
	app->window.Destroy();
}