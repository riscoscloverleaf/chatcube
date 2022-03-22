/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2015 Alan Buckley   All Rights Reserved.
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
 * FontItemRenderer.cpp
 *
 *  Created on: 17 Jun 2015
 *      Author: alanb
 */

#include "fontitemrenderer.h"

namespace tbx {
namespace view {

/**
 * Set the font to render an item.
 *
 * Also sets the height and baseline
 *
 * @param font font to use
 */
void FontItemRenderer::font(tbx::Font font)
{
	_font = font;
	if (font.is_valid())
	{
		tbx::BBox bounds = font.bounding_box();
		_height = bounds.max.y - bounds.min.y;
		_baseline = -bounds.min.y;
	}
}

void FontItemRenderer::render(const ItemRenderer::Info &info)
{
	std::string t = _value_provider->value(info.index);
	if (!t.empty() && _font.is_valid())
	{
		if (info.selected)
		{
			_font.set_colours(tbx::Colour::white, tbx::Colour::black);
		} else
		{
			_font.set_colours(tbx::Colour::black, tbx::Colour::white);
		}
		_font.paint(info.screen.x, info.screen.y+_baseline, t, tbx::Font::FPF_OSUNITS);
	}
}

unsigned int FontItemRenderer::width(unsigned int index) const
{
  std::string t = _value_provider->value(index);
  return _font.is_valid() ? _font.string_width_os(t) : 32 * t.size();
}

tbx::Size FontItemRenderer::size(unsigned int index) const
{
	return tbx::Size(width(index), _height);
}

} /* namespace view */
} /* namespace tbx */
