#include "app.hpp"
#include "simulation.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FROCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>

namespace engine
{
    Simulation simulation;

    float aspect = 1;
    float rToPixels = 555;

    struct Teksty
    {
        const char* kulki;
        const char* fps;
        const char* symulacjaAktywna;
        const char* krokiSymulacji;
        const char* skalaCzasu;
        const char* grawitacja;
        const char* sila;
        const char* maxX;
        const char* watki;
        const char* dodajKulke;
        const char* promien;
        const char* masa;
        const char* pozX;
        const char* pozY;
        const char* predkoscX;
        const char* predkoscY;
        const char* kolor;
        const char* przyciskDodajKulke;
        const char* dodajGaz;
        const char* ilosc;
        const char* promienGazu;
        const char* masaGazu;
        const char* lewo;
        const char* prawo;
        const char* gora;
        const char* dol;
        const char* kolorGazu;
        const char* przyciskDodajGaz;
        const char* usunWszystko;
        const char* wyzerujPredkosci;
        const char* jezyk;
        const char* zastosuj;
    };
    bool jezykPolski = true;
    static const Teksty PL =
    {
        "Kulki: %d",
        "FPS: %.0f",
        "Symulacja aktywna",
        "Liczba kroków symulacji",
        "Skala czasu",
        "Grawitacja",
        "Siła przeciągania myszką",
        "Max X",
        "Liczba wątków",
        "Dodaj kulkę:",
        "Promień",
        "Masa",
        "Pozycja X",
        "Pozycja Y",
        "Prędkość X",
        "Prędkość Y",
        "Kolor",
        "Dodaj kulkę",
        "Dodaj gaz:",
        "Ilość",
        "Promień gazu",
        "Masa gazu",
        "Lewo",
        "Prawo",
        "Góra",
        "Dół",
        "Kolor gazu",
        "Dodaj gaz",
        "Usuń wszystko",
        "Wyzeruj predkości",
        "Switch to english",
        "Zastosuj"
    };
    static const Teksty EN =
    {
        "Balls: %d",
        "FPS: %.0f",
        "Simulation active",
        "Number of simulation steps",
        "Time scale",
        "Gravity",
        "Mouse drag force",
        "Max X",
        "Number of threads",
        "Add ball:",
        "Radius",
        "Mass",
        "Position X",
        "Position Y",
        "Velocity X",
        "Velocity Y",
        "Color",
        "Add ball",
        "Add gas:",
        "Amount",
        "Gas radius",
        "Gas mass",
        "Left",
        "Right",
        "Up",
        "Down",
        "Gas color",
        "Add gas",
        "Clear all",
        "Reset speeds",
        "Przełącz na polski",
        "Apply"
    };

    App::App()
    {
        simulation = Simulation();
        LoadModels();
        CreatePipelineLayout();
        RecreateSwapChain();
        CreateCommandBuffers();
        InitImGui();
    }
    App::~App()
    {
        ShutdownImGui();
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void App::Run()
    {
        while (!window.ShouldClose())
        {
            simulation.Update();
            glfwPollEvents();
            DrawFrame();
        }

        vkDeviceWaitIdle(device.device());
    }

    void AddTriangle(std::vector<Model::Vertex>& vertices, float x1, float y1, float x2, float y2, float x3, float y3)
    {
        vertices.push_back(Model::Vertex{{x1, y1}});
        vertices.push_back(Model::Vertex{{x2, y2}});
        vertices.push_back(Model::Vertex{{x3, y3}});
    }

    void AddCircleFan(std::vector<Model::Vertex>& vertices, float cx, float cy, float r, int segments)
    {
        vertices.push_back(Model::Vertex{{cx, cy}});

        for (int i = 0; i <= segments; i++)
        {
            float a = (float)i / segments * 2.0f * M_PI;
            vertices.push_back(Model::Vertex{{cx + r * cos(a), cy + r * sin(a)}});
        }
    }

    void App::LoadModels()
    {
        std::vector<Model::Vertex> vertices4 {};
        vertices4.push_back(Model::Vertex{{-1, -1}});
        vertices4.push_back(Model::Vertex{{-1, 1}});
        vertices4.push_back(Model::Vertex{{1, -1}});
        vertices4.push_back(Model::Vertex{{1, -1}});
        vertices4.push_back(Model::Vertex{{-1, 1}});
        vertices4.push_back(Model::Vertex{{1, 1}});
        modelCirc4 = std::make_unique<Model>(device, vertices4);

        std::vector<Model::Vertex> vertices8 {};
        AddCircleFan(vertices8,  0, 0, 1.0f, 8);
        modelCirc8 = std::make_unique<Model>(device, vertices8);

        std::vector<Model::Vertex> vertices16 {};
        AddCircleFan(vertices16, 0, 0, 1.0f, 16);
        modelCirc16 = std::make_unique<Model>(device, vertices16);

        std::vector<Model::Vertex> vertices32 {};
        AddCircleFan(vertices32, 0, 0, 1.0f, 32);
        modelCirc32 = std::make_unique<Model>(device, vertices32);

        std::vector<Model::Vertex> vertices64 {};
        AddCircleFan(vertices64, 0, 0, 1.0f, 64);
        modelCirc64 = std::make_unique<Model>(device, vertices64);
    }

    void App::CreatePipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != 0)
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    void App::CreatePipeline()
    {
        assert(swapChain != nullptr && "can not create pipeline before swapchain");
        assert(pipelineLayout != nullptr && "can not create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig {};
        Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = swapChain->GetRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(device, "shaders/triangle.vert.spv", "shaders/triangle.frag.spv", pipelineConfig);
    }

    void App::RecreateSwapChain()
    {
        auto extent = window.GetExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = window.GetExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device.device());

        if (swapChain == nullptr)
        {
            swapChain = std::make_unique<SwapChain>(device, extent);
        }
        else
        {
            swapChain = std::make_unique<SwapChain>(device, extent, std::move(swapChain));
            if (swapChain->ImageCount() != commandBuffers.size())
            {
                FreeCommandBuffers();
                CreateCommandBuffers();
            }
        }
        aspect = static_cast<float>(swapChain->GetSwapChainExtent().width) / static_cast<float>(swapChain->GetSwapChainExtent().height);
        rToPixels = static_cast<float>(swapChain->GetSwapChainExtent().height) / 2.0f;
        CreatePipeline();
    }

    void App::CreateCommandBuffers()
    {
        commandBuffers.resize(swapChain->ImageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = device.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
        if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != 0)
        {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void App::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    void App::RecordCommandBuffer(int imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != 0)
        {
            throw std::runtime_error("failed to begin recording command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->GetRenderPass();
        renderPassInfo.framebuffer = swapChain->GetFrameBuffer(imageIndex);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain->GetSwapChainExtent();

        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color = {0.03f, 0.03f, 0.03f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain->GetSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain->GetSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, swapChain->GetSwapChainExtent()};
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        // renderowanie wszystkich kulek na raz
        std::vector<Model::InstanceData> instances;
        instances.reserve(simulation.balls.size());
        for (Ball& ball : simulation.balls)
        {
            instances.push_back({
                {ball.r / 255.0f, ball.g / 255.0f, ball.b / 255.0f},
                {ball.px, ball.py},
                ball.radius,
                aspect
            });
        }
        modelCirc32->UpdateInstances(instances);
        pipeline->Bind(commandBuffers[imageIndex]);
        modelCirc32->BindAndDraw(commandBuffers[imageIndex]);

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float winW = static_cast<float>(swapChain->GetSwapChainExtent().width);
        float winH = static_cast<float>(swapChain->GetSwapChainExtent().height);

        if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_MouseRight, false))
        {
            ImVec2 mPos = ImGui::GetMousePos();
            float x = mPos.x;
            float y = mPos.y;
            float y1 = winH / 2;
            float dlugoscX = maxX + 1;
            y = 2*y/winH - 1;
            x = x / (dlugoscX * winH / 2) * dlugoscX - 1;

            for (uint16_t i = 0; i < simulation.balls.size(); i++)
            {
                Ball& ball = simulation.balls[i];
                float dx = x - ball.px;
                float dy = y - ball.py;
                if (dx * dx + dy * dy < ball.radius * ball.radius)
                {
                    auto it = simulation.balls.begin() + i;
                    simulation.balls.erase(it);
                    break;
                }
            }
        }

        static float radiusP;
        static float mP;
        static float colorP[3] = {0.5f, 0.5f, 0.5f};

        static bool zaznaczony = false;

        static bool przeciaganie = false;
        static uint16_t id;
        static ImVec2 previousMousePos;
        static float force = 5;
        if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_MouseLeft, false))
        {
            ImVec2 mPos = ImGui::GetMousePos();
            float x = mPos.x;
            float y = mPos.y;
            float y1 = winH / 2;
            float dlugoscX = maxX + 1;
            y = 2*y/winH - 1;
            x = x / (dlugoscX * winH / 2) * dlugoscX - 1;

            for (uint16_t i = 0; i < simulation.balls.size(); i++)
            {
                Ball& ball = simulation.balls[i];
                float dx = x - ball.px;
                float dy = y - ball.py;
                if (dx * dx + dy * dy < ball.radius * ball.radius)
                {
                    przeciaganie = true;
                    zaznaczony = true;
                    id = ball.id;

                    radiusP = ball.radius;
                    mP = ball.m;
                    colorP[0] = ball.r / 255.0f;
                    colorP[1] = ball.g / 255.0f;
                    colorP[2] = ball.b / 255.0f;

                    previousMousePos = mPos;
                    break;
                }
            }
        }
        if (przeciaganie && ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_MouseLeft))
            przeciaganie = false;
        if (przeciaganie)
        {
            ImVec2 mPos = ImGui::GetMousePos();
            float dx = mPos.x - previousMousePos.x;
            float dy = mPos.y - previousMousePos.y;
            dx /= winH;
            dy /= winH;
            previousMousePos = mPos;
            for (Ball& ball : simulation.balls)
            {
                if (ball.id == id)
                {
                    ball.vx += dx * force;
                    ball.vy += dy * force;
                    break;
                }
            }
        }

        static float fontSize = 1.0f;
        if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Minus) || ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Equal))
        {
            if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Equal)) fontSize += 0.0625f;
            else fontSize -= 0.0625f;
            if (fontSize < 0.5f) fontSize = 0.5f;
            if (fontSize > 2.0f) fontSize = 2.0f;
            ImGuiIO& io = ImGui::GetIO();
            io.FontGlobalScale = fontSize;
            std::cout<<fontSize<<'\n';
        }

        float simWidth = winH * ((maxX - minX) / (maxY - minY));
        float panelX = simWidth;
        float panelWidth = winW - simWidth;

        const Teksty& t = jezykPolski ? PL : EN;

        ImGui::SetNextWindowPos(ImVec2(panelX, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(panelWidth, winH - 0.0f), ImGuiCond_Always);
        ImGui::Begin("Symulacja", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        if (ImGui::Button(t.jezyk))
            jezykPolski = !jezykPolski;
        ImGui::Text(t.kulki, (int)simulation.balls.size());
        ImGui::SameLine();
        ImGui::Text(t.fps, ImGui::GetIO().Framerate);

        ImGui::Separator();

        ImGui::Checkbox(t.symulacjaAktywna, &symulacjaAktywna);
        int tmpKroki = simulation.krokiFizyki;
        ImGui::SliderInt(t.krokiSymulacji, &tmpKroki, 1, 9);
        simulation.krokiFizyki = static_cast<uint8_t>(tmpKroki);
        ImGui::SliderFloat(t.skalaCzasu, &timeScale, -2.0f, 2.0f, "%.2f");
        ImGui::SliderFloat(t.grawitacja, &g, -10.0f, 10.0f, "%.1f");
        ImGui::SliderFloat(t.sila, &force, 0.0f, 32.0f, "%.0f");
        ImGui::InputFloat(t.maxX, &maxX, 0.01f, 0.01f, "%.2f");

        ImGui::Separator();

        ImGui::Text(t.dodajKulke);

        static float radius = 0.05f;
        static float mass = 1.0f;
        static float px = 0.0f;
        static float py = 0.0f;
        static float vx = 0.0f;
        static float vy = 0.0f;
        static float color[3] = {0.125f, 0.5f, 1.0f};

        ImGui::SliderFloat(t.promien, &radius, 0.01f, 0.3f, "%.3f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat(t.masa, &mass, 0.01f, 100.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat(t.pozX, &px, minX, maxX, "%.3f");
        ImGui::SliderFloat(t.pozY, &py, minY, maxY, "%.3f");
        ImGui::SliderFloat(t.predkoscX, &vx, -5.0f, 5.0f, "%.3f");
        ImGui::SliderFloat(t.predkoscY, &vy, -5.0f, 5.0f, "%.3f");
        ImGui::ColorEdit3(t.kolor, color);

        if (ImGui::Button(t.przyciskDodajKulke))
        {
            simulation.AddBall(
                radius,
                mass,
                px, py,
                vx, vy,
                (uint8_t)(color[0] * 255),
                (uint8_t)(color[1] * 255),
                (uint8_t)(color[2] * 255)
            );
        }

        ImGui::Separator();

        ImGui::Text(t.dodajGaz);

        static int ilosc = 100;
        static float radiusG = 0.03f;
        static float massG = 0.1f;
        static float left = minX;
        static float right = maxX;
        static float up = minY;
        static float down = maxY;
        static float colorG[3] = {0.35f, 0.35f, 0.35f};

        ImGui::SliderInt(t.ilosc, &ilosc, 1, 32000, "%d", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat(t.promienGazu, &radiusG, 0.001f, 0.1f, "%.4f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat(t.masaGazu, &massG, 0.01f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat(t.lewo, &left, minX, maxX, "%.3f");
        ImGui::SliderFloat(t.prawo, &right, minX, maxX, "%.3f");
        ImGui::SliderFloat(t.gora, &up, minY, maxY, "%.3f");
        ImGui::SliderFloat(t.dol, &down, minY, maxY, "%.3f");
        ImGui::ColorEdit3(t.kolorGazu, colorG);

        if (ImGui::Button(t.przyciskDodajGaz))
        {
            simulation.AddGas(
                ilosc, radiusG, massG, left, right, up, down, (uint8_t)(colorG[0] * 255), (uint8_t)(colorG[1] * 255), (uint8_t)(colorG[2] * 255)
            );
        }

        ImGui::Separator();

        if (ImGui::Button(t.usunWszystko))
        {
            simulation.ClearAll();
        }
        ImGui::SameLine();
        if (ImGui::Button(t.wyzerujPredkosci))
        {
            for (Ball& ball : simulation.balls)
            {
                ball.vx = 0.0f;
                ball.vy = 0.0f;
            }
        }

        if (zaznaczony)
        {
            ImGui::Separator();

            std::string promien = t.promien;
            promien += "##2";
            std::string masa = t.masa;
            masa += "##2";
            std::string prex = t.predkoscX;
            prex += "##2";
            std::string prey = t.predkoscY;
            prey += "##2";
            std::string kolo = t.kolor;
            kolo += "##2";

            ImGui::SliderFloat(promien.c_str(), &radiusP, 0.01f, 0.3f, "%.3f", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat(masa.c_str(), &mP, 0.01f, 100.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
            ImGui::ColorEdit3(kolo.c_str(), colorP);

            if (ImGui::Button(t.zastosuj))
            {
                for (Ball& ball : simulation.balls)
                {
                    if (ball.id == id)
                    {
                        ball.radius = radiusP;
                        ball.m = mP;
                        ball.r = colorP[0] * 255.0f;
                        ball.g = colorP[1] * 255.0f;
                        ball.b = colorP[2] * 255.0f;
                        break;
                    }
                }
            }
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);


        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != 0)
        {
            throw std::runtime_error("failed to record command buffer");
        }
    }

    void App::DrawFrame()
    {
        uint32_t imageIndex;
        auto result = swapChain->AcquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain();
            return;
        }

        if (result != 0 && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to get swap chain image");
        }

        RecordCommandBuffer(imageIndex);
        result = swapChain->SubmitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.WasWindowResized())
        {
            window.ResetWindowResizedFlag();
            RecreateSwapChain();
            return;
        }
        if (result != 0)
        {
            throw std::runtime_error("failed to show swap chain image");
        }
    }

    void App::InitImGui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        static const ImWchar ranges[] = {
            0x0020, 0x00FF,
            0x0100, 0x017F,
            0,
        };

        io.Fonts->AddFontFromFileTTF(
            "Roboto-Medium.ttf",
            16.0f,
            nullptr,
            ranges
        );

        ImGui_ImplGlfw_InitForVulkan(window.GetGLFWwindow(), true);

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.ApiVersion = VK_API_VERSION_1_0;
        initInfo.Instance = device.getInstance();
        initInfo.PhysicalDevice = device.getPhysicalDevice();
        initInfo.Device = device.device();
        initInfo.QueueFamily = device.findPhysicalQueueFamilies().graphicsFamily;
        initInfo.Queue = device.graphicsQueue();
        initInfo.DescriptorPool = device.getImGuiDescriptorPool();
        initInfo.MinImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
        initInfo.ImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;

        initInfo.PipelineInfoMain.RenderPass = swapChain->GetRenderPass();
        initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&initInfo);
    }

    void App::ShutdownImGui()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}