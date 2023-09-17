#include <format>
#include "platform.h"
#include "window.h"
#include "application.h"

using namespace std;
namespace SharedUtils
{
    void IPlatform::main_loop()
    {
        while (!_window->ShouldClose())
        {
        }
    }

    UnixPlatform::UnixPlatform(std::shared_ptr<IWindow> window, std::shared_ptr<IApplication> app)
    {
        cout << format("--> UnixPlatform::UnixPlatform") << std::endl;
        // cannot initialzie parents member in the ctor init list
        this->_window = window;
        create_window();
        cout << format("<-- UnixPlatform::UnixPlatform") << std::endl;
    }

    void UnixPlatform::create_window()
    {
    }
}
