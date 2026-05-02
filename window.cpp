#include "window.hpp"
#include <stdexcept>

namespace engine
{
    Window::Window(int w, int h, std::string name) : width{w}, heigth{h}, windowName{name}
    {
        InitWindow();
    }

    Window::~Window()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::InitWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // aby się nie odpaliło w trybie opengl
        glfwWindowHint(GLFW_RESIZABLE, 1);

        window = glfwCreateWindow(width, heigth, windowName.c_str(), nullptr, nullptr); // pierwszy nullptr to tryb okna
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, FrameBufferResizeCallback);
    }

    void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != 0)
        {
            throw std::runtime_error("failed to create window surface");
        }
    }

    void Window::FrameBufferResizeCallback(GLFWwindow *windowW, int width, int height)
    {
        auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(windowW));
        window->frameBufferResized = true;
        window->width = width;
        window->heigth = height;
    }
}