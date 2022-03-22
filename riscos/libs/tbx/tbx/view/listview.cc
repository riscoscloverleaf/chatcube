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
 * listview.cc
 *
 *  Created on: 25 Mar 2010
 *      Author: alanb
 */

#include "listview.h"
#include "../osgraphics.h"
#include <algorithm>

namespace tbx {

namespace view {


/**
 * Constructor
 */
ListView::ListView(tbx::Window window, ItemRenderer *item_renderer)
    : ItemView(window),
      _item_renderer(item_renderer),
      _height(0),
      _width(0)
{
}

/**
 * Destructor
 */
ListView::~ListView()
{
}

/**
 * Change the item renderer.
 *
 * If there are items already defined it will
 * resize or refresh the view depending on if
 * auto_size is set.
 */
void ListView::item_renderer(ItemRenderer *ir)
{
	_item_renderer = ir;
	if (_count)
	{
		if (_flags & AUTO_SIZE)
		{
			// Force resize
			auto_size(false);
			auto_size(true);
		} else
		{
			refresh();
		}
	}
}

/**
 * Change the row height.
 *
 * If the row height is 0. It will take the height from
 * the first item in the list. This is how the view
 * starts up.
 */
void ListView::row_height(unsigned int height)
{
	if (height == 0 && _count && _item_renderer != 0)
	{
		height = _item_renderer->height(0);
	}
	if (height != _height)
	{
		_height = height;
		if (_count && updates_enabled())
		{
			update_window_extent();
			refresh();
		}
	}
}

/**
 * Turn on or off auto sizing.
 *
 * When auto sizing is on the item width is checked whenever an
 * item is added, changed or removed.
 *
 * To prevent a scan of the whole list on removal call the
 * removing method before actually doing a removal.
 */
void ListView::auto_size(bool on)
{
	if (on)
	{
		if ((_flags & AUTO_SIZE)==0)
		{
			size_to_width();
			_flags |= AUTO_SIZE;
		}
	} else
	{
		_flags &= ~AUTO_SIZE;
	}
}

/**
 * Set the width.
 *
 * Setting the width automatically turns of auto sizing
 */
void ListView::width(unsigned int width)
{
	_flags &= ~AUTO_SIZE;
	if (width != _width)
	{
		_width = width;
		if (_count && updates_enabled())
		{
			update_window_extent();
		}
	}
}

/**
 * Measure the maximum width for a range of rows.
 *
 * Measures from first to end-1
 *
 *@param from first row to measure (inclusive)
 *@param end last row to measure (not inclusive)
 */
unsigned int ListView::max_width(unsigned int from, unsigned int end)
{
	unsigned int max_width = 0;
	for (unsigned int row = 0; row < end; row++)
	{
		unsigned int item_width = _item_renderer->width(row);
		if (item_width > max_width) max_width = item_width;
	}
	return max_width;
}

/**
 * Check if width has been reduced to less then _width
 *
 * Measures from first to end-1
 *
 *@param from first row to measure (inclusive)
 *@param end last row to measure (not inclusive)
 *@return maximum width in the range.
 */
unsigned int ListView::check_width(unsigned int from, unsigned int end)
{
	unsigned int max_width = 0;
	for (unsigned int row = 0; row < end; row++)
	{
		unsigned int item_width = _item_renderer->width(row);
		if (item_width > max_width) max_width = item_width;
		if (item_width == _width) break; // Don't need to do any more
	}
	return max_width;
}


/**
 * Size width to match content.
 * Does nothing if there is no content
 *
 * Using this function to set the width automatically turns off auto sizing
 */
void ListView::size_to_width(unsigned int min/* = 0*/, unsigned int max /*= 0xFFFFFFFF*/)
{
	_flags &= ~AUTO_SIZE;
	if (_item_renderer == 0 || _count == 0) return;
	unsigned int new_width = max_width(0, _count);
	if (new_width < min) new_width = min;
	if (new_width > max) new_width = max;
	if (new_width != _width)
	{
		width(new_width);
	}
}

/**
 * Redraw the List view
 */
void ListView::redraw(const tbx::RedrawEvent & event)
{
	BBox work_clip = event.visible_area().work(event.clip());

	unsigned int first_row = (-work_clip.max.y - _margin.top) / _height;
	unsigned int last_row =  (-work_clip.min.y - _margin.top) / _height;

	if (first_row < 0) first_row = 0;
	if (last_row < 0) return; // Nothing to draw

	if (first_row >= _count) return; // Nothing to draw
	if (last_row >= _count) last_row = _count - 1;

    ItemRenderer::Info cell_info(event);

	cell_info.bounds.max.y = -first_row * _height - _margin.top;
	cell_info.screen.y = event.visible_area().screen_y(cell_info.bounds.max.y);

	cell_info.screen.x = event.visible_area().screen_x(_margin.left);
	cell_info.bounds.min.x = _margin.left;
    cell_info.bounds.max.x = _margin.left + _width;

    for (unsigned int row = first_row; row <= last_row; row++)
    {
        cell_info.bounds.min.y = cell_info.bounds.max.y - _height;
        cell_info.screen.y -= _height;
        cell_info.index = row;

        cell_info.selected = (_selection != 0 && _selection->selected(row));

        if (cell_info.selected)
        {
        	// Fill background with selected colour
    		OSGraphics g;
    		g.foreground(Colour::black);
    		g.fill_rectangle(cell_info.screen.x, cell_info.screen.y,
    				cell_info.screen.x + _width -1,
    				cell_info.screen.y + _height - 1);
        }

        _item_renderer->render(cell_info);

        cell_info.bounds.max.y = cell_info.bounds.min.y;
    }
}

/**
 * Update the Window extent after a change in size.
 */
void ListView::update_window_extent()
{
	if (!updates_enabled()) return;

	int width = _width + _margin.left + _margin.right;
	int height = _count * _height + _margin.top + _margin.bottom;

	WindowState state;
	_window.get_state(state);
	if (width < state.visible_area().bounds().width())
		width = state.visible_area().bounds().width();

	if (height < state.visible_area().bounds().height())
	   height = state.visible_area().bounds().height();

	BBox extent(0,-height, width, 0);
	_window.extent(extent);
}

/**
 * Refresh the whole report view.
 *
 * It should also be called if all the data changes.
 */

void ListView::refresh()
{
	if (updates_enabled())
	{
		BBox all(_margin.left, -_margin.top - _count * _height,
				_margin.left + _width, -_margin.top);
		_window.force_redraw(all);
	}
}

/**
 * Inform the view that items have been inserted.
 *
 * @param where location for insertion
 * @param how_many number of items inserted
 */
void ListView::inserted(unsigned int where, unsigned int how_many)
{
	if (_item_renderer != 0)
	{
		// Automatically set the height if not set
		if (_height == 0) _height = _item_renderer->height(where);
		if ((_flags & AUTO_SIZE))
		{
			// Auto size
			unsigned int width = max_width(where, where + how_many);
			if (width > _width) _width = width;
		}
	}
	_count += how_many;
	if (updates_enabled()) update_window_extent();

	if (_selection) _selection->inserted(where, how_many);

	//TODO: Use window block copy to update
	if (updates_enabled())
	{
		BBox dirty(_margin.left,
			- _count * _height - _margin.top,
			_width + _margin.left,
			- where * _height - _margin.top);
		_window.force_redraw(dirty);
	}
}

/**
* Inform the view that items are about to be removed.
*
* This call is optional, but it stops the need for a full
* scan every time something is removed when auto size is on.
*
* @param where location items will be removed from
* @param how_many number of items that will be removed
*/
void ListView::removing(unsigned int where, unsigned int how_many)
{
	_flags |= AUTO_SIZE_CHECKED;
	if (_flags & AUTO_SIZE)
	{
		for (unsigned int row = where; row < where + how_many; row++)
		{
			unsigned int item_width = _item_renderer->width(row);
			if (item_width == _width)
			{
				_flags |= WANT_AUTO_SIZE;
				break;
			}
		}
	}
}

/**
 * Inform the view that items have been removed.
 *
 * @param where location for removal
 * @param how_many number of items removed
 */
void ListView::removed(unsigned int where, unsigned int how_many)
{
	if ((_flags & AUTO_SIZE) && !(_flags & AUTO_SIZE_CHECKED))
	{
		_flags |= WANT_AUTO_SIZE;
	}
	_flags &= ~ AUTO_SIZE_CHECKED;
	_count -= how_many;
	unsigned int old_width = _width;
	if (_flags & WANT_AUTO_SIZE)
	{
		_flags &= ~WANT_AUTO_SIZE;
		if (_count == 0) _width = 0;
		else _width = check_width(0, _count);
	}
	//TODO: Use window block copy to update
	BBox dirty(_margin.left,
		-_count * _height - _margin.top,
		old_width + _margin.left,
		-where * _height - _margin.top);
	if (_selection) _selection->removed(where, how_many);
	if (updates_enabled())
	{
		update_window_extent();
		_window.force_redraw(dirty);
	}
}

/**
* Inform the view that items are about to be changed
*
* This call is optional, but it stops the need for a full
* scan every time something an item is changed when auto size is on.
*
* @param where location items will be changed from
* @param how_many number of items that will be changed
*/
void ListView::changing(unsigned int where, unsigned int how_many)
{
	_flags |= AUTO_SIZE_CHECKED;
	if (_flags & AUTO_SIZE)
	{
		for (unsigned int row = where; row < where + how_many; row++)
		{
			unsigned int item_width = _item_renderer->width(row);
			if (item_width == _width)
			{
				_flags |= WANT_AUTO_SIZE;
				break;
			}
		}
	}
}

/**
 * Inform the view that items have been changed.
 *
 * @param where location of first change
 * @param how_many number of items changed
 */
void ListView::changed(unsigned int where, unsigned int how_many)
{
	bool may_shrink = true;
	if ((_flags & AUTO_SIZE))
	{
		if  (_flags & AUTO_SIZE_CHECKED)
		{
			if ((_flags & WANT_AUTO_SIZE) == 0) may_shrink = false;
		}

		unsigned int new_width = (may_shrink) ? 0 : _width;
		for (unsigned int row = where; row < where + how_many; row++)
		{
			unsigned int item_width = _item_renderer->width(row);
			if (item_width > new_width) new_width = item_width;
		}

		if (new_width < _width)
		{
			unsigned int check;
			if (where > 0)
			{
				check = check_width(0, where);
				if (check > new_width) new_width = check;
			}
			if (new_width != _width && where+how_many <=_count)
			{
				check = check_width(where+how_many-1, _count);
				if (check > new_width) new_width = check;
			}
		}

		if (new_width != _width)
		{
			width(new_width);
			_flags |= AUTO_SIZE;
		}
	}
	_flags &= ~(AUTO_SIZE_CHECKED | WANT_AUTO_SIZE);

	if (updates_enabled())
	{
		int last_row = where + how_many;
		BBox dirty(_margin.left,
			-last_row * _height - _margin.top,
			_width + _margin.left,
			-where * _height - _margin.top);
		_window.force_redraw(dirty);
	}
}

/**
 * Whole view has been cleared
 */
void ListView::cleared()
{
	if (_count)
	{
		refresh();
		_count = 0;
		if (_flags & AUTO_SIZE) _width = 0;
		if (_selection) _selection->clear();
		if (updates_enabled())
		{
			update_window_extent();
			WindowState state;
			_window.get_state(state);
			Point &scroll = state.visible_area().scroll();
			if (scroll.x != 0 || scroll.y != 0)
			{
				scroll.x = 0;
				scroll.y = 0;
				_window.open_window(state);
			}
		}
	}
}

/**
 * Return the index for inserting an item from a screen
 * point.
 */
unsigned int ListView::insert_index(const Point &scr_pt) const
{
	WindowState state;
	_window.get_state(state);

	int work_y = state.visible_area().work_y(scr_pt.y) + _margin.top;
	unsigned int row = (-work_y + _height/2) / _height;

	if (row < 0) row = 0;
	else if (row > _count) row = _count;

	return row;
}

/**
 * Return the index under the screen point, does not check
 * if it hits content of the item renderer.
 */
unsigned int ListView::screen_index(const Point &scr_pt) const
{
	WindowState state;
	_window.get_state(state);

	unsigned int row;

	Point work_pt = state.visible_area().work(scr_pt);
	if (work_pt.x < _margin.left || work_pt.x > _margin.left + (int)_width
		|| work_pt.y > -_margin.top)
	{
		row = NO_INDEX;
	} else
	{
		row = (-work_pt.y - _margin.top) / _height;
		if (row >= _count) row = NO_INDEX;
	}

	return row;
}

/**
 * Return the index under the screen point, checks
 * if it hits content of the item renderer.
 *
 * Returns: NO_INDEX if doesn't hit content
 */
unsigned int ListView::hit_test(const Point &scr_pt) const
{
	unsigned int index;
	if (_item_renderer == 0 || _count == 0)
	{
		index = NO_INDEX;
	} else
	{
		WindowState state;
		_window.get_state(state);

		Point work_pt = state.visible_area().work(scr_pt);
		if (work_pt.x < _margin.left || work_pt.x > _margin.left + (int)_width
			|| work_pt.y > -_margin.top)
		{
			index= NO_INDEX;
		} else
		{
			index = (-work_pt.y - _margin.top) / _height;
			if (index >= _count) index = NO_INDEX;
		}
		if (index != NO_INDEX)
		{
			BBox bounds;
			get_bounds(bounds, index);
			Point in_item;
			in_item.x = work_pt.x - bounds.min.x;
			in_item.y = work_pt.y - bounds.min.y;
			if (!_item_renderer->hit_test(index, Size(_width, _height), in_item))
				index = NO_INDEX;
		}
	}

	return index;
}

/**
 * Get bounding box of the given index in work area coordinates
 */
void ListView::get_bounds(BBox &bounds, unsigned int index) const
{
	bounds.min.x = _margin.left;
	bounds.max.x = bounds.min.x + _width;
	bounds.max.y = -_margin.top - index * _height;
	bounds.min.y = bounds.max.y - _height;
}

/**
 * Get bounding box of the range of indices in work area coordinates
 */
void ListView::get_bounds(BBox &bounds, unsigned int first, unsigned int last) const
{
	bounds.min.x = _margin.left;
	bounds.max.x = bounds.min.x + _width;
	bounds.max.y = -_margin.top - first * _height;
	bounds.min.y =  -_margin.top - (last+1) * _height;
}

/**
 * Override this method to process selection by dragging.
 *
 * allow_drag_selection must be set for this to be called.
 *
 * @param drag_box Final bounding box of drag
 * @param adjust adjust button was used for selection
 */
void ListView::process_drag_selection(const BBox &drag_box, bool adjust)
{
	WindowState state;
	_window.get_state(state);

	Point first_pt(state.visible_area().work(drag_box.max));
	Point last_pt(state.visible_area().work(drag_box.min));

	if (first_pt.y < last_pt.y)
	{
		std::swap<Point>(first_pt, last_pt);
	}
	first_pt.y += _margin.top;
	last_pt.y += _margin.top;

	unsigned int first = -first_pt.y / _height;
	if (first >= _count) return;
	unsigned int last = -last_pt.y / _height;
	if (last < 0) return;
	if (last >= _count) last = _count - 1;

	bool first_top = ((-first_pt.y  - (_height * first)) <= _height/2);
	bool last_top = ((-last_pt.y - (_height * last)) <= _height/2);

	Size line_size(_width, _height);

	if (!first_top)
	{
		first_pt.x -= _margin.left;
		if (_item_renderer->hit_test(first, line_size, first_pt) == NO_INDEX) first++;
	}
	if (last_top)
	{
		last_pt.x -= _margin.left;
		if (_item_renderer->hit_test(last, line_size, last_pt) == NO_INDEX) last--;
	}

	if (first <= last)
	{
		if (adjust) _selection->toggle(first, last);
		else _selection->select(first, last);
	}
}

// End of namespaces
}
}
