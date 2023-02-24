#pragma once

#include <glfw3.h>
#include <ILog.h>
#include <string>


namespace IRun {
	enum struct WindowAPI
	{
		// IWindow Alpha
		IWindow,
		GLFW
	};

	class Window {
	public:
		Window(int64_t width, int64_t height, const std::string& title);
		~Window();

		bool IsRunning() const;
		void Update() const;

		GLFWwindow* GetNativeHandle() const;

		Window(Window&) = delete;
		Window(Window&&) = delete;
	private:

		const int64_t m_width;
		const int64_t m_height;
		std::string m_title;

		WindowAPI m_api;

		GLFWwindow* m_GLFWWindow;
	};
}

