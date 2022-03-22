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
 * TextView.cc
 *
 *  Created on: 31-Jul-2009
 *      Author: alanb
 */

#include "textview.h"
#include <cstring>
#include "../font.h"
#include <fstream>

namespace tbx
{
namespace view
{

const char ROW_HEIGHT = 40;

/**
 * Construct a text view.
 *
 * Default to black text on with a white background text colour
 *
 * @param window to show it in
 * @param wrap true to wrap to window width. Default false
 */
TextView::TextView(tbx::Window window, bool wrap /*= false*/) :
	_window(window),
	_wrap(wrap),
	_text(0),
	_size(0),
	_width(0),
	_foreground(tbx::Colour::black),
	_background(tbx::Colour::white)
{
	_window.add_redraw_listener(this);
	if (_wrap) _window.add_open_window_listener(this);
}

/**
 * Destructor removes listeners from window and deletes the text
 */
TextView::~TextView()
{
	_window.remove_open_window_listener(this);
	delete [] _text;
}

/**
 * Set the text for the text view
 *
 * @param text pointer to zero terminated text
 */
void TextView::text(const char *text)
{
	_size = std::strlen(text);
	delete [] _text;
	if (_size)
	{
		_text = new char[_size+1];
		std::strcpy(_text, text);
	}
	else _text = 0;
	recalc_layout();
}

/**
 * Set the text for the text view
 *
 * @param text new text for the text view
 */
void TextView::text(const std::string &text)
{
	_size = text.size();
	delete [] _text;
	if (_size)
	{
		_text = new char[_size+1];
		text.copy(_text, _size);
		_text[_size] = 0;
	}
	else _text = 0;
	recalc_layout();
}

/**
 * Set wrap to window width on or off
 *
 * @param w set to true to turn on wrapping.
 */
void TextView::wrap(bool w)
{
	if (w != _wrap)
	{
		_wrap = w;
		recalc_layout();
		if (w) _window.add_open_window_listener(this);
		else _window.remove_open_window_listener(this);
	}
}

/**
 * Change the margin
 *
 * @param margin new margin
 */
void TextView::margin(const tbx::Margin &margin)
{
	_margin = margin;
	update_window_extent();
	refresh();
}

/**
 * Set the current background colour for the text
 *
 * Note: This colour is used for painting the text only
 * it does not fill the text view area.
 *
 * @param colour new background colour
 */
void TextView::background(tbx::Colour colour)
{
	if (colour != _background)
	{
		_background = colour;
		if (_text && _size) refresh();
	}
}
/**
 * Set the current foreground colour for the text
 *
 * @param colour new foreground colour
 */
void TextView::foreground(tbx::Colour colour)
{
	if (colour != _foreground)
	{
		_foreground = colour;
		if (_text && _size) refresh();
	}
}

/**
 * Update the Window extent after a change in size of text.
 */
void TextView::update_window_extent()
{
	WindowState state;
	_window.get_state(state);
	update_window_extent(state.visible_area().bounds());
}

/**
 * Update the Window extent after a change in size.
 *
 * @param visible_bounds visible area of window on screen
 */
void TextView::update_window_extent(const BBox &visible_bounds)
{
	int width = _width + _margin.left + _margin.right;
	int height = _line_end.size() * ROW_HEIGHT + _margin.top + _margin.bottom;

	if (_wrap || width < visible_bounds.width())
		width = visible_bounds.width();

	if (height < visible_bounds.height())
	   height = visible_bounds.height();

	BBox extent(0,-height, width, 0);
	_window.extent(extent);
}

/**
 * Refresh the whole report view.
 *
 * It should also be called if all the data changes.
 */
void TextView::refresh()
{
	BBox all(_margin.left, -_margin.top - _line_end.size() * ROW_HEIGHT,
			_margin.left + _width, -_margin.top);
	_window.force_redraw(all);
}

/**
 * Redraw the window
 *
 * @param event details on what needs to be redrawn
 */
void TextView::redraw(const RedrawEvent &event)
{
	if (_text == 0 || _size == 0) return; // Nothing to draw

	BBox work_clip = event.visible_area().work(event.clip());

	unsigned int first_row = (-work_clip.max.y - _margin.top) / ROW_HEIGHT;
	unsigned int last_row =  (-work_clip.min.y - _margin.top) / ROW_HEIGHT;

	if (first_row < 0) first_row = 0;
	if (last_row < 0) return; // Nothing to draw

	if (first_row >= _line_end.size()) return; // Nothing to draw
	if (last_row >= _line_end.size()) last_row = _line_end.size() - 1;

	tbx::WimpFont font;
	font.set_colours(_foreground, _background);

	int x = event.visible_area().screen_x(_margin.left);
	int y = event.visible_area().screen_y(-_margin.top - (first_row+1) * ROW_HEIGHT) + 4;
	int start = (first_row == 0) ? 0 : _line_end[first_row-1];

	for (unsigned int row = first_row; row <= last_row; row++)
	{
		int end = _line_end[row];
		if (_text[start] == '\n') start++;
		else if (_wrap && _text[start] == ' ' && start > 0 && _text[start-1] != '\n') start++;
		if (start < end)
		{
			font.paint(x, y+8, _text + start, end-start);
		}
		start = end;
		y -= ROW_HEIGHT;
	}
}

/**
 * Window has been opened or resized, so re do layout
 *
 * @param event details on open window event
 */
void TextView::open_window(OpenWindowEvent &event)
{
	recalc_layout(event.visible_area());
}

/**
 * Recalculate the text layout after text is changed or
 * window resized.
 */
void TextView::recalc_layout()
{
	WindowState state;
	_window.get_state(state);
	recalc_layout(state.visible_area().bounds());
}

/**
 * Recalculate the text layout after text is changed or
 * window resized.
 *
 * @param visible_bounds new visible area of the window
 */
void TextView::recalc_layout(const BBox &visible_bounds)
{
	_line_end.clear();
	tbx::Font font; // Can't use WimpFont as it doesn't have split function
	font.desktop_font();

	unsigned int refresh_width = _width, refresh_lines = _line_end.size();

	if (_wrap)
	{
		// Wrap to window
		int pos = 0;
		int start;
		_width = visible_bounds.width() - _margin.left - _margin.right;

		while (pos < (int)_size)
		{
			start = pos;
			pos = font.find_split_os(_text + start, -1, _width, ' ') + start;
			if (pos == start)
			{
				pos = font.find_index_xy_os(_text + start, -1, _width, 0) + start;
				if (pos == start) pos++;
			}
			std::string temp(_text+start, pos - start + 1);
			_line_end.push_back(pos);
			if (_text[pos] == '\n') pos++;
		}
	} else
	{
		char *p = _text;
		char *start;
		char *end = p + _size;
		unsigned int line_width;
		_width = 0;

		while (p != end)
		{
			start = p;
			p = std::strchr(start, '\n');
			if (p == 0) p = end;
			_line_end.push_back(p - _text);
			line_width = font.string_width_os(start, p - start);
			if (line_width > _width) _width = line_width;
			if (p != end) p++;
		}
	}

	update_window_extent(visible_bounds);

	if (_width > refresh_width) refresh_width = _width;
	if (_line_end.size() > refresh_lines) refresh_lines = _line_end.size();
	BBox all(_margin.left, -_margin.top - refresh_lines * ROW_HEIGHT,
			_margin.left + refresh_width, -_margin.top);
	_window.force_redraw(all);
}

/**
 * Load text for view from file.
 *
 * Replace all control codes (character < ASCII 32) except line feed (10)
 * with a space.
 *
 * @param file_name - name of file to load
 * @returns true if file loaded OK.
 */
bool TextView::load_file(const std::string &file_name)
{
	std::ifstream is(file_name.c_str());
	bool loaded = false;
	_size = 0;
	delete [] _text;
	_text = 0;

	if (is)
	{
		is.seekg (0, std::ios::end);
		_size = is.tellg();
		is.seekg (0, std::ios::beg);

		_text = new char[_size+1];
		is.read (_text,_size);
		_text[_size] = 0;
		if (!is.fail())
		{
			loaded = true;
			// Replace all control codes except "\n" with a space
			for (unsigned int c = 0; c < _size; c++)
			{
				if (_text[c] < 32 && _text[c] != '\n') _text[c] = ' ';
			}
		}
		is.close();
	}

	recalc_layout();

	return loaded;
}

}
}
