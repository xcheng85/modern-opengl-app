#include <format>
#include <iostream>
#include "application.h"
#include "context.h"

using namespace std;
namespace SharedUtils
{
    Application::Application(std::unique_ptr<IContext> ctx)
        : _context(std::move(ctx))
    {
        cout << format("--> Application::Application") << std::endl;
        cout << format("<-- Application::Application") << std::endl;
    }

    Application::~Application()
    {
    }

    void Application::init()
    {
    }

    void Application::update()
    {
    }

    void Application::resize(const uint32_t width, const uint32_t height)
    {
        cout << format("--> Application::resize") << std::endl;

        this->_context->resize(width, height);

        cout << format("<-- Application::resize") << std::endl;
    }

    void Application::terminate()
    {
        cout << format("--> Application::terminate") << std::endl;
        cout << format("<-- Application::terminate") << std::endl;
    }
}
