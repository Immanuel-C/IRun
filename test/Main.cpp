#include <imgui.h>
#include <backends\imgui_impl_opengl3.h>
#include <glad\glad.h>
#include <IWindowImGUIBackend.h>
#include <IWindow.h>
#include <IWindowGL.h>
#include <ILog.h>
#include <thread>
#include <renderer/opengl/ShaderProgram.h>
#include <renderer/opengl/VertexArrayObject.h>
#include <renderer/opengl/VertexBufferObject.h>
#include <renderer/opengl/IndexBufferObject.h>


int main() {
	IWindow::Window window{};
	IWindow::GL::Context glcontext{};

	window.Create(800, 600, "Hello IRun!");
	glcontext.Create(window, 4, 6);
	gladLoadGL();

	glcontext.MakeContextCurrent();

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

    double prevTime = window.GetTime(), currentTime = 0.0;
    int framecount = 0, prevFramecount = 0;

    std::array<float, 12> vertices {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    std::array<uint32_t, 6> indices {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    IRun::GL::ShaderProgram shader{ "shaders/vert.hlsl", "shaders/frag.hlsl" };
    shader.Bind();
    IRun::GL::VertexArrayObject vao{};
    vao.Bind();
    IRun::GL::VertexBufferObject vbo{ vertices.data(), vertices.size() };
    vbo.Bind();
    IRun::GL::IndexBufferObject ibo{ indices.data(), indices.size() };
    ibo.Bind();
    ibo.Unbind();
    vbo.Unbind();
    vao.Unbind();
    shader.Unbind();


    while (window.IsRunning()) {

        currentTime = window.GetTime();
        framecount++;

        // ImGui Loop
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        ImGui_ImplIWindow_NewFrame();
        ImGui::ShowDemoWindow();
        
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

        ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
        if (ImGui::Begin("Example: Simple overlay", (bool*)true, window_flags))
        {
            ImGui::Text("FPS Counter");
            ImGui::Separator();
            ImGui::Text("%f", io.Framerate * 1000.0f);
        }
        ImGui::End();
        
        ImGui::Render();
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Bind();
        vao.Bind();
        vbo.Bind();
        ibo.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        ibo.Unbind();
        vbo.Unbind();
        vao.Unbind();
        shader.Unbind();



        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ImGui::EndFrame();



        window.Update();
        glcontext.SwapBuffers();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplIWindow_Shutdown();
    ImGui::DestroyContext();
}