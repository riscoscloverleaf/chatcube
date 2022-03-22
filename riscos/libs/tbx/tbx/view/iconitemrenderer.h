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
 * iconitemrenderer.h
 *
 *  Created on: 22 Apr 2010
 *      Author: alanb
 */

#ifndef TBX_ICONITEMRENDERER_H_
#define TBX_ICONITEMRENDERER_H_

#include "itemrenderer.h"

namespace tbx {

namespace view {

/**
 * Class to render an icon with text similar to the normal
 * view in the filer.
 *
 * i.e. An icon centered above some text.
 *
 * Construct with ItemViewValue<std::string> value providers to the
 * return the text and sprite name to render or derive from this
 * class and override the text() and sprite_name() members.
 */
class IconItemRenderer: public tbx::view::TypedItemRenderer<std::string>
{
	ItemViewValue<std::string> *_sprite_name_provider;
	bool _use_client_area;

protected:
	IconItemRenderer(bool use_client = true);

public:
	IconItemRenderer(ItemViewValue<std::string> *np, bool use_client = true);
	IconItemRenderer(ItemViewValue<std::string> *np, ItemViewValue<std::string> *sp, bool use_client = true);
	virtual ~IconItemRenderer();

	virtual std::string text(unsigned int index) const;
	virtual std::string sprite_name(unsigned int index) const;

	/**
	 * Returns true to use the client sprite area
	 * to retrieve the sprite to display, or false for the wimp sprite area.
	 *
	 * default value is given in the constructor
	 *
	 * Override if the location of the sprite can vary between the two
	 */
	virtual bool use_client_sprite_area(unsigned int index) const {return _use_client_area;}

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
	virtual void render(const ItemRenderer::Info &info);

	static tbx::Size standard_size(unsigned int width = 160, unsigned int sprite_height = 68);

	virtual unsigned int width(unsigned int index) const;
	virtual unsigned int height(unsigned int index) const;
	virtual Size size(unsigned int index) const;
	virtual bool hit_test(unsigned int index, const Size &size, const Point &pos) const;
	virtual bool intersects(unsigned int index, const Size &size, const BBox &box) const;

	void get_sprite_box(unsigned int index, const Size &size, BBox &sbox) const;
	void get_text_box(unsigned int index, const Size &size, BBox &sbox) const;
};

}

}

#endif /* TBX_ICONITEMRENDERER_H_ */
