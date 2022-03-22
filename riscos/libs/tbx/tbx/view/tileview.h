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
 * tileview.h
 *
 *  Created on: 16 Apr 2010
 *      Author: alanb
 */

#ifndef TBX_TILEVIEW_H_
#define TBX_TILEVIEW_H_

#include "itemview.h"
#include "../openwindowlistener.h"

namespace tbx {

namespace view {

/**
 * Class to show items tiled in a window.
 *
 * Each item is placed in a cell of the same size and the items
 * are wrapped depending on the current width of the window.
 */
class TileView : public ItemView, tbx::OpenWindowListener
{
protected:
	ItemRenderer *_item_renderer; //!< Renderer for items in the tile view
	tbx::Size _tile_size;         //!< Size of tiles
	int _cols_per_row;            //!< Number of tiles in a row

public:
	TileView(tbx::Window window, ItemRenderer *item_renderer = 0);
	virtual ~TileView();

	void item_renderer(ItemRenderer *ir);

	/**
	 * Return the size of a tile
	 */
	const tbx::Size &tile_size() const {return _tile_size;}
	void tile_size(tbx::Size new_size);

	tbx::Size max_size(unsigned int from, unsigned int end);
	tbx::Size check_size(unsigned int from, unsigned int end);
	void size_to_tiles();

	virtual void auto_size(bool on);
	// Window events used
	virtual void redraw(const tbx::RedrawEvent &event);
	virtual void open_window(tbx::OpenWindowEvent &event);

	virtual void update_window_extent();
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

	/**
	 * Get bounding box of the range of indices in work area coordinates
	 */
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

	bool recalc_layout(const BBox &visible_area);


};

}

}

#endif /* TBX_TILEVIEW_H_ */
