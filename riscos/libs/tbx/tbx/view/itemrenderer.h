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
 * itemrenderer.h
 *
 *  Created on: 24 Mar 2010
 *      Author: alanb
 */

#ifndef ITEMRENDERER_H_
#define ITEMRENDERER_H_

#include <string>
#include "../bbox.h"
#include "../redrawlistener.h"
#include "viewvalue.h"

namespace tbx
{
   class Sprite;
};

namespace tbx {

namespace view {

/**
 * Base class to render an indexed item.
 *
 */
class ItemRenderer
{
public:
	ItemRenderer() {};
	virtual ~ItemRenderer() {};

public:
	/**
	 * Information on what needs to be redrawn
	 */
	struct Info
	{
		/**
		 * Redraw event object from FixedCellView
		 */
		const tbx::RedrawEvent &redraw;

		/**
		 * Bounding box of cell to redraw in work area coordinates
		 */
		tbx::BBox bounds;

		/**
		 * Bottom left of items bounding box in screen coordinates
		 */
		tbx::Point screen;

		/**
		 * zero based index of item to redraw
		 */
		unsigned int index;

		/**
		 * Is the item selected
		 */
		bool selected;

		/**
		 * Construct from Redraw event.
		 *
		 * Other parameters need to be set before it is used.
		 */
		Info(const tbx::RedrawEvent &r) : redraw(r) {};
	};

	/**
	 * Called to render each item that needs drawing
	 *
	 * The item is only clipped to the containing Windows visible area,
	 * not to the cell bounds so the content must be drawn entirely
	 * inside the cell bounding box or the clip area should be set
	 * before drawing.
	 *
	 * @param info Information on redraw event and item to be redrawn
	 */
	virtual void render(const ItemRenderer::Info &info) = 0;

	/**
	 * Used to measure the width of a column
	 */
	virtual unsigned int width(unsigned int index) const = 0;

	/**
	 * Used to measure the height of column
	 */
	virtual unsigned int height(unsigned int index) const = 0;

	/**
	 * Called to get the size of the item.
	 *
	 * Should return the equivalent of Size(width(index), height(index))
	 */
	virtual Size size(unsigned int index) const = 0;

	/**
	 * Check if the point given hits content of the cell rather
	 * then a margin or background.
	 *
	 * Default returns true so all the cell is content.
	 *
	 * @param index index of item to check
	 * @param size of area item is rendered in
	 * @param pos location to check relative to bottom left of cell
	 * @return true if content is at that location, otherwise false
	 */
	virtual bool hit_test(unsigned int index, const Size &size, const Point &pos) const {return true;}

	/**
	 * Check if the given rectangle intersects the content of the cell
	 * rather than a margin or background.
	 *
	 * Default returns true so all cell is content.
	 *
	 * @param index index of item to  check
	 * @param size of area item is rendered in
	 * @param box box to check if it intersect the cell
	 * @return true if some contents are in the box, otherwise false
	 */
	virtual bool intersects(unsigned int index, const Size &size, const BBox &box) const {return true;}

};

/**
 * Base class for item renderer that work on a specific type and
 * use an ItemViewValue object to return the value to use.
 */
template<class T> class TypedItemRenderer : public ItemRenderer
{
protected:
	ItemViewValue<T> *_value_provider; //!< Object to retrieve the value for a rendered
public:
	/**
	 * Construct the item view renderer
	 *
	 * @param vp object used to retrieve values to render
	 */
	TypedItemRenderer(ItemViewValue<T> *vp) : _value_provider(vp) {}
};

/**
 * Renderer base class to put text in a cell using the current
 * wimp font.
 *
 * The text is provided by an ItemViewValue<std::string> class.
 */
class WimpFontItemRenderer : public TypedItemRenderer<std::string>
{
public:
	/**
	 * Construct with object to give value to render
	 *
	 * @param vv object that returns a string to render
	 */
	WimpFontItemRenderer(ItemViewValue<std::string> *vv)
	: TypedItemRenderer<std::string>(vv)
	  {
	  }

	virtual ~WimpFontItemRenderer() {};

	/**
	 * Render text in black using current wimp font
	 *
	 * @param info Information on what to render and where
	 */
	virtual void render(const ItemRenderer::Info &info);


	/**
	 * Used to measure the width of a column
	 *
	 * @param index zero based index of item to get the width for
	 */
	virtual unsigned int width(unsigned int index) const;

	/**
	 * Wimp font height is always 40
	 *
	 * @param index (unused in this renderer)
	 */
	virtual unsigned int height(unsigned int index) const {return 40;}

	/**
	 * Called to get the size of the cell.
	 *
	 * @param index zero based index of item to get the size for
	 */
	virtual Size size(unsigned int index) const;

};

/**
 * Item renderer to render a sprite in a cell.
 *
 * Uses an ItemViewValue<Sprite> to provide the sprite to render
 */

class SpriteItemRenderer : public TypedItemRenderer<tbx::Sprite *>
{
public:
	/**
	 * Construct with object to give value to render
	 *
	 * @param vv object that returns a sprite to render
	 */
	SpriteItemRenderer(ItemViewValue<tbx::Sprite *> *vv) :
		 TypedItemRenderer<tbx::Sprite *>(vv) {};

	virtual ~SpriteItemRenderer() {};

	/**
	 * Render the sprite to the screen.
	 */
	virtual void render(const ItemRenderer::Info &info);

	/**
	 * Used to measure the width of a column
	 */
	virtual unsigned int width(unsigned int index) const;

	/**
	 * Used to measure the height
	 */
	virtual unsigned int height(unsigned int index) const;

	/**
	 * Called to get the size of the cell.
	 */
	virtual Size size(unsigned int index) const;
};

}

}

#endif /* ITEMRENDERER_H_ */
