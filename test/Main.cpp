#include <IWindow.h>
#include <IWindowGL.h>

#include <backends/imgui_impl_opengl3.h>
#include <IWindowImGUIBackend.h>

#include <renderer/opengl/VertexBufferObject.h>
#include <renderer/opengl/VertexArrayObject.h>
#include <renderer/opengl/IndexBufferObject.h>
#include <renderer/opengl/ShaderProgram.h>

#include <math/Vector.h>

#include <glad/glad.h>

constexpr int64_t WIDTH = 1280, HEIGHT = 720;
constexpr char const* TITLE = "Hello IRun!";

constexpr IRun::Math::Color CLEAR_COLOUR = { 100, 215, 156 };

int TestGL() {
    IWindow::Window window{};
    IWindow::GL::Context glcontext{};

    if (!window.Create(WIDTH, HEIGHT, TITLE)) return EXIT_FAILURE;

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
        // Position             Uv
        0.5f,  0.5f, 0.0f,      1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,      1.0f, 0.0f, // bottom right
       -0.5f, -0.5f, 0.0f,      0.0f, 0.0f, // bottom left
       -0.5f,  0.5f, 0.0f,      0.0f, 1.0f, // top left 
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

        glClearColor((CLEAR_COLOUR.r / 255.0f), (CLEAR_COLOUR.g / 255.0f), (CLEAR_COLOUR.b / 255.0f), 1.0f);
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

#include <ecs\Components.h>
#include <renderer/vulkan/Renderer.h>
#include <thread>

int TestRendererVk() {
    IWindow::Window window{};

    window.Create(WIDTH, HEIGHT, TITLE);

    IRun::ECS::Helper helper{};

    helper.index<IRun::ECS::VertexData, IRun::ECS::IndexData, IRun::ECS::Shader>("VertexData", "IndexData", "Shader");

    IRun::Vk::Renderer renderer{ window, helper, true };

    std::vector<IRun::Vertex> vertexData = {
        { { -0.5f, -0.5f,  0.0f }, { 0.0f, 1.0f } },  // Top Left:     0
        { {  0.5f, -0.5f,  0.0f }, { 1.0f, 1.0f } },  // Top Right:    1
        { {  0.5f,  0.5f,  0.0f }, { 1.0f, 0.0f } },  // Bottom Right: 2
        { { -0.5f,  0.5f,  0.0f }, { 0.0f, 0.0f } },  // Bottom Left:  3
    };

    std::vector<uint32_t> indexData = {
        0, 1, 2, // Top Left
        2, 3, 0  // Bottom Right
    };

    IRun::ECS::Entity entity = helper.create<IRun::ECS::VertexData, IRun::ECS::IndexData, IRun::ECS::Shader>(
        {
            vertexData
        },
        {
            indexData
        },
        {
            "shaders/vert.hlsl",
            "shaders/frag.hlsl",
            IRun::ShaderLanguage::HLSL
        }
    );

    renderer.AddEntity(entity);

    IWindow::Monitor primaryMonitor = window.GetPrimaryMonitor();

    while (window.IsRunning()) {
        renderer.ClearColor({ 100, 215, 156 });
        renderer.Draw();

        if (window.IsKeyDown(IWindow::Key::F) && !window.IsFullscreen()) {
            window.Fullscreen(true, primaryMonitor);
        }
        else if (window.IsKeyUp(IWindow::Key::F) && window.IsFullscreen()) {
            window.Fullscreen(false, primaryMonitor);
            window.SetWindowSize(WIDTH, HEIGHT);
        }

        window.Update();
    }

    renderer.Destroy();

    return EXIT_SUCCESS;
}


int TestECS() {
    IRun::ECS::Helper helper{};

    helper.index<IRun::ECS::VertexData, IRun::ECS::Shader>("VertexData", "Shader");

    std::vector<IRun::Vertex> vertexData = {
        IRun::Vertex{ { 1.0f, 0.0f,  1.0f } },
        IRun::Vertex{ { 0.5f, 0.14f, 0.12532523f } },
    };

    IRun::ECS::Entity entity = helper.create<IRun::ECS::VertexData, IRun::ECS::Shader>(
        {
            vertexData
        }, 
        {
            "vert/file/name.hlsl",
            "frag/file/name.hlsl",
            IRun::ShaderLanguage::HLSL
        }
    );

    IRun::ECS::SerializedEntity serializedEntity = IRun::ECS::Serialize<IRun::ECS::VertexData, IRun::ECS::Shader>(helper, entity);

    IRun::ECS::Entity deserializedEntity = IRun::ECS::Deserialize<IRun::ECS::VertexData, IRun::ECS::Shader>(helper, serializedEntity);

    auto [data, shader] = helper.get<IRun::ECS::VertexData, IRun::ECS::Shader>(entity);

    return EXIT_SUCCESS;
}

int main() {
    return TestGL();
}