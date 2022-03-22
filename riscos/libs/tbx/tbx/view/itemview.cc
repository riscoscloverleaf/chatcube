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
 * itemview.cc
 *
 *  Created on: 24 Mar 2010
 *      Author: alanb
 */

#include "itemview.h"

namespace tbx {

namespace view {

/**
 * Construct the item view with the given renderer.
 *
 * By default auto sizing is on a select dragging when using a multiple selection
 */
ItemView::ItemView(Window window) :
		_window(window),
		_selection(0),
		_click_listeners(0),
		_count(0),
		_flags(ItemView::AUTO_SIZE | ItemView::SELECT_DRAG )
{
	_window.add_redraw_listener(this);
	_window.add_scroll_request_listener(this);
}

/**
 * Destructor removed any listeners
 */
ItemView::~ItemView()
{
	_window.remove_redraw_listener(this);
	_window.remove_scroll_request_listener(this);
	if (_selection || _click_listeners)
	{
		if (_selection) _selection->remove_listener(this);
		_window.remove_mouse_click_listener(this);
		delete _click_listeners;
	}
}


/**
 * Set the selection model to use for the item view.
 *
 * @param selection Selection model to use for selecting items in the list
 * or 0 to disable selection.
 */
void ItemView::selection(Selection *selection)
{
	if (selection == _selection) return;

	if (_selection)
	{
		if (_count) _selection->clear();
		_selection->remove_listener(this);
		delete _selection;
		if (selection == 0 && _click_listeners == 0) _window.remove_mouse_click_listener(this);
	} else if (selection != 0)
	{
		if (_click_listeners == 0) _window.add_mouse_click_listener(this);
	}

	_selection = selection;
	if (_selection) _selection->add_listener(this);
}

/**
 * Add a listener for a click on the item view.
 *
 * The click listeners are run after any selection process.
 */
void ItemView::add_click_listener(ItemViewClickListener *listener)
{
	if (_click_listeners == 0)
	{
		_click_listeners = new SafeList<ItemViewClickListener>();
		if (_selection == 0) _window.add_mouse_click_listener(this);
	}
	_click_listeners->push_back(listener);
}

/**
 * Remove a listener for a click on the item view
 */
void ItemView::remove_click_listener(ItemViewClickListener *listener)
{
	if (_click_listeners != 0)
	{
		_click_listeners->remove(listener);
		if (_click_listeners->empty())
		{
			delete _click_listeners;
			_click_listeners = 0;
			if (_selection == 0) _window.remove_mouse_click_listener(this);
		}
	}
}

/**
 * Allow selection of multiple items by using a drag box
 *
 *@param on allow or disallow drag selection
 *@param on_item set to true to allow drag selection starting on an item
 */
void ItemView::allow_drag_selection(bool on, bool on_item /*= false*/)
{
	if (on)
	{
		_flags |= SELECT_DRAG;
		if (on_item) _flags |= SELECT_DRAG_ON_ITEM;
		else _flags &= ~SELECT_DRAG_ON_ITEM;
	} else
	{
		_flags &= ~(SELECT_DRAG | SELECT_DRAG_ON_ITEM);
	}
}

/**
 * Turn on/off selection of an item when the menu button
 * is pressed over an item and no other item is selected.
 *
 * Usually you would add the ItemViewClearMenuSelection as a has been
 * hidden listener to the menu on the views window so a menu selection
 * is removed when the menu is closed.
 *
 * @param on true to turn menu selection on, false to turn it off.
 */
void ItemView::menu_selects(bool on)
{
	if (on) _flags |= SELECT_MENU;
	else _flags &= ~SELECT_MENU;
}

/**
 * Returns true if the menu button selects an item
 */
bool ItemView::menu_selects() const
{
	return (_flags & SELECT_MENU)!= 0;
}

/**
 * Clear the current selection if it was made by the menu button
 */
void ItemView::clear_menu_selection()
{
	if (_flags & LAST_SELECT_MENU && _selection != 0)
	{
		_selection->clear();
	}
}

/**
 * Returns true if the last selection was made by the menu button
 */
bool ItemView::last_selection_menu() const
{
	return (_flags & LAST_SELECT_MENU) != 0;
}

/**
 * Change the margin
 */
void ItemView::margin(const tbx::Margin &margin)
{
	_margin = margin;
	update_window_extent();
	refresh();
}

/**
 * Process scroll requests.
 *
 * This allows page clicks on the scroll bar to take into account
 * the margins.
 *
 * It makes line clicks on the scroll bar move 32 units left/right
 * and 40 OS units up/down.
 *
 * The appropriate flag in the window must be set for this event
 * to be generated.
 *
 * Override this to customise the scrolling behaviour.
 */
void ItemView::scroll_request(const tbx::ScrollRequestEvent &event)
{
	tbx::WindowOpenInfo new_open = event.open_info();

	switch(event.x_scroll())
	{
	case tbx::ScrollRequestEvent::LEFT:
		new_open.visible_area().scroll().x -= 32;
		break;
	case tbx::ScrollRequestEvent::RIGHT:
		new_open.visible_area().scroll().x += 32;
		break;

	case tbx::ScrollRequestEvent::PAGE_LEFT:
		{
			int width = new_open.visible_area().bounds().width();
			width -= (_margin.left + _margin.right);
			new_open.visible_area().scroll().x -= width;
		}
		break;
	case tbx::ScrollRequestEvent::PAGE_RIGHT:
		{
			int width = new_open.visible_area().bounds().width();
			width -= (_margin.left + _margin.right);
			new_open.visible_area().scroll().x += width;
		}
		break;

	case tbx::ScrollRequestEvent::NONE_X:
		// Do nothing - stops compiler warning
		break;
	}

	switch(event.y_scroll())
	{
	case tbx::ScrollRequestEvent::DOWN:
		new_open.visible_area().scroll().y -= 40;
		break;
	case tbx::ScrollRequestEvent::UP:
		new_open.visible_area().scroll().y += 40;
		break;

	case tbx::ScrollRequestEvent::PAGE_DOWN:
		{
			int height = new_open.visible_area().bounds().height();
			height -= (_margin.top + _margin.bottom);
			new_open.visible_area().scroll().y -= height;
		}
		break;
	case tbx::ScrollRequestEvent::PAGE_UP:
		{
			int height = new_open.visible_area().bounds().height();
			height -= (_margin.top + _margin.bottom);
			new_open.visible_area().scroll().y += height;
		}
		break;

	case tbx::ScrollRequestEvent::NONE_Y:
		// Do nothing - stops compiler warning
		break;
	}

	_window.open_window(new_open);

}


/**
 * Mouse clicked on window containing list view.
 *
 * The version in this class processes the selection if
 * necessary and fires the ItemViewClickListeners.
 *
 * If it is overridden then this functionality will
 * be lost unless it is replaced or the ItemView
 * implementation called.
 */
void ItemView::mouse_click(MouseClickEvent &event)
{
	if (_selection || _click_listeners)
	{
		unsigned int index = hit_test(event.point());
		if (_selection)
		{
			if (event.is_menu())
			{
				if ((_flags & SELECT_MENU)
					 && index != NO_INDEX
					 && _selection->empty())
				{
					_selection->select(index);
					_flags |= LAST_SELECT_MENU;
				}
			} else if (event.is_select())
			{
				if (index == NO_INDEX) _selection->clear();
				else if (!_selection->selected(index))
				{
					_selection->set(index);
				}
			} else if (event.is_adjust() && index != NO_INDEX)
			{
				_selection->toggle(index);
			}

			if (_selection->type() == Selection::MULTIPLE)
			{
				if (event.is_select_drag() || event.is_adjust_drag())
				{
					if (((_flags & SELECT_DRAG) && index == NO_INDEX)
							|| (_flags & SELECT_DRAG_ON_ITEM))
					{
						_window.drag_rubber_box_local(event.point(), new Selector(this, event.is_adjust_drag()));
					}
				}
			}
		}
		if (_click_listeners)
		{
			process_mouse_click(index, event);
		}
	}
}

//! @cond INTERNAL
/**
 * Drag has been finished so select items in final box
 * and self destruct
 */
void ItemView::Selector::drag_finished(const BBox &final)
{
	_me->process_drag_selection(final, _adjust);
	delete this;
}

/**
 * Drag has been cancelled so self destruct
 */
void ItemView::Selector::drag_cancelled()
{
	delete this;
}

//! @endcond

/**
 * Helper function to process mouse clicks
 */
void ItemView::process_mouse_click(int index, MouseClickEvent &event)
{
	ItemViewClickEvent ev(*this, index, event);
	SafeList<ItemViewClickListener>::Iterator i(*_click_listeners);
	ItemViewClickListener *l;
	while ((l = i.next())!= 0)
	{
		l->itemview_clicked(ev);
	}
}

/**
 * Selection has changed event handling.
 *
 * The ItemView implementation forces a redraw of the items that have
 * had there selection changed.
 */
void ItemView::selection_changed(const SelectionChangedEvent &event)
{
	if (updates_enabled())
	{
		BBox bounds;
		get_bounds(bounds, event.first(), event.last());
		_window.force_redraw(bounds);
	}

	// Any other selection turns last select menu off
	_flags &= ~LAST_SELECT_MENU;
}


}
}
