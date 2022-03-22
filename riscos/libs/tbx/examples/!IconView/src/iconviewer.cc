// Name        : iconviewer.cc
// Author      : Alan Buckley
// Copyright   : This is released under the same COPYRIGHT as TBX
// Description : Simple application to show using a TileView to display
//             : Icons in a "filer" like way.
//

#include "iconviewer.h"
#include "tbx/path.h"
#include "tbx/sprite.h"
#include "tbx/menu.h"

#include <sstream>

using namespace tbx;

IconViewer::IconViewer(Window window) :
	_icons(&_view),
	_name_provider(&_icons, &IconData::name),
	_sprite_provider(&_icons, &IconData::sprite_name),
	_icon_renderer(&_name_provider, &_sprite_provider, false),
	_view(window, &_icon_renderer),
	_select_all(&_view),
	_clear_selection(&_view),
	_remove_selected(this, &IconViewer::remove_selected),
	_clear_menu_selection(&_view),
	_update_selected_item(&_icons, &_selection)
{
	window.add_loader(this);
	_view.selection(&_selection);
	window.add_command(1, &_select_all);
	window.add_command(2, &_clear_selection);
	window.add_command(3, &_remove_selected);

	// Setup so menu selects an icon which is cleared when the menu is closed
	_view.menu_selects(true);
	window.menu().add_has_been_hidden_listener(&_clear_menu_selection);
	// Update text of menu item to reference current selection when the menu is
	// shown.
	window.menu().add_about_to_be_shown_listener(&_update_selected_item);
}

IconViewer::~IconViewer()
{
}

// Just grab the file name, but don't actually do anything else
bool IconViewer::load_file(tbx::LoadEvent &event)
{
   if (event.from_filer())
   {
	   Path path(event.file_name());
	   WimpSprite ws(event.file_type(), path.leaf_name());

	   unsigned int index = _view.insert_index(event.destination_point());
	   _icons.insert(index, IconData(path.leaf_name(), ws.name()));
   }

   return false;
}

// Remove the selected items from the view.
void IconViewer::remove_selected()
{
	while (_selection.any())
	{
		_icons.erase(_selection.last());
	}
}

/**
 * Update menu item with selection name when selection is shown
 */
void IconViewer::UpdateSelected::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	Menu icon_menu = event.id_block().self_object();
	MenuItem sel_item = icon_menu.item(5);
	std::string text;
	if (_selection->empty())
	{
		text="Icon ''";
		sel_item.fade(true);
	} else
	{
		if (_selection->one())
		{
			text = "Icon '";
			text += _items->item(_selection->first()).name();
			text += "'";
		} else
		{
			text = "Selection";
		}
		sel_item.fade(false);
	}
	sel_item.text(text);
}
