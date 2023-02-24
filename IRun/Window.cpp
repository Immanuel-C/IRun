#include "Window.h"

namespace IRun {

	Window::Window(int64_t width, int64_t height, const std::string& title)
		: m_width{ width },
		  m_height{ height },
		  m_title{ title }
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_GLFWWindow = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
		I_ASSERT_FATAL_ERROR(!m_GLFWWindow, "Failed to create GLFW window!");
	}


	bool Window::IsRunning() const {
		return !glfwWindowShouldClose(m_GLFWWindow);
	}

	void Window::Update() const {
		glfwPollEvents();
	}

	GLFWwindow* Window::GetNativeHandle() const {
		return m_GLFWWindow;
	}

	Window::~Window() {
		glfwDestroyWindow(m_GLFWWindow);
		glfwTerminate();
	}

}