#include <Entry.h>

#include <renderer/camera/Camera3D.h>

bool firstMouse = true;
float yaw = -90.0f, pitch = 0.0f;
IWindow::Vector2<int32_t> lastPosition{};

static void MouseMoveCallback(IWindow::Window& window, IWindow::Vector2<int32_t> position);

class TestApp : public IRun::App {
public:
    IRun::Camera3D camera;

    IRun::ECS::Entity drawableEntity;

    virtual void OnCreate(IRun::CommandLineArguments& args) override {
        window.Create({ 1920, 1080 }, L"IRun Test Application", IWindow::Monitor::GetPrimaryMonitor());

        camera = IRun::Camera3D{ 90.0f, 1280.0f / 720.0f, glm::vec2{ 0.1f, 100.0f }, glm::vec3{ 0.0f, 0.0f, 3.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f } };

        renderer = { window, camera, helper, true };

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

        for (int i = 0; i < 1; i++) {
            renderer.AddEntity(entity);
        }

        window.SetUserPointer(this);

        window.SetMouseMoveCallback(MouseMoveCallback);
        window.SetCursor(IWindow::CursorID::Hidden);
       // window.Fullscreen(true, IWindow::Monitor::GetPrimaryMonitor());

        RECT clipRect{};
        ::GetClientRect(window.GetNativeWindowHandle(), &clipRect);
        ::ClientToScreen(window.GetNativeWindowHandle(), (POINT*)&clipRect.left);
        ::ClientToScreen(window.GetNativeWindowHandle(), (POINT*)&clipRect.right);
        ::ClipCursor(&clipRect);

        window.Fullscreen(true, IWindow::Monitor::GetPrimaryMonitor());
    }

    virtual void OnUIRender(double deltaTimeMs) override {
    }

    virtual void OnRender(double deltaTimeMs) override {
        renderer.ClearColor({ 100.0f, 100.0f, 100.0f });
        renderer.Draw();
    }

    const float movementSpeed = 0.1f;
    glm::vec3 cameraPos = { 0.0f, 0.0f, 3.0f };
    const glm::vec3 cameraUp = { 0.0f, 1.0f, 0.0f };

    virtual void OnUpdate(double deltaTimeMs) override {
        std::wstring title{ L"Delta Time (ms): " };
        title.append(std::to_wstring(deltaTimeMs));
        window.SetTitle(title);

        if (window.IsKeyDown(IWindow::Key::W))
            camera.SetPosition(camera.GetPosition() + (movementSpeed * camera.GetFront()));
        if (window.IsKeyDown(IWindow::Key::S)) 
            camera.SetPosition(camera.GetPosition() - (movementSpeed * camera.GetFront()));
        if (window.IsKeyDown(IWindow::Key::A)) 
            camera.SetPosition(camera.GetPosition() - (glm::normalize(glm::cross(camera.GetFront(), camera.GetUp())) * movementSpeed));
        if (window.IsKeyDown(IWindow::Key::D))
            camera.SetPosition(camera.GetPosition() + (glm::normalize(glm::cross(camera.GetFront(), camera.GetUp())) * movementSpeed));

       camera.Update(window);
    }

    
    virtual void OnDestroy() override {

    }
};

static void MouseMoveCallback(IWindow::Window& window, IWindow::Vector2<int32_t> position) {
    TestApp* testApp = (TestApp*)window.GetUserPointer();

    if (firstMouse)
    {
        lastPosition = position;
        firstMouse = false;
    }

    float xoffset = (float)position.x - lastPosition.x;
    float yoffset = (float)position.y - lastPosition.y;
    lastPosition.x = position.x;
    lastPosition.y = position.y;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction{};
    direction.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
    direction.y = sinf(glm::radians(pitch));
    // Convert from origin being in bottom left to top left.
    direction.y *= -1.0f;
    direction.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));
    testApp->camera.SetFront(glm::normalize(direction));
}

namespace IRun {
    std::shared_ptr<IRun::App> CreateApp() {
        return std::make_shared<TestApp>();
    }
}