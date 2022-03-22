// Name        : iconviewer.cc
// Author      : Alan Buckley
// Copyright   : This is released under the same COPYRIGHT as TBX
// Description : Simple application to show using a TileView to display
//             : Icons in a "filer" like way.
//

#ifndef ICONVIEWER_H_
#define ICONVIEWER_H_

#include "tbx/window.h"
#include "tbx/view/tileview.h"
#include "tbx/view/iconitemrenderer.h"
#include "tbx/view/viewitems.h"
#include "tbx/view/selectcommands.h"
#include "tbx/abouttobeshownlistener.h"
#include "tbx/loader.h"

#include <string>
#include <vector>

class IconViewer : tbx::Loader
{
private:
	/**
	 * Class to store data for the view
	 */
	class IconData
	{
	public:
		IconData(const std::string n, const std::string s) : _name(n), _sprite_name(s) {}

		/**
		 * Return the text to display in the icon view
		 */
		std::string name() const {return _name;}

		/**
		 * Return the name of the sprite to display in the icon view.
		 */
		std::string sprite_name() const {return _sprite_name;}

	private:
		std::string _name;
		std::string _sprite_name;
	};

	tbx::view::ViewItems<IconData> _icons;

	tbx::view::MethodItemViewValue<std::string, tbx::view::ViewItems<IconData>, IconData> _name_provider;
	tbx::view::MethodItemViewValue<std::string, tbx::view::ViewItems<IconData>, IconData> _sprite_provider;
	tbx::view::IconItemRenderer _icon_renderer;
	tbx::view::TileView _view;
	tbx::view::MultiSelection _selection;

	tbx::view::SelectAllCommand _select_all;
	tbx::view::ClearSelectionCommand _clear_selection;
	tbx::CommandMethod<IconViewer> _remove_selected;
	tbx::view::ItemViewClearMenuSelection _clear_menu_selection;

	/**
	 * Class to update selection menu item with name or "selection"
	 */
	class UpdateSelected : public tbx::AboutToBeShownListener
	{
		tbx::view::ViewItems<IconData> *_items;
		tbx::view::MultiSelection *_selection;
	public:
		UpdateSelected(tbx::view::ViewItems<IconData> *items, tbx::view::MultiSelection *sel) :
			_items(items), _selection(sel) {}
		virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	} _update_selected_item;


public:
	IconViewer(tbx::Window window);
	virtual ~IconViewer();

	virtual bool load_file(tbx::LoadEvent &event);

	void remove_selected();
};

#endif /* ICONVIEWER_H_ */
