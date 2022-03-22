
#include "tbx/application.h"
#include "tbx/autocreatelistener.h"
#include "StopWatchwnd.h"

using namespace std;

int main()
{
   tbx::Application my_app("<StopWatch$Dir>");

   StopWatchWnd window_handler;

   my_app.set_autocreate_listener("StopWatch", &window_handler);

   my_app.run();

   return 0;
}
