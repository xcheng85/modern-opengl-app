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
            // try
            // {
            //     // Load the requested app
            //     if (app_requested())
            //     {
            //         if (!start_app())
            //         {
            //             LOGE("Failed to load requested application");
            //             return ExitCode::FatalError;
            //         }

            //         // Compensate for load times of the app by rendering the first frame pre-emptively
            //         timer.tick<Timer::Seconds>();
            //         active_app->update(0.01667f);
            //     }

            //     update();

            //     if (active_app && active_app->should_close())
            //     {
            //         std::string id = active_app->get_name();
            //         on_app_close(id);
            //         active_app->finish();
            //     }

            //     window->process_events();
            // }
            // catch (std::exception &e)
            // {
            //     LOGE("Error Message: {}", e.what());
            //     LOGE("Failed when running application {}", active_app->get_name());

            //     on_app_error(active_app->get_name());

            //     if (app_requested())
            //     {
            //         LOGI("Attempting to load next application");
            //     }
            //     else
            //     {
            //         return ExitCode::FatalError;
            //     }
            // }
        }
    }

    UnixPlatform::UnixPlatform(std::shared_ptr<IWindow> window, std::shared_ptr<IApplication> application)
    {
        cout << format("--> UnixPlatform::UnixPlatform") << std::endl;
        // cannot initialzie parents member in the ctor init list
        this->_window = window;
        this->_application = application;
        cout << format("<-- UnixPlatform::UnixPlatform") << std::endl;
    }

    UnixPlatform::~UnixPlatform()
    {
        
    }
}
