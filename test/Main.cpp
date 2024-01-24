#include <glad/glad.h>
#include <IWindow.h>
#include <IWindowGL.h>

#include <backends/imgui_impl_opengl3.h>
#include <IWindowImGUIBackend.h>

#include <renderer/opengl/VertexBufferObject.h>
#include <renderer/opengl/VertexArrayObject.h>
#include <renderer/opengl/IndexBufferObject.h>
#include <renderer/opengl/ShaderProgram.h>

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


#include <renderer/vulkan/Instance.h>
#include <renderer/vulkan/Surface.h>
#include <renderer/vulkan/Device.h>
#include <renderer/vulkan/Swapchain.h>
#include <renderer/vulkan/GraphicsPipeline.h>
#include <renderer/vulkan/Framebuffers.h>
#include <renderer/vulkan/CommandPool.h>
#include <renderer/vulkan/Sync.h>
#include <renderer/vulkan/Buffer.h>

#include <thread>

constexpr uint32_t MAX_FRAMES = 2;

int TestVK() {
    IWindow::Window window{};

    window.Create(800, 600, "Hello IRun!");

    IRun::Vk::Instance instance{ window };
    IRun::Vk::Surface surface{ window, instance };
    IRun::Vk::Device device{ instance, surface };
    IRun::Vk::Swapchain swapchain{ false, window, surface, device };
    IRun::Vk::GraphicsPipeline graphicsPipeline{ "shaders/vulkanVert.hlsl", "shaders/vulkanFrag.hlsl", device, swapchain };
    IRun::Vk::Framebuffers framebuffers{ swapchain, graphicsPipeline.GetRenderPass(), device };
    IRun::Vk::CommandPool commandPool{ device, device.GetQueueFamilies().graphicsFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT };
    std::vector<IRun::Vk::CommandBuffer> commandBuffers{};
    
    for (int i = 0; i < framebuffers.Get().size(); i++) 
        commandBuffers.emplace_back(commandPool.CreateBuffer(device, IRun::Vk::CommandBufferLevel::Primary));
    
    std::vector<IRun::Vk::Sync<IRun::Vk::Semaphore>> imageAvailableSemaphore{};
    imageAvailableSemaphore.resize(MAX_FRAMES);
    for (IRun::Vk::Sync<IRun::Vk::Semaphore>& semaphore : imageAvailableSemaphore) 
        semaphore = IRun::Vk::Sync<IRun::Vk::Semaphore>{ device };

    std::vector<IRun::Vk::Sync<IRun::Vk::Semaphore>> renderFinishedSemaphore{};
    renderFinishedSemaphore.resize(MAX_FRAMES);
    for (IRun::Vk::Sync<IRun::Vk::Semaphore>& semaphore : renderFinishedSemaphore) 
        semaphore = IRun::Vk::Sync<IRun::Vk::Semaphore>{ device };

    std::vector<IRun::Vk::Sync<IRun::Vk::Fence>> drawFences{};
    drawFences.resize(MAX_FRAMES);
    for (IRun::Vk::Sync<IRun::Vk::Fence>& fence : drawFences)
        fence = IRun::Vk::Sync<IRun::Vk::Fence>{ device, VK_FENCE_CREATE_SIGNALED_BIT };
    
    std::array<IRun::Vertex, 3> vertices{
        glm::vec3{  0.0f, -0.5f,  0.0f },
        glm::vec3{  0.5f,  0.5f,  0.0f },
        glm::vec3{ -0.5f,  0.5f,  0.0f },
    };

    IRun::Vk::Buffer<IRun::Vertex, vertices.size()> shape{ device, vertices.data(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = graphicsPipeline.GetRenderPass();
    renderPassBeginInfo.framebuffer = nullptr;
    renderPassBeginInfo.renderArea.offset = { 0, 0 };
    renderPassBeginInfo.renderArea.extent = swapchain.GetChosenSwapchainDetails().first;
    VkClearValue clearColour{};
    clearColour.color = { { 0.25f, 0.5f, 0.75f, 1.0f } };
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColour;

    uint32_t currentFrame = 0;

    while (window.IsRunning()) {
        VkFence fencesToWaitFor[] = {
            drawFences[currentFrame].Get()
        };

        vkWaitForFences(device.Get().first, 1, fencesToWaitFor, true, UINT64_MAX);
        vkResetFences(device.Get().first, 1, fencesToWaitFor);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(device.Get().first, swapchain.Get(), UINT64_MAX, imageAvailableSemaphore[currentFrame].Get(), nullptr, &imageIndex);

        renderPassBeginInfo.framebuffer = framebuffers[imageIndex];

        commandPool.BeginRecordingCommands(device, commandBuffers[imageIndex]);

            vkCmdBeginRenderPass(commandPool[commandBuffers[imageIndex]], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

                vkCmdBindPipeline(commandPool[commandBuffers[imageIndex]], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.Get());

                VkViewport viewport{};
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = (float)swapchain.GetChosenSwapchainDetails().first.width;
                viewport.height = (float)swapchain.GetChosenSwapchainDetails().first.height;
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;

                vkCmdSetViewport(commandPool[commandBuffers[imageIndex]], 0, 1, &viewport);

                VkRect2D scissor{};
                scissor.offset = { 0, 0 };
                scissor.extent = { swapchain.GetChosenSwapchainDetails().first.width, swapchain.GetChosenSwapchainDetails().first.height };

                vkCmdSetScissor(commandPool[commandBuffers[imageIndex]], 0, 1, &scissor);

                VkBuffer buffers[] = {
                    shape.Get()
                };

                VkDeviceSize offsets[] = {
                    0
                };

                vkCmdBindVertexBuffers(commandPool[commandBuffers[imageIndex]], 0, 1, buffers, offsets);

                vkCmdDraw(commandPool[commandBuffers[imageIndex]], (uint32_t)shape.GetSize(), 1, 0, 0);

            vkCmdEndRenderPass(commandPool[commandBuffers[imageIndex]]);

        commandPool.EndRecordingCommands(commandBuffers[imageIndex]);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        VkSemaphore imageAvailableSemaphorePtr = imageAvailableSemaphore[currentFrame].Get();
        submitInfo.pWaitSemaphores = &imageAvailableSemaphorePtr;
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        // 1:1 with pWaitSemaphores
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        VkCommandBuffer commandBufferPtr = commandPool[commandBuffers[imageIndex]];
        submitInfo.pCommandBuffers = &commandBufferPtr;
        submitInfo.signalSemaphoreCount = 1;
        VkSemaphore renderFinishedSemaphorePtr = imageAvailableSemaphore[currentFrame].Get();
        submitInfo.pSignalSemaphores = &renderFinishedSemaphorePtr;

        VK_CHECK(vkQueueSubmit(device.GetQueues().at(IRun::Vk::QueueType::Graphics), 1, &submitInfo, drawFences[currentFrame].Get()), "Failed to sumbit semaphore and command buffer info to graphics queue!");

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphorePtr;
        presentInfo.swapchainCount = 1;
        VkSwapchainKHR swapchainPtr = swapchain.Get();
        presentInfo.pSwapchains = &swapchainPtr;
        presentInfo.pImageIndices = &imageIndex;

        VK_CHECK(vkQueuePresentKHR(device.GetQueues().at(IRun::Vk::QueueType::Presentation), &presentInfo), "Failed to present swapchain image");

        currentFrame = (currentFrame + 1) % MAX_FRAMES;

        window.Update();
    }

    // Wait until the device isn't doing anything
    vkDeviceWaitIdle(device.Get().first);

    shape.Destroy(device);
    for (IRun::Vk::Sync<IRun::Vk::Fence>& fence : drawFences)
        fence.Destroy(device);
    for (IRun::Vk::Sync<IRun::Vk::Semaphore>& semaphore : imageAvailableSemaphore) 
        semaphore.Destroy(device);
    for (IRun::Vk::Sync<IRun::Vk::Semaphore>& semaphore : renderFinishedSemaphore) 
        semaphore.Destroy(device);
    commandPool.Destroy(device);
    framebuffers.Destroy(device);
    graphicsPipeline.Destroy(device);
    swapchain.Destroy(device);
    device.Destroy();
    surface.Destroy(instance);
    instance.Destroy();

    return EXIT_SUCCESS;
}

int main() {
    return TestVK();
}