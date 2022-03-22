// Name        : iconviewer.cc
// Author      : Alan Buckley
// Copyright   : This is released under the same COPYRIGHT as TBX
// Description : Simple application to show using a TileView to display
//             : Icons in a "filer" like way.
//
#include "tbx/application.h"
#include "tbx/autocreate.h"
#include "iconviewer.h"

int main()
{
	tbx::Application icon_view_app("<IconView$Dir>");

	icon_view_app.set_autocreate_listener("IconView", new tbx::AutoCreateClass<IconViewer>());

	icon_view_app.run();

	return 0;
}
