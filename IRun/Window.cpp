#include "Window.h"

namespace IRun {

	Window::Window(int64_t width, int64_t height, const std::string& title, WindowAPI api)
		: m_width{ width },
		  m_height{ height },
		  m_title{ title },
		  m_api{ api }
	{
		InitWindow();
	}

	void Window::InitWindow() {
		if (m_api == WindowAPI::IWindow) {
			I_ASSERT_FATAL_ERROR(!m_iWindow.Create(m_width, m_height, m_title), "Failed to create IWindow window!");
		}
		else if (m_api == WindowAPI::GLFW) { 
			glfwInit();

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

			m_GLFWWindow = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
			I_ASSERT_FATAL_ERROR(!m_GLFWWindow, "Failed to create GLFW window!");
		}
	}

	bool Window::IsRunning() {
		return m_api == WindowAPI::IWindow ? m_iWindow.IsRunning() : !glfwWindowShouldClose(m_GLFWWindow);
	}

	void Window::Update() {
		m_api == WindowAPI::IWindow ? m_iWindow.Update() : glfwPollEvents();
	}

	WindowAPI Window::GetAPI()
	{
		return m_api;
	}

	IWindow::Window& Window::GetIWindowAPIHandle() {
		return m_iWindow;
	}

	GLFWwindow* Window::GetGLFWAPIHandle() {
		return m_GLFWWindow;
	}

	Window::~Window() {
		glfwDestroyWindow(m_GLFWWindow);
		glfwTerminate();
	}

}