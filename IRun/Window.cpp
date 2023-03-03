#include "Window.h"

namespace IRun {

	Window::Window(int64_t width, int64_t height, const std::string& title)
		: m_width{ width },
		  m_height{ height },
		  m_title{ title }
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
		I_ASSERT_FATAL_ERROR(!m_window, "Failed to create GLFW window!");

		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, FramebufferResizeCallback);
	}


	bool Window::IsRunning() const {
		return !glfwWindowShouldClose(m_window);
	}

	void Window::Update() const {
		glfwPollEvents();
	}

	GLFWwindow* Window::GetNativeHandle() const {
		return m_window;
	}

	bool Window::WasResized() { return m_resized; }

	void Window::SetResized(bool resized) {
		m_resized = resized;
	}

	void Window::FramebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height) {
		Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
		window->SetResized(true);
	}

	Window::~Window() {
		glfwTerminate();
	}

}