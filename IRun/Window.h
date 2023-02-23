#pragma once

#include <glfw3.h>
#include <IWindow.h>
#include <ILog.h>


namespace IRun {
	enum struct WindowAPI
	{
		// IWindow Alpha
		IWindow,
		GLFW
	};

	class Window {
	public:
		Window(int64_t width, int64_t height, const std::string& title, WindowAPI api);
		~Window();

		bool IsRunning();
		void Update();

		WindowAPI GetAPI();

		IWindow::Window& GetIWindowAPIHandle();
		GLFWwindow* GetGLFWAPIHandle();

		Window(Window&) = delete;
		Window(Window&&) = delete;
	private:
		void InitWindow();

		const int64_t m_width;
		const int64_t m_height;
		std::string m_title;

		WindowAPI m_api;

		IWindow::Window m_iWindow;
		GLFWwindow* m_GLFWWindow;
	};
}

