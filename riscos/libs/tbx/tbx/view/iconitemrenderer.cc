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
 * iconitemrenderer.cc
 *
 *  Created on: 22 Apr 2010
 *      Author: alanb
 */

#include "iconitemrenderer.h"
#include "../iconpainter.h"
#include "../font.h"
#include "../sprite.h"
#include "../application.h"

//#define CHECK_SIZES
#ifdef CHECK_SIZES
#include "../graphics.h"
#endif

namespace tbx {

namespace view {

const int IV_MARGIN = 8; // Margin on each side of cell
const int IV_GAP = 8;    // Gap between sprite and text

/**
 * Protected constructor for derived renderer which must provide
 * the text() and sprite_name() overrides.
 *
 * @param use_client true to use client sprite area, false for wimp sprite area
 *        (default true)
 */
IconItemRenderer::IconItemRenderer(bool use_client /*= true*/) :
		TypedItemRenderer<std::string>(0),
		_sprite_name_provider(0),
		_use_client_area(use_client)
{
}

/**
 * Constructor for IconItemRenderer that uses the same value provider
 * to give the text and sprite name. i.e. They are the same.
 *
 * @param np value provider to return the name used for the text and sprite
 * @param use_client true to use client sprite area, false for wimp sprite area
 *        (default true)
 */
IconItemRenderer::IconItemRenderer(ItemViewValue<std::string> *np, bool use_client /*= true*/) :
		TypedItemRenderer<std::string>(np),
		_sprite_name_provider(np),
		_use_client_area(use_client)
{
}
/**
 * Constructor for IconItemRenderer that uses the different value providers
 * to give the text and sprite name.
 *
 * @param np value provider to return the text
 * @param sp value provider to return the sprite name
 * @param use_client true to use client sprite area, false for wimp sprite area
 *        (default true)
 */
IconItemRenderer::IconItemRenderer(ItemViewValue<std::string> *np, ItemViewValue<std::string> *sp, bool use_client /*= true*/) :
		TypedItemRenderer<std::string>(np),
		_sprite_name_provider(sp),
		_use_client_area(use_client)
{
}

IconItemRenderer::~IconItemRenderer()
{
}

/**
 * Return the text for the icon
 *
 * By default uses the value provider from the constructor
 * Override to get the value in a different way
 */
std::string IconItemRenderer::text(unsigned int index) const
{
	if (_value_provider) return _value_provider->value(index);
	return std::string();
}

/**
 * Return the sprite name for the icon
 *
 * By default uses the value provider from the constructor
 * Override to get the value in a different way
 */
std::string IconItemRenderer::sprite_name(unsigned int index) const
{
	if (_sprite_name_provider) return _sprite_name_provider->value(index);
	else return text(index);
}

/**
 * Return standard size for an icon with the given text width and sprite height
 *
 * text_width width of text in os units. Defaults to 160 (approx 10 chars)
 * sprite_height height of sprite in os units. Defaults to 68 (standard height in filer)
 */
tbx::Size IconItemRenderer::standard_size(unsigned int text_width /*= 160*/, unsigned int sprite_height /*= 68*/)
{
	Size size;
	size.width = text_width + IV_MARGIN * 2;
	size.height = sprite_height
	                   + 32 /* text */
	                   + IV_GAP
	                   + IV_MARGIN * 2;
	return size;
}


/* Render the icon
 *
 * The item is only clipped to the containing Windows visible area,
 * not to the cell bounds so the content must be drawn entirely
 * inside the cell bounding box or the clip area should be set
 * before drawing.
 *
 * @param info Information on redraw event and item to be redrawn
 */
void IconItemRenderer::render(const ItemRenderer::Info &info)
{
	BBox content(info.bounds);
	content.inflate(-IV_MARGIN); // Reduce size for border

	if (use_client_sprite_area(info.index))
	{
		// Can't do sprite and text icon for user client area so
		// do two paints.
		IconPainter ip;
		ip.inverted(info.selected);
		ip.bounds() = content;
		ip.bounds().max.y = content.min.y + 32;
		ip.hcentred(true);

		// Paint text at bottom
		ip.text(text(info.index));
		ip.redraw(info.redraw);

		// Paint sprite at top
		ip.text_icon(false);
		ip.sprite(sprite_name(info.index));
		ip.use_client_sprite_area();
		ip.bounds().max.y = content.max.y;
		ip.bounds().min.y = content.max.y - 68;
		ip.redraw(info.redraw);
	} else
	{
		// WIMP sprite can be done with one Icon
		IconPainter ip;
		ip.text(text(info.index)).sprite(sprite_name(info.index));
		ip.text_and_sprite_centred();
		ip.inverted(info.selected);
		ip.bounds() = content;
		ip.redraw(info.redraw);
	}

#ifdef CHECK_SIZES
	Graphics g;

	BBox sbounds;
	info.redraw.visible_area().screen(content, sbounds);

	g.set_wimp_colour(WimpColour::grey2);
	g.rect(sbounds);

	BBox sbox;
	get_sprite_box(info.index, sbox);
	sbox.move(info.screen.x, info.screen.y);

	g.set_wimp_colour(WimpColour::red);
	g.rect(sbox);

	BBox tbox;
	get_text_box(info.index, tbox);
	tbox.move(info.screen.x, info.screen.y);

	g.set_wimp_colour(WimpColour::dark_green);
	g.rect(tbox);
#endif

}

/**
 * Return the width of the given item
 */
unsigned int IconItemRenderer::width(unsigned int index) const
{
	return size(index).width;
}

/**
 * Return height of item
 */
unsigned int IconItemRenderer::height(unsigned int index) const
{
	return size(index).height;
}

/**
 * Called to get the size of the item.
 *
 * Should return the equivalent of Size(width(index), height(index))
 */
Size IconItemRenderer::size(unsigned int index) const
{
	std::string t = text(index);
	unsigned int text_width = 0;
	if (!t.empty())
	{
		WimpFont font;
		text_width = font.string_width_os(t);
	}

	Size sprite_size(68,68);
	if (use_client_sprite_area(index))
	{
		UserSprite us(app()->sprite_area()->get_sprite(sprite_name(index)));
		if (us.is_valid()) sprite_size = us.size();
	} else
	{
		WimpSprite ws(sprite_name(index));
		if (ws.exist()) sprite_size = ws.size();
	}
	if ((int)text_width < sprite_size.width) text_width = sprite_size.width;

	return standard_size(text_width, sprite_size.height);
}

/**
 * Check if the point given hits content of the cell rather
 * then a margin or background.
 *
 * @param index index of item to check
 * @param size of area item is rendered in
 * @param pos location to check relative to bottom left of cell
 * @return true if content is at that location, otherwise false
 */
bool IconItemRenderer::hit_test(unsigned int index, const Size &size, const Point &pos) const
{
	// Quick test
	if (pos.x < IV_MARGIN || pos.y < IV_MARGIN
		|| pos.x > size.width - IV_MARGIN
		|| pos.y > size.height - IV_MARGIN)
		return false;

	// Check sprite box
	BBox check;
	get_sprite_box(index, size, check);

	if (check.contains(pos)) return true;

	// Finally check text
	get_text_box(index, size, check);
	return check.contains(pos);
}

/**
 * Check if the given rectangle intersects the content of the cell
 * rather than a margin or background.
 *
 * @param index index of item to  check
 * @param size of area item is rendered in
 * @param box box to check if it intersect the cell
 * @return true if some contents are in the box, otherwise false
 */
bool IconItemRenderer::intersects(unsigned int index, const Size &size, const BBox &box) const
{
	// Quick test intersects if mid point is in box
	if (box.contains(size.width/2, size.height/2)) return true;

	// Check sprite
	BBox check;
	get_sprite_box(index, size, check);
	if (check.intersects(box)) return true;

	// Check text
	get_text_box(index, size, check);
	return check.intersects(box);
}

/**
 * Gets sprite box relative to bottom left corner
 */
void IconItemRenderer::get_sprite_box(unsigned int index, const Size &size, BBox &sbox) const
{
	Size sprite_size;

	if (use_client_sprite_area(index))
	{
		UserSprite us(app()->sprite_area(), sprite_name(index));
		sprite_size = us.size();
	} else
	{
		WimpSprite ws(sprite_name(index));
		sprite_size = ws.size();
	}

	sbox.min.x = (size.width - sprite_size.width) / 2;
	sbox.max.x = sbox.min.x + sprite_size.width;
	sbox.max.y = size.height - IV_MARGIN;
	sbox.min.y = sbox.max.y - sprite_size.height;
}

/**
 * Get text box relative to bottom left corner
 */
void IconItemRenderer::get_text_box(unsigned int index, const Size &size, BBox &tbox) const
{
	WimpFont wf;
	Size text_size;
	text_size.width = wf.string_width_os(text(index));
	text_size.height = wf.height_os();

	tbox.min.x = (size.width - text_size.width) / 2;
	tbox.max.x = tbox.min.x + text_size.width;
	tbox.min.y = IV_MARGIN;
	tbox.max.y = tbox.min.y + text_size.height;
}


}

}
