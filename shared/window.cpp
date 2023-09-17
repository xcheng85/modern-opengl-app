#include <format>
#include <iostream>

#include <GLFW/glfw3.h>

#include "window.h"

using namespace std;
namespace SharedUtils
{
    // unnamed namespace to be default
    namespace
    {
        void error_callback(int error, const char *description)
        {
            cerr << format("GLFW Error (code {}): {}", error, description) << endl;
        }
    }

    GlfwWindow::GlfwWindow()
    {
        cout << format("--> GlfwWindow::GlfwWindow") << endl;
        if (!glfwInit())
        {
            throw std::runtime_error("GLFW couldn't be initialized.");
        }

        glfwSetErrorCallback(error_callback);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        // to be extended
        _window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

        if (!_window)
        {
            throw std::runtime_error("Couldn't create glfw window.");
        }

        cout << format("<-- GlfwWindow::GlfwWindow") << endl;
    }

    GlfwWindow::~GlfwWindow()
    {
        glfwDestroyWindow(_window);

        glfwTerminate();
    }

    bool GlfwWindow::ShouldClose()
    {
        return glfwWindowShouldClose(_window);
    }
}
