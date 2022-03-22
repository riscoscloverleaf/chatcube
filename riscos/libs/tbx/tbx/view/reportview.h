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
 * reportview.h
 *
 *  Created on: 30 Mar 2010
 *      Author: alanb
 */

#ifndef TBX_REPORTVIEW_H_
#define TBX_REPORTVIEW_H_

#include "itemview.h"

namespace tbx {

namespace view {

/**
 * Class to handle the display/interaction with a list
 * of items presented in columns.
 */
class ReportView : public ItemView
{
private:
	unsigned int _height;
	unsigned int _width;
	unsigned int _column_gap;

	struct ColInfo
	{
			ItemRenderer *renderer;
			unsigned int width;
	};
	std::vector<ColInfo> _columns;

public:
	ReportView(tbx::Window window);
	virtual ~ReportView();

	virtual void auto_size(bool on);

	void row_height(unsigned int height);
	/**
	 * Get the height of one row of the report
	 */
	unsigned int row_height() const {return _height;}

	void column_gap(unsigned int gap);
	/**
	 * Get the gap between columns in the report
	 */
	unsigned int column_gap() const {return _column_gap;}

	unsigned int add_column(ItemRenderer *cr, unsigned int width = 0);
	//TODO: void insert_column(int where, ReportViewCellRenderer *cr, int width);
	void remove_column(unsigned int column);

	/**
	 * Get the number of columns in the report view
	 */
	unsigned int column_count() const {return _columns.size();}
	/**
	 * Get the number of rows in the report view
	 */
	unsigned int row_count() const {return _count;}

	void column_width(unsigned int column, unsigned int width);
	unsigned int column_width(unsigned int column) const;

	void size_column_to_width(unsigned int column, unsigned int min_width = 0, unsigned int max_width = 0xFFFFFFFF);
	void size_all_to_width(unsigned int min_width = 0, unsigned int max_width = 0xFFFFFFFF);

	// Redraw listener override
	virtual void redraw(const tbx::RedrawEvent &event);


	/**
	 * Called to update the window extent needed to contain
	 * this view.
	 */
	virtual void update_window_extent();

	/**
	 * Called to refresh the entire views contents
	 */
	virtual void refresh();


	virtual void inserted(unsigned int where, unsigned int how_many);
	virtual void removing(unsigned int where, unsigned int how_many);
	virtual void removed(unsigned int where, unsigned int how_many);
	virtual void changing(unsigned int where, unsigned int how_many);
	virtual void changed(unsigned int where, unsigned int how_many);
	virtual void cleared();

	void cell_changing(unsigned int index, unsigned int column);
	void cell_changed(unsigned int index, unsigned int column);


	/**
	 * Return the index for inserting an item from a screen
	 * point.
	 */
	virtual unsigned int insert_index(const Point &scr_pt) const;

	virtual unsigned int screen_index(const Point &scr_pt) const;
	virtual unsigned int hit_test(const Point &scr_pt) const;
	virtual void get_bounds(BBox &bounds, unsigned int index) const;
	virtual void get_bounds(BBox &bounds, unsigned int first, unsigned int last) const;
	void get_cell_bounds(BBox &bounds, unsigned int row, unsigned int column) const;

	unsigned int column_from_x(int x) const;
	int x_from_column(unsigned int column) const;

protected:
	/**
	 * Override this method to process selection by dragging.
	 *
	 * allow_drag_selection must be set for this to be called.
	 *
	 * @param drag_box Final bounding box of drag
	 * @param adjust adjust button was used for selection
	 */
	virtual void process_drag_selection(const BBox &drag_box, bool adjust);

	// Helpers
	unsigned int calc_row_height() const;
	bool adjust_max_width(unsigned int start, unsigned int end);
	bool adjust_min_width(unsigned int from, unsigned int end);
};

}

}

#endif /* REPORTVIEW_H_ */
