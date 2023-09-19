#include <format>
#include <iostream>

#include <GLFW/glfw3.h>

#include "window.h"
#include "platform.h"

using namespace std;
namespace SharedUtils
{
    // unnamed namespace to avoid naming clash for function symbol such as:
    namespace
    {
        void error_callback(int error, const char *description)
        {
            cerr << format("GLFW Error (code {}): {}", error, description) << endl;
        }
    }

    void resize(GLFWwindow *window, int width, int height)
    {
        cout << format("--> GlfwWindow::resize") << endl;
        if (auto platform = reinterpret_cast<IPlatform *>(glfwGetWindowUserPointer(window)))
        {
            platform->resize(width, height);
        }
        cout << format("<-- GlfwWindow::resize") << endl;
    }

    void keyCB(GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/)
    {
        cout << format("--> GlfwWindow::keyCB") << endl;
    }

    void IWindow::usePlatform(const IPlatform* const p)
    {
        this->_platform = const_cast<IPlatform*>(p);
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

        glfwSetWindowSizeCallback(_window, resize);
        glfwSetKeyCallback(_window, keyCB);

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

    void GlfwWindow::Close()
    {
        // correlate with ShouldClose()
        glfwSetWindowShouldClose(_window, GLFW_TRUE);
    }

    void GlfwWindow::ProcessEvents()
    {
        glfwPollEvents();
    }
    

    void GlfwWindow::usePlatform(const IPlatform* const p)
    {
        IWindow::usePlatform(p);
        glfwSetWindowUserPointer(_window, const_cast<IPlatform*>(p));
    }
}
