/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2015 Alan Buckley   All Rights Reserved.
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
* FontItemRenderer.h
 *
 *  Created on: 17 Jun 2015
 *      Author: alanb
 */

#ifndef FONTITEMRENDERER_H_
#define FONTITEMRENDERER_H_

#include "../font.h"
#include "itemrenderer.h"

namespace tbx {
namespace view {

/**
 * ItemRenderer to render text with a given font in a fixed height
 */
class FontItemRenderer  : public TypedItemRenderer<std::string>
{
	tbx::Font _font;
	unsigned int _baseline;
	unsigned int _height;

public:
	/**
	 * Construct with object to give value to render
	 *
	 * @param vv object that returns a string to render
	 */
	FontItemRenderer(tbx::view::ItemViewValue<std::string> *vv)
	: tbx::view::TypedItemRenderer<std::string>(vv),
	  _baseline(0), _height(40)
	  {
	  }

	/**
	 * Construct renderer with give font and object to give value to render
	 *
	 * @param font Font to use
	 * @param vv object that returns a string to render
	 */
	FontItemRenderer(tbx::Font font, ItemViewValue<std::string> *vv)
	: TypedItemRenderer<std::string>(vv),
	  _baseline(0), _height(40)
	  {
		this->font(font);
	  }


	virtual ~FontItemRenderer() {};

	/**
	 * Get the font used to render an item
	 */
	tbx::Font font() {return _font;}

	/**
	 * Set the font to render an item.
	 *
	 * Also sets the height and baseline
	 *
	 * @param font font to use
	 */
	void font(tbx::Font font);

	/**
	 * Get the height of the renderer
	 * @returns height of the renderer
	 */
	unsigned int height() const {return _height;}
	/**
	 * Set the height of the renderer.
	 * @param height the new height. This is not checked so should be larger
	 *               than the tallest character
	 */
	void height(unsigned int height) {_height = height;}
	/**
	 * Get the font base line.
	 *
	 * @returns position above the bottom of the item for the text baseline
	 */
	unsigned int baseline() const {return _baseline;}
	/**
	 * Set the font base line
	 *
	 * @param baseline new font base line. Not checked so invalid values
	 *  could cause display problems.
	 */
	void baseline(unsigned int baseline) {_baseline = baseline;}

	/**
	 * Render text in black using font set
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
	 * Get font row height
	 *
	 * @param index (unused in this renderer)
	 */
	virtual unsigned int height(unsigned int index) const {return _height;}

	/**
	 * Called to get the size of the cell.
	 *
	 * @param index zero based index of item to get the size for
	 */
	virtual tbx::Size size(unsigned int index) const;
};

} /* namespace view */
} /* namespace tbx */

#endif /* FONTITEMRENDERER_H_ */
