#pragma once
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE _VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace engine
{
    class Window
    {
        public:
        Window(int w, int h, std::string name);
        ~Window();
        Window(const Window&) = delete;
        Window &operator=(const Window&) = delete;

        bool ShouldClose() {return glfwWindowShouldClose(window);}
        VkExtent2D GetExtent()
        {
            return {static_cast<uint32_t>(width), static_cast<uint32_t>(heigth)};
        }
        bool WasWindowResized()
        {
            return frameBufferResized;
        }
        void ResetWindowResizedFlag()
        {
            frameBufferResized = false;
        }

        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

        GLFWwindow* GetGLFWwindow() { return window; }

        private:
        static void FrameBufferResizeCallback(GLFWwindow *window, int with, int height);
        void InitWindow();
        int width;
        int heigth;
        bool frameBufferResized = false;
        std::string windowName;
        GLFWwindow *window;
    };
}