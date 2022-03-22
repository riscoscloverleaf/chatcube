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
 * listview.h
 *
 *  Created on: 25 Mar 2010
 *      Author: alanb
 */

#ifndef TBX_LISTVIEW_H_
#define TBX_LISTVIEW_H_

#include "itemview.h"

namespace tbx {

namespace view {

/**
 * Class to show items in a list down the window.
 *
 *
 */
class ListView : public ItemView
{
protected:
	ItemRenderer *_item_renderer; //!< Object used to render itesm
	unsigned int _height;         //!< Height of one item
	unsigned int _width;		  //!< Width of list view

public:
	ListView(tbx::Window window, ItemRenderer *item_renderer = 0);
	virtual ~ListView();

	void item_renderer(ItemRenderer *ir);


	void row_height(unsigned int height);
	/**
	 * Return the row height
	 */
	unsigned int row_height() const {return _height;}

	virtual void auto_size(bool on);

	void width(unsigned int width);
	/**
	 * Return the current width of the ListView
	 */
	unsigned int width() const;

	unsigned int max_width(unsigned int from, unsigned int end);
	unsigned int check_width(unsigned int from, unsigned int end);

	void size_to_width(unsigned int min_width = 0, unsigned int max_width = 0xFFFFFFFF);

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

	virtual unsigned int insert_index(const Point &scr_pt) const;
	virtual unsigned int screen_index(const Point &scr_pt) const;
	virtual unsigned int hit_test(const Point &scr_pt) const;


	virtual void get_bounds(BBox &bounds, unsigned int index) const;
	virtual void get_bounds(BBox &bounds, unsigned int first, unsigned int last) const;

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

};

}

}

#endif /* LISTVIEW_H_ */
