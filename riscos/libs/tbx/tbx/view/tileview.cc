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
 * tileview.cc
 *
 *  Created on: 16 Apr 2010
 *      Author: alanb
 */

#include "tileview.h"

namespace tbx {

namespace view {

/**
 * Construct a tile view for specific Window using
 * the given item renderer.
 */
TileView::TileView(tbx::Window window, ItemRenderer *item_renderer) : ItemView(window),
	 _item_renderer(item_renderer),
	 _tile_size(1,1),
	 _cols_per_row(1)
{
	_window.add_open_window_listener(this);
}

TileView::~TileView()
{
	_window.remove_open_window_listener(this);
}

/**
 * Change the item renderer.
 *
 * If there are items already defined it will
 * resize or refresh the view depending on if
 * auto_size is set.
 */
void TileView::item_renderer(ItemRenderer *ir)
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
 * Set the size of a tile in the tile view.
 *
 * Calling this function automatically turns of auto sizing
 *
 * @param new_size the new size of a tile in the view
 */
void TileView::tile_size(tbx::Size new_size)
{
	_flags &= ~AUTO_SIZE;
	if (new_size != _tile_size)
	{
		_tile_size = new_size;
		update_window_extent();
		refresh();
	}
}

/**
 * Turn on or off auto sizing.
 *
 * When auto sizing is on the item size is checked whenever an
 * item is added, changed or removed.
 *
 * To prevent a scan of the whole list on removal call the
 * removing method before actually doing a removal.
 */
void TileView::auto_size(bool on)
{
	if (on)
	{
		if ((_flags & AUTO_SIZE)==0)
		{
			size_to_tiles();
			_flags |= AUTO_SIZE;
		}
	} else
	{
		_flags &= ~AUTO_SIZE;
	}
}

/**
 * Size  to match content.
 * Does nothing if there is no content
 *
 * Using this function to set the size automatically turns off auto sizing
 */
void TileView::size_to_tiles()
{
	_flags &= ~AUTO_SIZE;
	if (_item_renderer == 0 || _count == 0) return;
	tbx::Size new_size = max_size(0, _count);
	if (new_size != _tile_size)
	{
		tile_size(new_size);
	}
}

/**
 * Measure the maximum tile size for a range of items.
 *
 * Measures from first to end-1
 *
 *@param from first row to measure (inclusive)
 *@param end last row to measure (not inclusive)
 */
tbx::Size TileView::max_size(unsigned int from, unsigned int end)
{
	tbx::Size max_size(1,1); // tile size can never be less than this
	for (unsigned int index = 0; index < end; index++)
	{
		tbx::Size item_size = _item_renderer->size(index);
		if (item_size.width > max_size.width) max_size.width = item_size.width;
		if (item_size.height > max_size.height) max_size.height = item_size.height;
	}
	return max_size;
}

/**
 * Check if size has been reduced to less than tile_size
 *
 * Measures from first to end-1
 *
 *@param from first row to measure (inclusive)
 *@param end last row to measure (not inclusive)
 *@return maximum size in the range.
 */
tbx::Size TileView::check_size(unsigned int from, unsigned int end)
{
	tbx::Size max_size(1,1);
	for (unsigned int index = 0; index < end; index++)
	{
		tbx::Size item_size = _item_renderer->size(index);
		if (item_size.width > max_size.width) max_size.width = item_size.width;
		if (item_size.height > max_size.height) max_size.height = item_size.height;
		if (max_size == _tile_size) break; // Don't need to do any more
	}
	return max_size;
}


/**
 * Redraw the window
 */
void TileView::redraw(const RedrawEvent &event)
{
	BBox work_clip = event.visible_area().work(event.clip());
	Size cell_size = _tile_size;
	int row_count = (_count + _cols_per_row - 1) / _cols_per_row;

	int first_row = (-work_clip.max.y + _margin.top) / cell_size.height;
	int last_row =  (-work_clip.min.y + _margin.top) / cell_size.height;

	if (last_row < 0) return; // Nothing to draw
	if (first_row > row_count) return; // Nothing to draw
	if (first_row < 0) first_row = 0;
	if (last_row > row_count) last_row = row_count - 1;

    int first_col = (work_clip.min.x - _margin.left) / cell_size.width;
    int last_col = (work_clip.max.x - _margin.left) / cell_size.width;

    if (first_col >= (int)_count) return; // Nothing to redraw
    if (first_col >= _cols_per_row) return; // Nothing to redraw

    if (last_col >= (int)_count) last_col = _count - 1;
    if (last_col >= _cols_per_row) last_col = _cols_per_row - 1;

    ItemRenderer::Info cell_info(event);

	cell_info.bounds.max.y = -first_row * cell_size.height - _margin.top;
	cell_info.screen.y = event.visible_area().screen_y(cell_info.bounds.max.y);
	int first_col_x = first_col * cell_size.width + _margin.left;
	int first_col_scr_x = event.visible_area().screen_x(first_col_x);

    for (int row = first_row; row <= last_row; row++)
    {
        cell_info.bounds.min.y = cell_info.bounds.max.y - cell_size.height;
        cell_info.bounds.min.x = first_col_x;
        cell_info.screen.y -= cell_size.height;
        cell_info.screen.x = first_col_scr_x;
        cell_info.index = row * _cols_per_row + first_col;

        for (int col = first_col; col <= last_col; col++)
        {
           if (cell_info.index < _count)
           {
        	  cell_info.selected = (_selection) ? _selection->selected(cell_info.index) : false;
              cell_info.bounds.max.x = cell_info.bounds.min.x + cell_size.width;

              _item_renderer->render(cell_info);
              cell_info.bounds.min.x = cell_info.bounds.max.x;
              cell_info.screen.x += cell_size.width;
              cell_info.index++;
           }
        }
        cell_info.bounds.max.y = cell_info.bounds.min.y;
    }
}

/**
 * Window has been opened or resized, so re do layout
 */
void TileView::open_window(OpenWindowEvent &event)
{
	if (updates_enabled() && recalc_layout(event.visible_area()))
	{
		refresh();
	}
}

/**
 * Update the Window extent after a change in size.
 *
 * Note: TileView do not change the width of the extent,
 * just the height.
 */
void TileView::update_window_extent()
{
	if (!updates_enabled()) return;

	WindowState state;
	_window.get_state(state);
	recalc_layout(state.visible_area().bounds());
}

/**
 * Recalculate the cell layout and update/refresh the window
 *
 * @param visible_area visible area of window on screen
 * @returns true if the number of cols_per_row changed
 */
bool TileView::recalc_layout(const BBox &visible_area)
{
	int cols_per_row = (visible_area.width() - _margin.left - _margin.right) / _tile_size.width;
	bool cols_per_row_changed = false;
	if (cols_per_row < 1) cols_per_row = 1;

	if (cols_per_row != _cols_per_row)
	{
		// Need to re do the layout
		_cols_per_row = cols_per_row;
		cols_per_row_changed = true;
	}

	int rows = (_count + cols_per_row - 1)/cols_per_row;
	int height = rows * _tile_size.height + _margin.top + _margin.bottom;
	if (height < visible_area.height())
		height = visible_area.height();
	BBox extent(0,-height, visible_area.width(), 0);
	_window.extent(extent);

	return cols_per_row_changed;
}

/**
 * Refresh the whole report view.
 *
 * It should also be called if all the data changes.
 */

void TileView::refresh()
{
	if (updates_enabled()) _window.force_redraw(_window.extent());
}

/**
 * Inform the view that items have been inserted.
 *
 * @param where location for insertion
 * @param how_many number of items inserted
 */
void TileView::inserted(unsigned int where, unsigned int how_many)
{
	unsigned int refresh_from = where;
	if (_item_renderer != 0)
	{
		// Automatically set the height if not set
		if ((_flags & AUTO_SIZE))
		{
			// Auto size
			Size size = max_size(where, where + how_many);
			if (size.width > _tile_size.width)
			{
				_tile_size.width = size.width;
				refresh_from = 0;
			}
			if (size.height > _tile_size.height)
			{
				_tile_size.height = size.height;
				refresh_from = 0;
			}
		}
	}
	_count += how_many;
	update_window_extent();

	if (_selection) _selection->inserted(where, how_many);

	int first_row = refresh_from / _cols_per_row;
	int last_row = _count / _cols_per_row + 1;
	if (updates_enabled())
	{
		BBox dirty(_margin.left,
			-last_row * _tile_size.height - _margin.top,
			_cols_per_row * _tile_size.width + _margin.left,
			-first_row * _tile_size.height - _margin.top);
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
void TileView::removing(unsigned int where, unsigned int how_many)
{
	_flags |= AUTO_SIZE_CHECKED;
	if (_flags & AUTO_SIZE)
	{
		for (unsigned int row = where; row < where + how_many; row++)
		{
			Size item_size = _item_renderer->size(row);
			if (item_size.width == _tile_size.width
				|| item_size.height == _tile_size.height)
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
void TileView::removed(unsigned int where, unsigned int how_many)
{
	if ((_flags & AUTO_SIZE) && !(_flags & AUTO_SIZE_CHECKED))
	{
		_flags |= WANT_AUTO_SIZE;
	}
	_flags &= ~ AUTO_SIZE_CHECKED;

	int first_row = where / _cols_per_row;
	int last_row = _count / _cols_per_row + 1;

	_count -= how_many;
	Size old_size = _tile_size;
	if (_flags & WANT_AUTO_SIZE)
	{
		_flags &= ~WANT_AUTO_SIZE;
		if (_count == 0) _tile_size = Size(1,1);
		else _tile_size = check_size(0, _count);
		if (old_size != _tile_size) first_row = 0;
	}

	if (_selection) _selection->removed(where, how_many);
	update_window_extent();

	//TODO: Use window block copy to update
	if (updates_enabled())
	{
		BBox dirty(_margin.left,
			-last_row * old_size.height - _margin.top,
			_cols_per_row * old_size.width + _margin.left,
			-first_row * old_size.height - _margin.top);
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
void TileView::changing(unsigned int where, unsigned int how_many)
{
	_flags |= AUTO_SIZE_CHECKED;
	if (_flags & AUTO_SIZE)
	{
		for (unsigned int row = where; row < where + how_many; row++)
		{
			Size item_size = _item_renderer->size(row);
			if (item_size.width == _tile_size.width
				|| item_size.height == _tile_size.height)
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
void TileView::changed(unsigned int where, unsigned int how_many)
{
	bool may_shrink = true;
	int first_row = where / _cols_per_row;
	int last_row = _count / _cols_per_row + 1;

	if ((_flags & AUTO_SIZE))
	{
		if  (_flags & AUTO_SIZE_CHECKED)
		{
			if ((_flags & WANT_AUTO_SIZE) == 0) may_shrink = false;
		}

		Size new_size = (may_shrink) ? Size(1,1) : _tile_size;
		for (unsigned int row = where; row < where + how_many; row++)
		{
			Size item_size = _item_renderer->size(row);
			if (item_size.width > new_size.width) new_size.width = item_size.width;
			if (item_size.height > new_size.height) new_size.height = item_size.height;
		}

		if (new_size.width < _tile_size.width
				|| new_size.height < _tile_size.height)
		{
			Size check;
			if (where > 0)
			{
				check = check_size(0, where);
				if (check.width > new_size.width) new_size.width = check.width;
				if (check.height > new_size.height) new_size.height = check.height;
			}
			if (new_size != _tile_size && where+how_many <=_count)
			{
				check = check_size(where+how_many-1, _count);
				if (check.width > new_size.width) new_size.width = check.width;
				if (check.height > new_size.height) new_size.height = check.height;
			}
		}

		if (new_size != _tile_size)
		{
			tile_size(new_size);
			_flags |= AUTO_SIZE;
			first_row = 0;
		}
	}
	_flags &= ~(AUTO_SIZE_CHECKED | WANT_AUTO_SIZE);

	if (updates_enabled())
	{
		BBox dirty(_margin.left,
			-last_row * _tile_size.height - _margin.top,
			_cols_per_row * _tile_size.width + _margin.left,
			-first_row * _tile_size.height - _margin.top);
		_window.force_redraw(dirty);
	}
}

/**
 * Whole view has been cleared
 */
void TileView::cleared()
{
	if (_count)
	{
		refresh();
		_count = 0;
		if (_flags & AUTO_SIZE) _tile_size = Size(1,1);
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
 * Find the index to insert an item based on the screen
 * coordinates given
 *
 * @param scr_pt position to find the index for
 * @return index for insert.
 */
unsigned int TileView::insert_index(const Point &scr_pt) const
{
	WindowState state;
	_window.get_state(state);

	Point work = state.visible_area().work(scr_pt);

	work.x -= _margin.left;
	work.y += _margin.top;

	int col = (work.x + _tile_size.width/2) / _tile_size.width;
	int row = (-work.y + _tile_size.height/2) / _tile_size.height;

	unsigned int index;
	if (row < 0) index = 0;
	else
	{
		if (col < 0) col = 0;
		else if (col >= _cols_per_row)
		{
			col = 0;
			row++;
		}
		index = row * _cols_per_row + col;
		if (index > _count) index = _count;
	}

	return index;
}

/**
 * Get item index for the screen location (if any)
 *
 * @param scr_pt position on screen (screen coordinates)
 * @return index of item under location or NO_INDEX if no item at location.
 */
unsigned int TileView::screen_index(const Point &scr_pt) const
{
	WindowState state;
	_window.get_state(state);

	Point work = state.visible_area().work(scr_pt);

	work.x -= _margin.left;
	work.y += _margin.top;

	int col = work.x / _tile_size.width;
	int row = -work.y / _tile_size.height;

	unsigned int index = NO_INDEX;
	if (col >= 0 && col < _cols_per_row
		&& row >= 0)
	{
	   index = row * _cols_per_row + col;
	   if (index >= _count) index = NO_INDEX;
	}

	return index;
}

/**
 * Hit test the given screen point.
 *
 * Unlike screen_index this uses the cell renderer to
 * see if it has actually hit content rather than just
 * somewhere in the cell.
 *
 * returns: index of hit or -1 if none.
 */
unsigned int TileView::hit_test(const Point &scr_pt) const
{
	WindowState state;
	_window.get_state(state);

	Point work = state.visible_area().work(scr_pt);

	work.x -= _margin.left;
	work.y += _margin.top;

	int column = work.x / _tile_size.width;
	int row = -work.y / _tile_size.height;

	unsigned int index = NO_INDEX;
	if (column >= 0 && column < _cols_per_row
		&& row >= 0)
	{
	   index = row * _cols_per_row + column;
	   if (index >= _count) index = NO_INDEX;
	   else
	   {
		   work.x -= column * _tile_size.width;
		   work.y += (row + 1) * _tile_size.height;
		   if (!_item_renderer->hit_test(index, _tile_size, work)) index = -1;
	   }
	}

	return index;
}

/**
 * Get the cell bounds for the specified index.
 *
 * @param bounds bounding box to update (work area coordinates)
 * @param index index to retrieve the bounds for
 *
 */
void TileView::get_bounds(BBox &bounds, unsigned int index) const
{
	int column = index % _cols_per_row;
	int row = index / _cols_per_row;

	bounds.min.x = _margin.left + column * _tile_size.width;
	bounds.max.x = bounds.min.x + _tile_size.width;
	bounds.max.y = -_margin.top - row * _tile_size.height;
	bounds.min.y = bounds.max.y - _tile_size.height;
}

/**
 * Get bounding box of the range of indices in work area coordinates
 */
void TileView::get_bounds(BBox &bounds, unsigned int first, unsigned int last) const
{
	int first_row = first / _cols_per_row;
	int last_row = last / _cols_per_row;
	int first_col = 0;
	int last_col = _cols_per_row;

	if (first_row == last_row)
	{
		first_col = first % _cols_per_row;
		last_col = last % _cols_per_row;
	}

	bounds.min.x = _margin.left + first_col * _tile_size.width;
	bounds.max.x = _margin.left + (last_col + 1) * _tile_size.width;
	bounds.max.y = -_margin.top - first_row * _tile_size.height;
	bounds.min.y = -_margin.top - (last_row+1) * _tile_size.height;
}

/**
 * Process selection by dragging.
 *
 * allow_drag_selection must be set for this to be called.
 *
 * @param drag_box Final bounding box of drag
 * @param adjust adjust button was used for selection
 */
void TileView::process_drag_selection(const BBox &drag_box, bool adjust)
{
    WindowState state;
    _window.get_state(state);
    BBox work_box = state.visible_area().work(drag_box);
    work_box.normalise();

    work_box.move(-_margin.left, _margin.top);

    int first_col = work_box.min.x / _tile_size.width;
    int last_col = work_box.max.x / _tile_size.width;

    if (first_col < 0) first_col = 0;
    if (last_col >= _cols_per_row) last_col = _cols_per_row - 1;

    if (first_col > last_col) return; // nothing to do

    int first_row = -work_box.max.y / _tile_size.height;
    int last_row = -work_box.min.y / _tile_size.height;
    if (first_row < 0) first_row = 0;
    if (last_row > (int)(_count-1)/_cols_per_row) last_row = (_count-1)/_cols_per_row;

    if (first_row > last_row) return; // nothing to do

    std::pair<unsigned int, unsigned int> range;

    unsigned int index = first_row * _cols_per_row + first_col;
    if (index >= _count) return; // Nothing to do

    int end_of_row_skip = _cols_per_row - (last_col - first_col+1);

    BBox row_relative(work_box);
    row_relative.move(-_tile_size.width * first_col, _tile_size.height * (first_row+ 1));
    BBox col_relative(row_relative);

    int col;
    int row = first_row;

    range.first = NO_INDEX;

    // Check top line
    for (col = first_col; col <= last_col && index < _count; col++)
    {
    	if (_item_renderer->intersects(index, _tile_size, col_relative))
    	{
    		if (range.first == NO_INDEX)
    		{
    			range.first = range.second = index;
    		} else
    		{
    			if (index == range.second + 1)
    			{
    				range.second++;
    			} else
    			{
    				if (adjust) _selection->toggle(range.first, range.second);
    				else _selection->select(range.first, range.second);
    				range.first = range.second = index;
    			}
    		}
    	}
    	index++;
    	col_relative.move_left(_tile_size.width);
    }

    row++;
    row_relative.move_up(_tile_size.height);
    if (row < last_row)
    {
    	int c1, c2;
    	unsigned int i1, i2;
    	while (row < last_row)
    	{
			// Only need to check start and end of column in middle
			index += end_of_row_skip;
			if (_item_renderer->intersects(index, _tile_size, row_relative))
			{
				c1 = first_col;
				i1 = index;
			} else
			{
				c1 = first_col + 1;
				i1 = index + 1;
			}

			if (first_col == last_col)
			{
				c2 = first_col;
				i2 = index;
			} else
			{
				col_relative = row_relative;
				int dist = last_col - first_col;
				index += dist;
				col_relative.move_left(dist * _tile_size.width);
				if (_item_renderer->intersects(index, _tile_size, col_relative))
				{
					c2 = last_col;
					i2 = index;
				} else
				{
					c2 = last_col - 1;
					i2 = index - 1;
				}
			}

			if (c1 <= c2)
			{
				if (range.first == NO_INDEX)
				{
					range.first = i1;
					range.second = i2;
				} else if (i1 == range.second+1)
				{
					range.second = i2;
				} else
				{
    				if (adjust) _selection->toggle(range.first, range.second);
    				else _selection->select(range.first, range.second);
					range.first = i1;
					range.second = i2;
				}
			}
			index++;
			row++;
			row_relative.move_up(_tile_size.height);
    	}
    }

    // Bottom row
    if (row == last_row)
    {
        index += end_of_row_skip;
		col_relative = row_relative;
        for (col = first_col; col <= last_col && index < _count; col++)
        {
        	if (_item_renderer->intersects(index, _tile_size, col_relative))
        	{
        		if (range.first == NO_INDEX)
        		{
        			range.first = range.second = index;;
        		} else
        		{
        			if (index == range.second + 1)
        			{
        				range.second++;
        			} else
        			{
        				if (adjust) _selection->toggle(range.first, range.second);
        				else _selection->select(range.first, range.second);
        				range.first = range.second = index;
        			}
        		}
        	}
        	index++;
        	col_relative.move_left(_tile_size.width);
        }
    }

    if (range.first != NO_INDEX)
    {
		if (adjust) _selection->toggle(range.first, range.second);
		else _selection->select(range.first, range.second);
    }
}

}
}
