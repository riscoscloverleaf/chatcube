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
 * reportview.cc
 *
 *  Created on: 30 Mar 2010
 *      Author: alanb
 */

#include "reportview.h"
#include "../osgraphics.h"

namespace tbx {

namespace view {

/**
 * Construct a report view for a window.
 *
 * @param window Window displaying the report view
 */
ReportView::ReportView(Window window) : ItemView(window),
	    _height(0), _width(0), _column_gap(4)
{
}

/**
 * Destructor of report view
 *
 */
ReportView::~ReportView()
{
}


/**
 * Change the row height.
 *
 * If the row height is 0. It will take the height from
 * the first item in the list. This is how the view
 * starts up.
 *
 * @param height new row height
 */
void ReportView::row_height(unsigned int height)
{
	if (height == 0 && _count && _columns.size() != 0)
	{
		height = calc_row_height();
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
 * Calculate the row height from the first row of the report
 *
 * @returns calculated row height
 */
unsigned int ReportView::calc_row_height() const
{
	std::vector<ColInfo>::const_iterator i;
	unsigned int height = 0;
	for (i = _columns.begin(); i != _columns.end(); ++i)
	{
		unsigned int col_height = (*i).renderer->height(0);
		if (col_height > height) height = col_height;
	}

	return height;
}

/**
 * Turn on or off auto sizing.
 *
 * When auto sizing is on the item width is checked whenever an
 * item is added, changed or removed.
 *
 * To prevent a scan of the whole list on removal call the
 * removing method before actually doing a removal.
 *
 * @param on true to turn autosizing on
 */
void ReportView::auto_size(bool on)
{
	if (on)
	{
		if ((_flags & AUTO_SIZE)==0)
		{
			size_all_to_width();
			_flags |= AUTO_SIZE;
		}
	} else
	{
		_flags &= ~AUTO_SIZE;
	}
}


/**
 * Change the gap between columns
 *
 * @param gap new gap between the columns
 */
void ReportView::column_gap(unsigned int gap)
{
	if (_column_gap != gap)
	{
	    if (column_count() > 1)
	    {
	       if (_column_gap < gap) _width += (column_count() - 1) * (gap-_column_gap);
	       else _width -= (column_count() - 1) * (_column_gap - gap);
	    }
		_column_gap = gap;
		if (_count && column_count())
		{
			update_window_extent();
			refresh();
		}
	}
}

/**
 * Update the Window extent after a change in size.
 */
void ReportView::update_window_extent()
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
void ReportView::refresh()
{
	if (updates_enabled())
	{
		BBox all(_margin.left, -_margin.top - _count * _height,
				_margin.left + _width, -_margin.top);
		_window.force_redraw(all);
	}
}


/**
 * Add a column to the report view
 *
 * @param cr Renderer to render cells for the column
 * @param width the width of the column
 * (any value other than zero turns off auto size for all columns)
 * @returns zero based column number
 */
unsigned int ReportView::add_column(ItemRenderer *cr, unsigned int width /*= 0*/)
{
	if (width > 0) _flags &=~AUTO_SIZE;
	if ((_flags & AUTO_SIZE) && _count)
	{
		for(unsigned int row = 0; row < _count; row++)
		{
			unsigned int col_width = cr->width(row);
			if (col_width > width) width = col_width;
		}
	}
	ColInfo ci;
	ci.renderer = cr;
	ci.width = width;
	if (column_count()) _width += _column_gap;
	_columns.push_back(ci);
	_width += width;
	update_window_extent();
	return column_count()-1;
}

/**
 * Remove column from report
 *
 * @param column zero based column number to remove
 */
void ReportView::remove_column(unsigned int column)
{
	if (column >= column_count()) return;
	_width -= _columns[column].width;
	_columns.erase(_columns.begin() + column);
	if (column_count()) _width -= _column_gap;
	update_window_extent();

	refresh(); // TODO: Use window block copy instead
}

/**
 * Set the column width.
 *
 * Setting the column width turns of auto sizing
 *
 * @param column zero based column number to set width for
 * @param width new width of the column
 */
void ReportView::column_width(unsigned int column, unsigned int width)
{
	if (width > 0) _flags &=~AUTO_SIZE;
	if (column >= column_count()) return;

	if (_columns[column].width != width)
	{
	    if (_columns[column].width > width) _width -= _columns[column].width - width;
	    else _width += width - _columns[column].width;

		_columns[column].width = width;
		//TODO: remove gap for zero width columns
		if (_count)
		{
			update_window_extent();
			refresh(); //TODO: Use window block copy instead
		}
	}
}

/**
 * Returns column width.
 *
 * @param column 0 based column to return width for
 * @returns column width or 0 if column >= column_count()
 */
unsigned int ReportView::column_width(unsigned int column) const
{
	if (column < column_count()) return _columns[column].width;
	return 0;
}

/**
 * Size column to content.
 * Does nothing if there is no content
 *
 * @param column column to resize
 * @param min_width minimum width of the column
 * @param max_width maximum width of the column
 */
void ReportView::size_column_to_width(unsigned int column, unsigned int min_width/* = 0*/, unsigned int max_width /*= 0xFFFFFFFF*/)
{
	if (column >= column_count() || _count == 0) return;
	unsigned int col_width = min_width;
	ItemRenderer *renderer = _columns[column].renderer;

	for (unsigned int row = 0; row < _count; row++)
	{
		unsigned int size = renderer->width(row);
		if (size > max_width) size = max_width;
		if (size > col_width) col_width = size;
	}

	if (col_width != _columns[column].width)
		column_width(column, col_width);
}

/**
 * Size all columns to content.
 * Does nothing if there is no content
 *
 * @param min_width minimum width of any column
 * @param max_width maximum width of any column
 */
void ReportView::size_all_to_width(unsigned int min_width /*= 0*/, unsigned int max_width /*= 0xFFFFFFFF*/)
{
	if (column_count() == 0 || _count == 0) return;
	if (adjust_min_width(0, _count))
	{
		if (min_width > 0 || max_width < _width)
		{
			std::vector<ColInfo>::iterator i;
			for (i = _columns.begin(); i != _columns.end(); ++i)
			{
				unsigned int col_width = (*i).width;
				if (col_width < min_width) col_width = min_width;
				if (col_width > max_width) col_width = max_width;
				if (col_width != (*i).width)
				{
					_width += col_width - (*i).width;
					(*i).width = col_width;
				}
			}
		}

		update_window_extent();
		refresh();
	}
}


/**
 * Adjust column widths and total width if any items in the
 * given range exceed the current widths.
 *
 * Note: Does not refresh the display
 *
 * @param start first row to check (inclusive)
 * @param end last row to check (not inclusive)
 * @returns true if any column was adjusted
 */
bool ReportView::adjust_max_width(unsigned int start, unsigned int end)
{
	std::vector<ColInfo>::iterator i;
	bool changed = false;

	for (i = _columns.begin(); i != _columns.end(); ++i)
	{
		unsigned int col_width = (*i).width;
		ItemRenderer *renderer = (*i).renderer;

		for (unsigned int row = start; row < end; row++)
		{
			unsigned int size = renderer->width(row);
			if (size > col_width) col_width = size;
		}

		if (col_width > (*i).width)
		{
			_width += col_width - (*i).width;
			(*i).width = col_width;
			changed = true;
		}
	}

	return changed;
}

/**
 * Check if any of the column widths have been reduced and
 * adjust columns if necessary
 *
 * Measures from first to end-1
 *
 *@param from first row to measure (inclusive)
 *@param end last row to measure (not inclusive)
 *@return true if any widths have changed.
 */
bool ReportView::adjust_min_width(unsigned int from, unsigned int end)
{
	std::vector<ColInfo>::iterator i;
	bool changed = false;

	for (i = _columns.begin(); i != _columns.end(); ++i)
	{
		unsigned int col_width = (*i).width;
		ItemRenderer *renderer = (*i).renderer;
		unsigned int max_col_width = 0;

		for (unsigned int row = from; row < end; row++)
		{
			unsigned int size = renderer->width(row);
			if (size > max_col_width) max_col_width = size;
			if (size == col_width) break; // Don't need any more checks on this column
		}

		if (max_col_width < col_width)
		{
			_width -= col_width - max_col_width;
			(*i).width = max_col_width;
			changed = true;
		}
	}

	return changed;
}


/**
 * Call after inserting rows into the collection
 * the ReportView is showing.
 *
 * @param where row column was inserted at
 * @param how_many number of columns inserted
 */
void ReportView::inserted(unsigned int where, unsigned int how_many)
{
	int first_row = where;

	_count += how_many;

	if (column_count() != 0)
	{
		// Automatically set the height if not set
		if (_height == 0) _height = calc_row_height();
		if ((_flags & AUTO_SIZE))
		{
			// Auto size
			if (adjust_max_width(where, where + how_many)) first_row = 0;
		}
	}

	//TODO: Use window block copy to update
	update_window_extent();

	if (_selection) _selection->inserted(where, how_many);

	if (updates_enabled())
	{
		BBox dirty(_margin.left,
			-_count * _height - _margin.top,
			_width + _margin.left,
			-first_row * _height - _margin.top);
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
void ReportView::removing(unsigned int where, unsigned int how_many)
{
	_flags |= AUTO_SIZE_CHECKED;
	if (_flags & AUTO_SIZE)
	{
		if (where == 0 && how_many >= _count) _flags |= WANT_AUTO_SIZE;
		else if (column_count())
		{
			std::vector<ColInfo>::iterator i;
			for (i = _columns.begin(); i != _columns.end(); ++i)
			{
				unsigned int col_width = (*i).width;
				ItemRenderer *renderer = (*i).renderer;
				for (unsigned int row = where; row < where + how_many; row++)
				{
					unsigned int item_width = renderer->width(row);
					if (item_width == col_width)
					{
						_flags |= WANT_AUTO_SIZE;
						break;
					}
				}
				if (_flags & WANT_AUTO_SIZE) break;
			}
		}
	}
}

/**
 * Call after removing rows from the collection
 * the ReportView is showing.
 *
* @param where location items were removed from
* @param how_many number of items that were removed
 */
void ReportView::removed(unsigned int where, unsigned int how_many)
{
	unsigned int first = where;
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
		if (_count == 0)
		{
			std::vector<ColInfo>::iterator i;
			first = 0;
			for (i = _columns.begin(); i != _columns.end(); ++i)
			{
				(*i).width = 0;
			}
			_width = 0;
		} else if (adjust_min_width(0, _count)) first = 0;
	}
	//TODO: Use window block copy to update
	if (_selection) _selection->removed(where, how_many);
	if (updates_enabled())
	{
		BBox dirty(_margin.left,
			-(_count + how_many) * _height - _margin.top,
			old_width + _margin.left,
			-first * _height - _margin.top);

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
void ReportView::changing(unsigned int where, unsigned int how_many)
{
	_flags |= AUTO_SIZE_CHECKED;
	if (_flags & AUTO_SIZE)
	{
		if (column_count())
		{
			std::vector<ColInfo>::iterator i;
			for (i = _columns.begin(); i != _columns.end(); ++i)
			{
				unsigned int col_width = (*i).width;
				ItemRenderer *renderer = (*i).renderer;
				for (unsigned int row = where; row < where + how_many; row++)
				{
					unsigned int item_width = renderer->width(row);
					if (item_width == col_width)
					{
						_flags |= WANT_AUTO_SIZE;
						break;
					}
				}
				if (_flags & WANT_AUTO_SIZE) break;
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
void ReportView::changed(unsigned int where, unsigned int how_many)
{
	unsigned int last = where + how_many;
	unsigned int old_width = _width;

	if ((_flags & AUTO_SIZE) && column_count())
	{
		bool may_shrink = true;
		if ((_flags & AUTO_SIZE_CHECKED) && ((_flags & WANT_AUTO_SIZE) == 0))
		{
			may_shrink = false;
		}

		std::vector<ColInfo>::iterator i;
		bool col_shrinks = may_shrink;
		for (i = _columns.begin(); i != _columns.end(); ++i)
		{
			unsigned int col_width = 0;
			ItemRenderer *renderer = (*i).renderer;
			for (unsigned int row = where; row < where + how_many; row++)
			{
				unsigned int item_width = renderer->width(row);
				if (item_width >= col_width) col_width = item_width;
			}

			if ((*i).width != col_width)
			{
				if (col_width < (*i).width)
				{
					col_shrinks = may_shrink;
				} else if (col_width > (*i).width)
				{
					_width += col_width - (*i).width;
					(*i).width = col_width;
				}
			}
		}

		if (col_shrinks && adjust_min_width(0, _count))
		{
			last = _count;
		}
		if (old_width != _width) update_window_extent();
		if (old_width < _width) old_width = _width;
	}

	_flags &= ~(AUTO_SIZE_CHECKED | WANT_AUTO_SIZE);
	if (updates_enabled())
	{
		BBox dirty(_margin.left,
			-last * _height - _margin.top,
			old_width + _margin.left,
			-where * _height - _margin.top);
		_window.force_redraw(dirty);
	}
}

/**
 * Whole view has been cleared
 */
void ReportView::cleared()
{
	if (_count)
	{
		refresh();
		_count = 0;
		if (_flags & AUTO_SIZE)
		{
			std::vector<ColInfo>::iterator i;
			for (i = _columns.begin(); i != _columns.end(); ++i)
			{
				(*i).width = 0;
			}
			_width = 0;
		}

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
* Inform the view that one column has been changed
*
* This call is optional, but it stops the need for a full
* scan every time something an item is changed when auto size is on.
*
* @param index location of item that will be changed from
* @param column The column that has been changed
*/
void ReportView::cell_changing(unsigned int index, unsigned int column)
{
	_flags |= AUTO_SIZE_CHECKED;
	if (_flags & AUTO_SIZE)
	{
		if (index < _count && column < column_count())
		{
			unsigned int col_width = _columns[column].width;
			ItemRenderer *renderer = _columns[column].renderer;
			unsigned int item_width = renderer->width(index);
			if (item_width == col_width)
			{
				_flags |= WANT_AUTO_SIZE;
			}
		}
	}
}

/**
 * Inform the view that items have been changed.
 *
 * @param index location of item that will be changed from
 * @param column The column that has been changed
 */
void ReportView::cell_changed(unsigned int index, unsigned int column)
{
	unsigned int first = index;
	unsigned int last = index + 1;
	unsigned int last_col_pos = x_from_column(column + 1);

	if ((_flags & AUTO_SIZE) && column < column_count())
	{
		bool may_shrink = true;
		if ((_flags & AUTO_SIZE_CHECKED) && ((_flags & WANT_AUTO_SIZE) == 0))
		{
			may_shrink = false;
		}

		unsigned int col_width = _columns[column].width;
		unsigned int old_width = col_width;
		ItemRenderer *renderer = _columns[column].renderer;
		unsigned int item_width = renderer->width(index);
		if (item_width > col_width)
		{
			col_width = item_width;
		} else if (item_width < col_width && may_shrink)
		{
			col_width = item_width;
			for(unsigned int row = 0; row < _count; row++)
			{
				item_width = renderer->width(index);
				if (item_width > col_width) col_width = item_width;
				if (col_width == old_width) break;
			}
		}
		if (col_width != old_width)
		{
			_width += col_width - old_width;
			_columns[column].width = col_width;
			first = 0;
			last = _count;
			if (last_col_pos < _margin.left + _width)
				last_col_pos = _margin.left + _width;
			update_window_extent();
		}
	}

	_flags &= ~(AUTO_SIZE_CHECKED | WANT_AUTO_SIZE);
	if (updates_enabled())
	{
		BBox dirty(x_from_column(column),
			-last * _height - _margin.top,
			last_col_pos,
			-first * _height - _margin.top);
		_window.force_redraw(dirty);
	}

}

/**
 * Get column from position on window
 *
 * @param x horizontal position from the left of the window work area
 */
unsigned int ReportView::column_from_x(int x) const
{
	int cc = column_count();
	int col = 0;
	x -= _margin.left;
	while (x > 0 && col < cc)
	{
		x -= _columns[col].width + _column_gap;
		if (x > 0) col++;
	}

	return col;
}

/**
 * Return x coordinate relative to window of column
 *
 * @returns horzontal distance of column from left of the window work area
 */
int ReportView::x_from_column(unsigned int column) const
{
	int x = _margin.left;
	unsigned int col = 0;

	while (col < column)
	{
		x += _columns[col].width + _column_gap;
		col++;
	}

	return x;
}

/**
 * Redraw the window.
 *
 * @param event information on area needing redraw
 */
void ReportView::redraw(const RedrawEvent &event)
{
	BBox work_clip = event.visible_area().work(event.clip());

	unsigned int first_row = (-work_clip.max.y - _margin.top) / _height;
	unsigned int last_row =  (-work_clip.min.y - _margin.top) / _height;

	if (first_row < 0) first_row = 0;
	if (last_row < 0) return; // Nothing to draw

	if (first_row >= _count) return; // Nothing to draw
	if (last_row >= _count) last_row = _count - 1;

	unsigned int first_col = column_from_x(work_clip.min.x);
	unsigned int last_col = column_from_x(work_clip.max.x);

    if (first_col >= column_count()) return; // Nothing to redraw
    if (last_col >= column_count()) last_col = column_count() - 1;

    ItemRenderer::Info cell_info(event);

	cell_info.bounds.max.y = -first_row * _height - _margin.top;
	cell_info.screen.y = event.visible_area().screen_y(cell_info.bounds.max.y);
	int first_col_x = x_from_column(first_col);
	int first_col_scr_x = event.visible_area().screen_x(first_col_x);

    for (unsigned int row = first_row; row <= last_row; row++)
    {
        cell_info.bounds.min.y = cell_info.bounds.max.y - _height;
        cell_info.bounds.min.x = first_col_x;
        cell_info.screen.y -= _height;
        cell_info.screen.x = first_col_scr_x;
        cell_info.index = row;

        cell_info.selected = (_selection != 0 && _selection->selected(row));

        if (cell_info.selected)
        {
        	// Fill background with selected colour
        	int sel_right;
        	if (last_col < column_count()-1) sel_right = x_from_column(last_col+1);
        	else sel_right = _width + _margin.left;
    		OSGraphics g;
    		g.foreground(Colour::black);
    		g.fill_rectangle(first_col_scr_x, cell_info.screen.y,
    				first_col_scr_x + sel_right - first_col_x - 1,
    				cell_info.screen.y + _height - 1);
        }

        for (unsigned int col = first_col; col <= last_col; col++)
        {
			cell_info.bounds.max.x = cell_info.bounds.min.x + _columns[col].width;
        	if (_columns[col].width > 0)
        	{
				_columns[col].renderer->render(cell_info);
        	}
			cell_info.screen.x += cell_info.bounds.max.x - cell_info.bounds.min.x + _column_gap;
			cell_info.bounds.min.x = cell_info.bounds.max.x + _column_gap;
        }
        cell_info.bounds.max.y = cell_info.bounds.min.y;
    }
}

/**
 * Return the index for inserting an item from a screen
 * point.
 *
 * @param scr_pt position to find the index for in screen coordinates
 * @return index for insert (returns count() if below last item).
 */
unsigned int ReportView::insert_index(const Point &scr_pt) const
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
 *
 * @param scr_pt the point to test
 * @return the index of the item below the point or NO_INDEX if the point is
 *         not over an item.
 */
unsigned int ReportView::screen_index(const Point &scr_pt) const
{
	WindowState state;
	_window.get_state(state);

	int work_y = state.visible_area().work_y(scr_pt.y);
	work_y += _margin.top;

	unsigned int row = -work_y / _height;

	if (row < 0 && row >= count()) row = NO_INDEX;

	return row;

}

/**
 * Return the index under the screen point, checks
 * if it hits content of the item renderer.
 *
 * @param scr_pt location on screen to test
 *
 * Returns: NO_INDEX if doesn't hit content
 */
unsigned int ReportView::hit_test(const Point &scr_pt) const
{
	unsigned int index;
	if (column_count() == 0 || _count == 0)
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
			unsigned int column = column_from_x(work_pt.x);
			if (column < column_count())
			{
				BBox bounds;
				get_cell_bounds(bounds, index, column);
				Point in_item;
				in_item.x = work_pt.x - bounds.min.x;
				in_item.y = work_pt.y - bounds.min.y;
				if (!_columns[column].renderer->hit_test(index, bounds.size(), in_item))
					index = NO_INDEX;
			}
		}
	}

	return index;
}

/**
 * Get bounding box of the given index in work area coordinates
 *
 * @param bounds updated to bounds of row with given index
 * @param index zero based row number
 */
void ReportView::get_bounds(BBox &bounds, unsigned int index) const
{
	bounds.min.x = _margin.left;
	bounds.max.x = bounds.min.x + _width;
	bounds.max.y = -_margin.top - index * _height;
	bounds.min.y = bounds.max.y - _height;
}

/**
 * Get bounding box of the range of indices in work area coordinates
 *
 * @param bounds updated to bounds of given rows
 * @param first first zero based row number
 * @param last last zero based row number
 */
void ReportView::get_bounds(BBox &bounds, unsigned int first, unsigned int last) const
{
	bounds.min.x = _margin.left;
	bounds.max.x = bounds.min.x + _width;
	bounds.max.y = -_margin.top - first * _height;
	bounds.min.y = -_margin.top - (last + 1) * _height;
}

/**
 * Get the cell bounds for the specified column and row
 *
 * It is permitted to specifiy a column and row outside of the
 * used cells.
 *
 * @param bounds bounding box to update (work area coordinates)
 * @param row row to retrieve the bounds for
 * @param column column to retrieve the bounds for
 */
void ReportView::get_cell_bounds(BBox &bounds, unsigned int row, unsigned int column) const
{
	bounds.min.x = x_from_column(column);
	bounds.max.x = bounds.min.x + _columns[column].width;
	bounds.max.y = -_margin.top - row * _height;
	bounds.min.y = bounds.max.y - _height;
}


/**
 * Override this method to process selection by dragging.
 *
 * allow_drag_selection must be set for this to be called.
 *
 * @param drag_box Final bounding box of drag
 * @param adjust adjust button was used for selection
 */
void ReportView::process_drag_selection(const BBox &drag_box, bool adjust)
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

	if (first <= last)
	{
		if (adjust) _selection->toggle(first, last);
		else _selection->select(first, last);
	}
}

// End of namespaces
}

}
