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

#include "iconpainter.h"
#include "swixcheck.h"
#include "application.h"
#include "sprite.h"
#include <swis.h>

namespace tbx {

/**
 * Construct an icon painter.
 *
 * By default the icon painter is set to wimp colour 7 on 0 with no
 * content
 */
IconPainter::IconPainter() : _flags(0x07000100), _bounds(0,-64,64,0), _sprite_area(0)
{
}

/**
 * Icon painter destructor
 */
IconPainter::~IconPainter() {
}

/**
 * Set the text for the icon painter.
 *
 * This automatically sets the icon_text flag
 */
IconPainter &IconPainter::text(std::string text)
{
  text_icon(true);
  _text = text;
   return *this;
}

/**
 * Set the validation string for the icon.
 *
 * See the RISC OS PRM for details.
 *
 * Note: This can replace the sprite if not set
 */
IconPainter &IconPainter::validation(std::string val)
{
    _validation = val;
    return *this;
}

/**
 * Set the sprite for the icon.
 *
 * Replaces the validiation string with the sprite name and
 * sets the sprite_icon flag
 */
IconPainter &IconPainter::sprite(std::string sprite)
{
    sprite_icon(true);
    _validation = "S"+sprite;

	return *this;
}

/**
 * Set the sprite area to use for the icon.
 *
 * If the sprite area is set the icon can not
 * display text as well.
 *
 * Call use_wimp_sprite_area to reset.
 */
IconPainter &IconPainter::sprite_area(SpriteArea *sprite_area)
{
	sprite_icon(true);
	text_icon(false);
	_sprite_area = sprite_area;

	return *this;
}

/**
 * Use sprite from WIMP sprite pool. Text can also
 * be shown if the sprite if from the WIMP sprite pool.
 */
IconPainter &IconPainter::use_wimp_sprite_area()
{
	sprite_icon(true);
	_sprite_area = 0;

    return *this;
}

/**
 * Use application client area for the sprite.
 *
 * Note: You can not display text as will for this icon type.
 */
IconPainter &IconPainter::use_client_sprite_area()
{
	sprite_icon(true);
	text_icon(false);
	_sprite_area = app()->sprite_area();

    return *this;
}

/**
 * Redraw the icon
 */
void IconPainter::redraw(const RedrawEvent &event)
{
	int icon_data[8];
	icon_data[0] = _bounds.min.x;
	icon_data[1] = _bounds.min.y;
	icon_data[2] = _bounds.max.x;
	icon_data[3] = _bounds.max.y;
	icon_data[4] = _flags;

    if (text_icon())
    {
        icon_data[5]  = reinterpret_cast<int>(_text.c_str());
        icon_data[6]  = reinterpret_cast<int>(_validation.c_str());
        icon_data[7] = _text.size();
    } else if (sprite_icon())
    {
        // Sprite only icon
    	if (_validation.size() > 2)
    	{
			icon_data[5] = reinterpret_cast<int>(_validation.c_str()+1);
			icon_data[6] = reinterpret_cast<int>((_sprite_area) ? _sprite_area->pointer() : WIMP_SPRITEAREA); // 1 = use WIMP sprite area
			icon_data[7] = _validation.size() - 1;
    	}
    }

	_swix(Wimp_PlotIcon, _IN(1), icon_data);
}

}
