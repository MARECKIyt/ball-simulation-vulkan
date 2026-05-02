#pragma once
#include "window.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "device.hpp"
#include "model.hpp"

#include <memory>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace engine
{
    class App
    {
        public:
        static constexpr int WIDTH = 1536;
        static constexpr int HEIGHT = 1024;
        App();
        ~App();
        App(const App&) = delete;
        App &operator=(const App&) = delete;

        void Run();

        private:
        void LoadModels();
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void DrawFrame();
        void RecreateSwapChain();
        void RecordCommandBuffer(int imageIndex);
        
        void InitImGui();
        void ShutdownImGui();

        Window window{WIDTH, HEIGHT, "Gra"};
        Device device{window};
        std::unique_ptr<SwapChain> swapChain;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<Model> modelCirc4;
        std::unique_ptr<Model> modelCirc8;
        std::unique_ptr<Model> modelCirc16;
        std::unique_ptr<Model> modelCirc32;
        std::unique_ptr<Model> modelCirc64;
    };
}