// Name        : tbxmin.cc
// Author      : Alan Buckley
// Copyright   : This is released under the same COPYRIGHT as TBX
// Description : Minimal tbx application
//

#include "tbx/application.h"

int main()
{
    // Set up the application loading in the toolbox resources
	tbx::Application my_app("<TbxMin$Dir>");

    // Run the application
	my_app.run();

	return 0;
}
