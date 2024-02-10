#include <Entry.h>


class TestApp : public IRun::App {
public:
    IRun::ECS::Entity drawableEntity;

    virtual void OnCreate(IRun::CommandLineArguments& args) override {
        window.Create({ 600, 600 }, L"IRun Test Application", IWindow::Monitor::GetPrimaryMonitor());
        renderer = { window, helper, false };

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

        for (int i = 0; i < 100; i++) {
            renderer.AddEntity(entity);
        }

    }

    virtual void OnUIRender(double deltaTimeMs) override {

    }

    virtual void OnRender(double deltaTimeMs) override {
        renderer.Draw();
    }

    virtual void OnUpdate(double deltaTimeMs) override {
        std::wstring title{ L"Delta Time (ms): " };
        title.append(std::to_wstring(deltaTimeMs));
        window.SetTitle(title);
    }

    
    virtual void OnDestroy() override {

    }
};

namespace IRun {
    std::shared_ptr<IRun::App> CreateApp() {
        return std::make_shared<TestApp>();
    }
}