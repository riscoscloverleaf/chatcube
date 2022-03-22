/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010 Alan Buckley   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * itemview.h
 *
 *  Created on: 24 Mar 2010
 *      Author: alanb
 */

#ifndef TBX_ITEMVIEW_H_
#define TBX_ITEMVIEW_H_

#include "../window.h"
#include "../mouseclicklistener.h"
#include "../margin.h"
#include "../draghandler.h"
#include "../safelist.h"
#include "../hasbeenhiddenlistener.h"
#include "../scrollrequestlistener.h"
#include "itemrenderer.h"
#include "selection.h"

namespace tbx {

namespace view {

/**
 * Listener for clicks on the item view
 */
class ItemViewClickListener;

/**
 * Base class for views that show an indexed list of items.
 *
 * The view does not maintain the list of items itself, it must
 * be told when the list of items are changed using the inserted,
 * removed and changed methods.
 *
 * The items are rendered using the ItemRenderer passed to the constructor.
 */
class ItemView :
	public RedrawListener,
	public ScrollRequestListener,
	public MouseClickListener,
	public SelectionListener
{
protected:
	Window _window; ///< Window displaying this view
	Margin _margin; ///< Margin around areas displaying items
	Selection *_selection; ///< Selection model
	/// Listeners for a mouse click on the view
	SafeList<ItemViewClickListener> *_click_listeners;
	unsigned int _count; ///< Number of items displayed in this view

	/**
	 * Internal view flags enum, can be one or more of the following
	 */
	enum ViewFlags {NONE = 0,       ///< No flags size
					AUTO_SIZE = 1,  ///< Autosizing on
					SELECT_DRAG = 2, ///< Select by dragging box allowed
					SELECT_DRAG_ON_ITEM = 4, ///< Select by dragging box starting on an item allowed
					SELECT_MENU = 8, ///< Menu button selects item if no others selected
					AUTO_SIZE_CHECKED = 0x100, ///< Removing or changing called
					WANT_AUTO_SIZE = 0x200,    ///< Need to auto size on removed called
					LAST_SELECT_MENU = 0x400,   ///< Last selection was caused by menu button
					NO_UPDATES = 0x800  ///< Do not update window
				   };
	unsigned int _flags; ///< Internal view flags

public:
	ItemView(Window window);
	virtual ~ItemView();


	/**
	 * Constant for invalid index. Used as a result from
	 * hit testing.
	 */
	static const unsigned int NO_INDEX = -1;

	/**
	 * Return the Window that this item view is attached
	 * to.
	 */
	tbx::Window &window() {return _window;}


	/**
	 * Return the margin around the data in the window.
	 */
	const tbx::Margin &margin() const {return _margin;}
	void margin(const tbx::Margin &margin);

	/**
	 * Set to true to resize the the view automatically
	 * when an item is added/removed or changed.
	 *
	 * Derived classes should set the AUTO_SIZE flag and when set on
	 * should recalculate the size for the existing items.
	 */
	virtual void auto_size(bool on) = 0;

	/**
	 * Returns true if auto size is on
	 */
	bool auto_size() const {return ((_flags & AUTO_SIZE)!= 0);}

	/**
	 * Enable/disable updating of attached window
	 *
	 * After turning on updates - a call to update_window_extents to refresh the window
	 *
	 * @param updates true to turn on updates (the default), false to turn them off
	 */
	void updates_enabled(bool updates) {if (updates) _flags &= ~NO_UPDATES; else _flags |= NO_UPDATES;}

	/**
	 * Check if updates of the attached window are enabled
	 *
	 * @returns true if updates are enabled
	 */
	bool updates_enabled() const {return ((_flags & NO_UPDATES) == 0);}

	/**
	 * Returns number of items being displayed
	 */
	unsigned int count() const {return _count;}

	void selection(Selection *selection);

	/**
	 * Get the selection model used for selecting items in the list
	 *
	 * @returns selection model or 0 if selection is not allowed
	 */
	Selection *selection() const {return _selection;}
	/**
	 * Get the selection model used for selecting items in the list
	 *
	 * @returns selection model or 0 if selection is not allowed
	 */
	Selection *selection() {return _selection;}

	void add_click_listener(ItemViewClickListener *listener);
	void remove_click_listener(ItemViewClickListener *listener);

	void allow_drag_selection(bool on, bool on_item = false);

	void menu_selects(bool on);
	bool menu_selects() const;
	void clear_menu_selection();
	bool last_selection_menu() const;

	/**
	 * Subclasses must override the redraw event to draw
	 * the items in the view.
	 */
	virtual void redraw(const tbx::RedrawEvent &event) = 0;

	// Scroll request override
	virtual void scroll_request(const tbx::ScrollRequestEvent &event);

	// Mouse click listener override
	virtual void mouse_click(tbx::MouseClickEvent &event);

	// Selection listener override
	virtual void selection_changed(const SelectionChangedEvent &event);

	/**
	 * Called to update the window extent needed to contain
	 * this view.
	 */
	virtual void update_window_extent() = 0;

	/**
	 * Called to refresh the entire views contents
	 */
	virtual void refresh() = 0;

	/**
	 * Inform the view that items have been inserted.
	 *
	 * @param where location for insertion
	 * @param how_many number of items inserted
	 */
	virtual void inserted(unsigned int where, unsigned int how_many) = 0;

	/**
	 * Inform the view that items are about to be removed.
	 *
	 * This call is optional, but it stops the need for a full
	 * scan of the list when auto size is on.
	 */
	virtual void removing(unsigned int where, unsigned int how_many) {}

	/**
	 * Inform the view that items have been removed.
	 *
	 * @param where location for removal
	 * @param how_many number of items removed
	 */
	virtual void removed(unsigned int where, unsigned int how_many) = 0;

	/**
	* Inform the view that items are about to be changed
	*
	* This call is optional, but it stops the need for a full
	* scan every time something an item is changed when auto size is on.
	*
	* @param where location items will be changed from
	* @param how_many number of items that will be changed
	*/
	virtual void changing(unsigned int where, unsigned int how_many) {};

	/**
	 * Inform the view that items have been changed.
	 *
	 * @param where location of first change
	 * @param how_many number of items changed
	 */
	virtual void changed(unsigned int where, unsigned int how_many) = 0;

	/**
	 * Inform the view that all the data has been cleared
	 */
	virtual void cleared() = 0;

	/**
	 * Return the index for inserting an item from a screen
	 * point.
	 */
	virtual unsigned int insert_index(const Point &scr_pt) const = 0;

	/**
	 * Return the index under the screen point, does not check
	 * if it hits content of the item renderer.
	 *
	 * @param scr_pt the point to test
	 * @return the index of the item below the point or NO_INDEX if the point is
	 *         not over an item.
	 */
	virtual unsigned int screen_index(const Point &scr_pt) const = 0;

	/**
	 * Return the index under the screen point, checks
	 * if it hits content of the item renderer.
	 *
	 * Returns: NO_INDEX if doesn't hit content
	 */
	virtual unsigned int hit_test(const Point &scr_pt) const = 0;

	/**
	 * Get bounding box of the given index in work area coordinates
	 */
	virtual void get_bounds(BBox &bounds, unsigned int index) const = 0;

	/**
	 * Get bounding box of the range of indices in work area coordinates
	 */
	virtual void get_bounds(BBox &bounds, unsigned int first, unsigned int last) const = 0;

protected:
	/**
	 * Override this method to process selection by dragging.
	 *
	 * allow_drag_selection must be set for this to be called.
	 *
	 * @param drag_box Final bounding box of drag
	 * @param adjust adjust button was used for selection
	 */
	virtual void process_drag_selection(const BBox &drag_box, bool adjust) {};

	// Helper methods
	void process_mouse_click(int index, MouseClickEvent &event);

private:
	/// @cond INTERNAL
	/**
	 * Class to handle selecting items after a rubber box drag
	 */
	class Selector : public tbx::DragHandler
	{
		ItemView *_me;
		bool _adjust;
	public:
		Selector(ItemView *me, bool adjust) : _me(me), _adjust(adjust) {};

		virtual void drag_finished(const tbx::BBox &final);
		virtual void drag_cancelled();
	};
	/// @endcond

};

/**
 * Event for click on ItemView
 */
class ItemViewClickEvent
{
	ItemView &_source;
	unsigned int _index;
	const MouseClickEvent &_click_event;

public:
	/**
	 * Constructor
	 */
	ItemViewClickEvent(ItemView &source, unsigned int index, const MouseClickEvent &click_event) :
		_source(source), _index(index), _click_event(click_event) {}

	/**
	 * Return item view this event occurred on
	 */
	ItemView &source() const {return _source;}

	/**
	 * Constant for index when there was no index hit
	 */
	static const unsigned int NO_INDEX = -1;

	/**
	 * Return true if an item was hit
	 */
	bool item_hit() const {return (_index != NO_INDEX);}

	/**
	 * Returns index click occurred on or NO_INDEX if click did not hit an item
	 */
	unsigned int index() const {return _index;}

	/**
	 * Returns original mouse click event on the window containing this item view
	 */
	const MouseClickEvent &click_event() const {return _click_event;}
};

/**
 * Listener for clicks on the item view
 */
class ItemViewClickListener : public Listener
{
public:
	ItemViewClickListener() {}
	virtual ~ItemViewClickListener() {}

	/**
	 * Method called when an item in the view is clicked
	 *
	 * @param event details of item hit (if any)
	 */
	virtual void itemview_clicked(const ItemViewClickEvent &event) = 0;
};

/**
 * Helper class to clear selection when an object has been
 * hidden if the selection was made by the menu button.
 *
 * Add to the object with it's add_has_been_hidden_listener.
 * Usually used on the menu shown from a view window.
 */
class ItemViewClearMenuSelection : public tbx::HasBeenHiddenListener
{
	ItemView *_view;
public:
	/**
	 * Constructor for the given item view
	 *
	 * @param iv ItemView this updates menu selection for
	 */
	ItemViewClearMenuSelection(ItemView *iv) : _view(iv) {};

	/**
	 * Clears the menu selection when the object is hidden
	 *
	 * @param hidden_event ignored
	 */
	virtual void has_been_hidden(const EventInfo &hidden_event)
	{
		_view->clear_menu_selection();
	}
};


}

}

#endif /* ITEMVIEW_H_ */
