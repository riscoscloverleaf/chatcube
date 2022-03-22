// Reverse main
//
// A.R.Buckley
//
// 15 Sept 2008
//
// Copyright is the same as for TBX (see COPYING in TBX directory for details)
//

#include "tbx/application.h"
#include "tbx/autocreatelistener.h"
#include "reverser.h"

using namespace std;

int main()
{
   // Setup the application and read in the resources
   tbx::Application my_app("<Reverse$Dir>");

   // A class to control the main window
   Reverser reverser;

   // Link the class to the "reverse" toolbox window when it is created
   my_app.set_autocreate_listener("reverse", &reverser);

   // Run the application
   my_app.run();

   return 0;
}
