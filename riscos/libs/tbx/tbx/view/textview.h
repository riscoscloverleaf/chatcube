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
 * TextView.h
 *
 *  Created on: 31-Jul-2009
 *      Author: alanb
 */

#ifndef TBX_TEXTVIEW_H_
#define TBX_TEXTVIEW_H_

#include "../window.h"
#include "../redrawlistener.h"
#include "../openwindowlistener.h"
#include "../margin.h"
#include "../colour.h"
#include <vector>

namespace tbx
{
namespace view
{
/**
 * Class to display text in a window
 */
class TextView :
	public tbx::RedrawListener,
	tbx::OpenWindowListener
{
private:
	tbx::Window _window;
	tbx::Margin _margin;
	bool _wrap;
	char *_text;
	unsigned int _size;
	std::vector<unsigned int> _line_end;
	unsigned int _width;
	tbx::Colour _foreground;
	tbx::Colour _background;

public:
	TextView(tbx::Window window, bool wrap = false);
	virtual ~TextView();

	void update_window_extent();
	void refresh();

	/**
	 * Get the window the text view is on
	 *
	 * @returns Window that displays the text view
	 */
	tbx::Window &window() {return _window;}

	/**
	 * Get the margin between the text view contents and the
	 * edge of the Window.
	 *
	 * @returns the margin
	 */
	const tbx::Margin &margin() const {return _margin;}
	void margin(const tbx::Margin &margin);

	/**
	 * Get a pointer to the text.
	 *
	 * @returns a zero terminated pointer to the text
	 */
	const char *text() const {return _text;}
	void text(const char *text);
	void text(const std::string &text);

	/**
	 * Get the current size of the text
	 *
	 * @returns size in bytes of the text
	 */
    unsigned int size() const {return _size;}

    /**
     * Get current background colour for the text
     *
     * @returns background colour
     */
    tbx::Colour background() const {return _background;}
    /**
     * Set the current background colour for the text
     *
     * @param colour new background colour
     */
    void background(tbx::Colour colour);
    /**
     * Get current foreground colour for the text
     *
     * @returns foreground colour
     */
    tbx::Colour foreground() const {return _foreground;}
    /**
     * Set the current foreground colour for the text
     *
     * @param colour new foreground colour
     */
    void foreground(tbx::Colour colour);


    bool load_file(const std::string &file_name);

    /**
     * Check it text view is set to wrap text
     *
     * @returns true if text view will wrap text
     */
    bool wrap() const {return _wrap;}
    void wrap(bool w);

	// Redraw listener override
	virtual void redraw(const tbx::RedrawEvent &event);
	virtual void open_window(tbx::OpenWindowEvent &event);

private:
	void update_window_extent(const BBox &visible_bounds);
	void recalc_layout();
	void recalc_layout(const BBox &visible_bounds);
};

}
}
#endif /* TBX_TEXTVIEW_H_ */
