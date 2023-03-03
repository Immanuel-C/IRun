#pragma once

#include <glfw3.h>
#include <ILog.h>
#include <string>


namespace IRun {

	class Window {
	public:
		Window(int64_t width, int64_t height, const std::string& title);
		~Window();

		bool IsRunning() const;
		void Update() const;

		GLFWwindow* GetNativeHandle() const;


		bool WasResized();
		void SetResized(bool resized);

		Window(Window&) = delete;
		Window(Window&&) = delete;
	private:

		const int64_t m_width;
		const int64_t m_height;
		std::string m_title;

		GLFWwindow* m_window;

		static void FramebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);

		bool m_resized = false;
	};
}

