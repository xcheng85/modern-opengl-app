#include <format>
#include <iostream>
#include "application.h"

using namespace std;
namespace SharedUtils
{
    Application::Application()
    {
        cout << format("--> Application::Application") << std::endl;
        cout << format("<-- Application::Application") << std::endl;
    }
}
