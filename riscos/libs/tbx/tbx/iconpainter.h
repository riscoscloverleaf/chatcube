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

#ifndef ICONPAINTER_H_
#define ICONPAINTER_H_

#include "redrawlistener.h"
#include <string>

namespace tbx {

class SpriteArea;

/**
 * Class to draw an icon in a redraw listener.
 *
 * The IconPainter can either be added to the window
 * directly as it is a RedrawListener or the redraw
 * method can be called using the redraw parameter
 * from another redraw listener for the window.
 */
class IconPainter: public tbx::RedrawListener
{
private:
	unsigned int _flags;
	BBox _bounds;
	std::string _text;
	std::string _validation;
	SpriteArea *_sprite_area;

public:
	IconPainter();
	virtual ~IconPainter();

    /**
     * Reference to the bounds of the icon.
     *
     * The bounds are in work area coordinates
     */
	BBox &bounds() {return _bounds;}

    /**
     * return the bounds of the icon in work area coordinates
     */
	const BBox &bounds() const {return _bounds;}

	IconPainter &text(std::string text);
	IconPainter &validation(std::string val);
	IconPainter &sprite(std::string sprite);
	IconPainter &sprite_area(SpriteArea *sprite_area);
	IconPainter &use_wimp_sprite_area();
	IconPainter &use_client_sprite_area();

    /**
     * Set icon as a text icon.
     *
     * This flag is automatically set if you use the text method.
     */
	IconPainter &text_icon(bool t) 		{if (t) _flags |= 1; else _flags &=~1; return *this;}

    /**
     * Check if this is a text icon
     */
    bool text_icon() const {return (_flags & 1)!=0;}

	/**
	 * Set icon as a sprite icon.
	 */
	IconPainter &sprite_icon(bool s) 	{if (s) _flags |= 2; else _flags &=~2; return *this;}
	/**
	 * Check if this is a sprite icon
	 */
	 bool sprite_icon() const {return (_flags & 2)!=0;}

	/**
	 * Add a border to the icon
	 */
	IconPainter &border(bool f) 		{if (f) _flags |= 4; else _flags &=~4; return *this;}
	/**
	 * Set horizontally centred flag
	 *
	 * @param f true to horizontally centre the icon
	 */
	IconPainter &hcentred(bool f) 		{if (f) _flags |= 8; else _flags &=~8; return *this;}
	/**
	 * Set vertically centred flag
	 *
	 * @param f true to vertically centre the icon
	 */
	IconPainter &vcentred(bool f) 		{if (f) _flags |= (1<<4); else _flags &=~(1<<4); return *this;}
	/**
	 * Set filled flag
	 *
	 * @param f true to fill the icon
	 */
	IconPainter &filled(bool f) 		{if (f) _flags |= (1<<5); else _flags &=~(1<<5); return *this;}
	/**
	 * Set right justified flag
	 *
	 * @param f true to right justify the icon
	 */
	IconPainter &rjust(bool f) 			{if (f) _flags |= (1<<9); else _flags &=~(1<<9); return *this;}
	/**
	 * Set show sprite at half size flag
	 *
	 * @param f true to show the sprite half size
	 */
	IconPainter &half_sprite(bool f) 	{if (f) _flags |= (1<<11); else _flags &=~(1<<11); return *this;}
	/**
	 * Set inverted flag
	 *
	 * @param f true to show icon inverted
	 */
	IconPainter &inverted(bool f) 		{if (f) _flags |= (1<<21); else _flags &=~(1<<21); return *this;}
	/**
	 * Set shaded flag
	 *
	 * @param f true to show the icon shaded
	 */
	IconPainter &shaded(bool f) 		{if (f) _flags |= (1<<22); else _flags &=~(1<<28); return *this;}
	/**
	 * Set foreground colour of sprite
	 *
	 * @param col WimpColour for foreground
	 */
	IconPainter &foreground(unsigned int col)	{_flags = (_flags & ~0x0F000000) | (col << 24); return *this;}
	/**
	 * Set background colour of sprite
	 *
	 * @param col WimpColour for background
	 */
	IconPainter &background(unsigned int col)	{_flags = (_flags & ~0xF0000000) | (col << 28); return *this;}

	/**
	 * Text and sprite left justified, text at bottom, sprite at top
	 * for text and sprite icons.
	 */
	IconPainter &text_and_sprite_left() {return hcentred(0).vcentred(0).rjust(0);}

	/**
	 * Text and sprite right justified, text at bottom, sprite at top
	 * for text and sprite icons.
	 */
	IconPainter &text_and_sprite_right() {return hcentred(0).vcentred(0).rjust(1);}

	/**
	 * Sprite at left, text +12 units right of it, text and sprite vertically centred
	 * for text and sprite icons.
	 */
	IconPainter &text_12_sprite_left() {return hcentred(0).vcentred(1).rjust(0);}

	/**
	 * text at left, sprite at right, text and sprite vertically centred
	 * for text and sprite icons.
	 */
	IconPainter &text_left_sprite_right() {return hcentred(0).vcentred(1).rjust(1);}
	/**
	 * text and sprite centred, text at bottom, sprite at top
	 * for text and sprite icons.
	 */
	IconPainter &text_and_sprite_centred() {return hcentred(1).vcentred(0).rjust(0);}
	/**
	 * text and sprite centred  text at top, sprite at bottom
	 * for text and sprite icons.
	 */
	IconPainter &top_text_and_sprite_centred() {return hcentred(1).vcentred(0).rjust(1);}
	/**
	 * text and sprite centred (text on top) text and sprite centred
	 * for text and sprite icons.
	 */
	IconPainter &on_top_text_and_sprite_centred() {return hcentred(1).vcentred(1).rjust(0);}

	/**
	 * text at right, sprite at left text and sprite centred
	 * for text and sprite icons.
	 */
	IconPainter &text_right_sprite_left() {return hcentred(1).vcentred(1).rjust(1);}

	virtual void redraw(const RedrawEvent &event);
};

}

#endif /* ICONPAINTER_H_ */
