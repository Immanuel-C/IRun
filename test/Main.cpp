#include <glad/glad.h>
#include <IWindow.h>
#include <IWindowGL.h>

#include <backends\imgui_impl_opengl3.h>
#include <IWindowImGUIBackend.h>

#include <renderer/vulkan/Instance.h>
#include <renderer/vulkan/Surface.h>
#include <renderer/vulkan/Device.h>
#include <renderer/vulkan/Swapchain.h>

#include <renderer\opengl\VertexBufferObject.h>
#include <renderer\opengl\VertexArrayObject.h>
#include <renderer\opengl\IndexBufferObject.h>
#include <renderer\opengl\ShaderProgram.h>

int TestGL() {
    IWindow::Window window{};
    IWindow::GL::Context glcontext{};



    if (!window.Create(800, 600, "Hello IRun!")) return EXIT_FAILURE;

    if (!glcontext.Create(window, 4, 6)) {
        std::cout << "Failed to create a IWindow context!\nThis is probably because your computor doesn't support the required version of OpenGL\n";
        return EXIT_FAILURE;
    }

    if (!gladLoadGL()) {
        std::cout << "Failed to load glad!\n";
        return EXIT_FAILURE;
    }

    glcontext.MakeContextCurrent();
    glcontext.vSync(false);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplOpenGL3_Init();
    ImGui_ImplIWindow_Init(window);

    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigWindowsResizeFromEdges = true;

    io.DisplaySize.x = (float)window.GetWindowSize().x;
    io.DisplaySize.y = (float)window.GetWindowSize().y;

    ImGui::StyleColorsDark();

    IRun::GL::ShaderProgram shaderProgram{ "shaders/vert.hlsl", "shaders/frag.hlsl" };
    shaderProgram.Bind();

    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
       -0.5f, -0.5f, 0.0f,  // bottom left
       -0.5f,  0.5f, 0.0f   // top left 
    };
    uint32_t indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    IRun::GL::VertexArrayObject vertexArrayObject{};
    vertexArrayObject.Bind();
    IRun::GL::VertexBufferObject vertexBufferObject{ vertices, sizeof(vertices) / sizeof(float) };
    vertexBufferObject.Bind();
    IRun::GL::IndexBufferObject indexBufferObject{ indices, sizeof(indices) / sizeof(uint32_t) };
    indexBufferObject.Bind();
    indexBufferObject.Unbind();
    vertexBufferObject.Unbind();
    vertexArrayObject.Unbind();
    shaderProgram.Unbind();


    while (window.IsRunning()) {
        // ImGui Loop
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        ImGui_ImplIWindow_NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.Bind();
        vertexArrayObject.Bind();
        indexBufferObject.Bind();
        vertexBufferObject.Bind();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        indexBufferObject.Unbind();
        vertexBufferObject.Unbind();
        vertexArrayObject.Unbind();
        shaderProgram.Unbind();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ImGui::EndFrame();

        window.Update();
        glcontext.SwapBuffers();
    }

    return EXIT_SUCCESS;
}

#include <thread>

int TestVK() {
    IWindow::Window window{};

    window.Create(800, 600, "Hello IRun!");

    IRun::Vk::Instance instance{ window };
    IRun::Vk::Surface surface{ window, instance };
    IRun::Vk::Device device{ instance, surface };
    IRun::Vk::Swapchain swapchain{ false, window, surface, device };

    while (window.IsRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        window.Update();
    }

    swapchain.Destroy(device);
    device.Destroy();
    surface.Destroy(instance);
    instance.Destroy();

    return EXIT_SUCCESS;
}

int main() {
    return TestGL();
}