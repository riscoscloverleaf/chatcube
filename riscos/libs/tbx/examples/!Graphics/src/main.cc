// main.cc
// Alan Buckley - Nov 2010
// This is released under the same COPYRIGHT as TBX
//
// Main entry point for the program

#include "tbx/application.h"
#include "tbx/autocreatelistener.h"
#include "tbx/matchlifetime.h"
#include "GraphicsWnd.h"
#include "SaveAsSprite.h"

using namespace std;

/**
 * Main entry point, creates application and sets
 * up initial toolbox event handlers.
 */
int main()
{
   tbx::Application my_app("<Graphics$Dir>");

   // Handler to draw the window
   GraphicsWnd window_handler;
   my_app.set_autocreate_listener("Window", &window_handler);

   // Handler for save as sprite
   tbx::MatchLifetime<SaveAsSprite> mlt_save_as_sprite("SaveSprite");

   my_app.run();

   return 0;
}
