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
 * itemrenderer.cc
 *
 *  Created on: 24 Mar 2010
 *      Author: alanb
 */

#include "itemrenderer.h"
#include "../font.h"
#include "../sprite.h"


namespace tbx {

namespace view {
/**
 * Render as cell retrieved as text.
 */
void WimpFontItemRenderer::render(const ItemRenderer::Info &info)
{
	std::string t = _value_provider->value(info.index);
	if (!t.empty())
	{
		WimpFont font;

		if (info.selected)
		{
			font.set_colours(Colour::white, Colour::black);
		} else
		{
			font.set_colours(Colour::black, Colour::white);
		}
		font.paint(info.screen.x, info.screen.y+8, t);
	}
}

/**
 * Used to measure the width of a column
 */
unsigned int WimpFontItemRenderer::width(unsigned int index) const
{
	std::string t = _value_provider->value(index);
	if (t.empty()) return 0;

	WimpFont font;
	return font.string_width_os(t);
}

/**
 * Called to get the size of an item.
 */
Size WimpFontItemRenderer::size(unsigned int index) const
{
	return Size((int)width(index), 40);
}

/**
 * Render a sprite in the cell
 */
void SpriteItemRenderer::render(const ItemRenderer::Info &info)
{
	tbx::Sprite *s = _value_provider->value(info.index);
	if (s) s->plot_screen(info.screen);
}

/**
 * Used to measure the width of a column
 */
unsigned int SpriteItemRenderer::width(unsigned int index) const
{
	tbx::Sprite *s = _value_provider->value(index);
	return s ? s->width() : 0;
}

/**
 * Used to measure the height
 */
unsigned int SpriteItemRenderer::height(unsigned int index) const
{
	tbx::Sprite *s = _value_provider->value(index);
	return s ? s->height() : 0;
}

/**
 * Called to get the size of an item.
 */
Size  SpriteItemRenderer::size(unsigned int index) const
{
	tbx::Sprite *s = _value_provider->value(index);
	return s ? Size(s->width(), s->height()) : Size(0,0);
}


}

}
