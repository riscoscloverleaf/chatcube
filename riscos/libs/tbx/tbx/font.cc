/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2014 Alan Buckley   All Rights Reserved.
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

#include "font.h"
#include "kernel.h"
#include "swixcheck.h"
#include "swis.h"

using namespace tbx;

Font::FontRef *Font::s_invalid_font_ref = new FontRef(0);
// Desktop font handle is filled in later, but is kept global so
// the font is never released.
Font::FontRef *Font::s_desktop_font_ref = new FontRef(0);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * Construct an uninitialised font.
 *
 * Copy another font to it or use the find method before using
 * the font.
 */
Font::Font()
{
	_font_ref = s_invalid_font_ref; // Null constructor gives system font
	_font_ref->add_ref();
}

/**
 * Construct a font from another font.
 *
 * This object will become a reference to the same OS font as the
 * other font.
 * @param other the font to copy
 */
Font::Font(const Font &other)
{
	_font_ref = other._font_ref;
	_font_ref->add_ref();
}

/**
 * Construct a font with the given name and size.
 *
 * You can call the is_valid method to check the font was found.
 *
 * @param fontName name of the font to find
 * @param width - width of the font
 * @param height - height of the font or -1 for same as width
 */
Font::Font(const std::string &fontName, int width, int height /*= -1*/)
{
	_font_ref = s_invalid_font_ref; // Null constructor gives system font
	_font_ref->add_ref();
	find(fontName, width, height);
}

/**
 * Destructor release the OS Font if no other font objects
 * are using it.
 */
Font::~Font()
{
	_font_ref->release();
}

/**
 *  Find the specified font and attach it to this object.
 *
 *  If the font is not found then the currently attached
 *  font is unchanged.
 *
 * @param fontName name of the font to find
 * @param width - width of the font
 * @param height - height of the font or -1 for same as width
 * @returns true if the font has found.
 */
bool Font::find(const std::string &fontName, int width, int height /*= -1*/)
{
	_kernel_swi_regs regs;

	regs.r[0] = 0;
	regs.r[1] = reinterpret_cast<int>(fontName.c_str());
	regs.r[2] = width;
	if (height == -1) regs.r[3] = width;
	else regs.r[3] = height;
	regs.r[4] = 0;
	regs.r[5] = 0;

	if (_kernel_swi(0x40081, &regs, &regs) == 0)
	{
		_font_ref->release();
		_font_ref = new FontRef(regs.r[0]);

		return true;
	}

	return false;
}

/**
 * Sets font to current desktop font.
 *
 * Using this give the full flexibility of the font system to
 * display and measure characters in the desktop font. For simple single
 * line text the WimpFont provides a simpler alternative
 *
 * @returns true if desktop font can be read and isn't the old style system font
 */
bool Font::desktop_font()
{
	int font_handle;
	if (_swix(Wimp_ReadSysInfo, _IN(0)|_OUT(0), 8, &font_handle) == 0
			&& font_handle != 0)
	{
		_font_ref->release();
		s_desktop_font_ref->handle = font_handle;
		_font_ref = s_desktop_font_ref;
		_font_ref->add_ref();
		return true;
	}
	return false;
}

/**
 *   Paint the font to the screen.
 *
 *@param x left position
 *@param y position of base line
 *@param text text to paint to the screen
 *@param flags paint flags, can NONE or one or more of the following:
 *   JUSTIFY - justify the text between x,y and the last graphics move position
 *   RUBOUT	 - rub out area behind text, defined be last two graphics move positions
 *   OSUNITS - x and y are specified as OS units, if not they are in millipoints
 */
void Font::paint(int x, int y, const std::string &text, int flags /*= Font::NONE*/) const
{
	_kernel_swi_regs regs;
	regs.r[0] = _font_ref->handle;
	regs.r[1] = reinterpret_cast<int>(text.c_str());
	regs.r[2] = flags;
	regs.r[2] |= 256 | 512; // Always use handle in r0 and kerning is on
	regs.r[3] = x;
	regs.r[4] = y;

	// Font_Paint
	swix_check(_kernel_swi(0x40086, &regs, &regs));
}

/**
 *   Paint the font to the screen.
 *
 *@param x left position
 *@param y position of base line
 *@param text text to paint to the screen
 *@param length length of text to paint or -1 for all
 *@param flags paint flags, can NONE or one or more of the following:
 *   JUSTIFY - justify the text between x,y and the last graphics move position
 *   RUBOUT	 - rub out area behind text, defined be last two graphics move positions
 *   OSUNITS - x and y are specified as OS units, if not they are in millipoints
 */
void Font::paint(int x, int y, const char *text, int length /*= -1*/, int flags /*= Font::NONE*/) const
{
	if (length > 0) flags |= (1<<7); // Use string length

	_kernel_swi_regs regs;
	regs.r[0] = _font_ref->handle;
	regs.r[1] = reinterpret_cast<int>(text);
	regs.r[2] = flags;
	regs.r[2] |= 256 | 512; // Always use handle in r0 and kerning is on
	regs.r[3] = x;
	regs.r[4] = y;
	regs.r[7] = length;

	// Font_Paint
	swix_check(_kernel_swi(0x40086, &regs, &regs));
}


/**
 *   Get the width of a string.
 *
 *@param text the string to measure
 *@returns length of string in millipoints
 */
int Font::string_width_mp(const std::string &text) const
{
	_kernel_swi_regs regs;
	regs.r[0] = _font_ref->handle;
	regs.r[1] = reinterpret_cast<int>(text.c_str());
	regs.r[2] = 256 | 512; // Handle is in r0 and kerning is on
	regs.r[3] = 0x70000000;
	regs.r[4] = -0x70000000;

	// Font_ScanString
	swix_check(_kernel_swi(0x400A1, &regs, &regs));

	return regs.r[3];
}

/**
 * Get width of string in millipoints
 *
 * @param text string to measure
 * @param length length of string or -1 to measure to terminating char 0
 * @returns width of string in millipoints
 */
int Font::string_width_mp(const char *text, int length /*= -1*/) const
{
	_kernel_swi_regs regs;
	regs.r[0] = _font_ref->handle;
	regs.r[1] = reinterpret_cast<int>(text);
	regs.r[2] = 256 | 512; // Handle is in r0 and kerning is on
	regs.r[3] = 0x70000000;
	regs.r[4] = -0x70000000;
	if (length > 0) regs.r[2] |= (1<<7); // Use string length
	regs.r[7] = length;

	// Font_ScanString
	swix_check(_kernel_swi(0x400A1, &regs, &regs));

	return regs.r[3];
}

/**
 *   Get the width of a string.
 *
 *@param text the string to measure
 *@returns length of string in OS units
 */
int Font::string_width_os(const std::string &text) const
{
	_kernel_swi_regs regs;
	regs.r[0] = _font_ref->handle;
	regs.r[1] = reinterpret_cast<int>(text.c_str());
	regs.r[2] = 256 | 512; // Handle is in r0 and kerning is on
	regs.r[3] = 0x70000000;
	regs.r[4] = -0x70000000;

	// Font_ScanString
	swix_check(_kernel_swi(0x400A1, &regs, &regs));

	// 400 OS units to the millipoint
	return (regs.r[3] + 399) / 400;
}

/**
 * Get width of string in os units
 *
 * @param text string to measure
 * @param length length of string or -1 to measure to terminating char 0
 * @returns width of string in os units
 */
int Font::string_width_os(const char *text, int length /*= -1*/) const
{
	_kernel_swi_regs regs;
	regs.r[0] = _font_ref->handle;
	regs.r[1] = reinterpret_cast<int>(text);
	regs.r[2] = 256 | 512; // Handle is in r0 and kerning is on
	regs.r[3] = 0x70000000;
	regs.r[4] = -0x70000000;
	if (length > 0) regs.r[2] |= (1<<7); // Use string length
	regs.r[7] = length;

	// Font_ScanString
	swix_check(_kernel_swi(0x400A1, &regs, &regs));

	// 400 OS units to the millipoint
	return (regs.r[3] + 399) / 400;
}

/**
 * Get height of string in os units
 *
 * @param text string to measure
 * @param length length of string or -1 to measure to terminating char 0
 * @param max_width maximum width of string is os units or -1 for maximum possible
 * @returns height of string in os units
 */
int Font::string_height_os(const char *text, int length /*= -1*/, int max_width /* = -1 */) const
{
	_kernel_swi_regs regs;
	regs.r[0] = _font_ref->handle;
	regs.r[1] = reinterpret_cast<int>(text);
	regs.r[2] = 256 | 512; // Handle is in r0 and kerning is on
	regs.r[3] = (max_width == -1) ? 0x70000000 : max_width * 400;
	regs.r[4] = -0x70000000;
	if (length > 0) regs.r[2] |= (1<<7); // Use string length
	regs.r[7] = length;

	// Font_ScanString
	swix_check(_kernel_swi(0x400A1, &regs, &regs));

	// 400 OS units to the millipoint
	return (-regs.r[4] + 399) / 400;
}

/**
 * Get height of string in os units
 *
 * @param text string to measure
 * @param max_width maximum width of string is os units or -1 for maximum possible
 * @returns height of string in os units
 */
int Font::string_height_os(const std::string &text, int max_width /*= -1*/) const
{
	_kernel_swi_regs regs;
	regs.r[0] = _font_ref->handle;
	regs.r[1] = reinterpret_cast<int>(text.c_str());
	regs.r[2] = 256 | 512; // Handle is in r0 and kerning is on
	regs.r[3] = (max_width == -1) ? 0x70000000 : max_width * 400;
	regs.r[4] = -0x70000000;
	regs.r[2] |= (1<<7); // Use string length
	regs.r[7] = text.size();

	// Font_ScanString
	swix_check(_kernel_swi(0x400A1, &regs, &regs));

	// 400 OS units to the millipoint
	return (-regs.r[4] + 399) / 400;
}

// Round millipoint to os units down
inline int round_down_mp_to_os(int mp)
{
	return (mp > 0) ? (mp/400) : ((mp-399)/400);
}

// Round millipoint to os units down
inline int round_up_mp_to_os(int mp)
{
	return (mp > 0) ? ((mp+399)/400) : (mp/400);
}

/**
 * Get size of string in os units
 *
 * @param text string to measure
 * @param max_width maximum width of string is os units or -1 for maximum possible
 * @returns size of string in os units
 */
tbx::Size Font::string_size_os(const std::string &text, int max_width /*= -1*/) const
{
	_kernel_swi_regs regs;
	int block[9];
	regs.r[0] = _font_ref->handle;
	regs.r[1] = reinterpret_cast<int>(text.c_str());
	regs.r[2] = 256 | 512; // Handle is in r0 and kerning is on
	regs.r[3] = (max_width == -1) ? 0x70000000 : max_width * 400;
	regs.r[4] = -0x70000000;
	regs.r[2] |= (1<<7); // Use string length
	regs.r[7] = text.size();
	regs.r[2] |= (1<<5) | (1<< 18); // use r5 return bounding block
	regs.r[5] = reinterpret_cast<int>(block);
	block[0] = block[1] = 0; // additional x, y offset on space
	block[2] = block[3] = 0; // additional x, y offset between each letter
	block[4] = -1;           // split character (-1 => none)

	// Font_ScanString
	swix_check(_kernel_swi(0x400A1, &regs, &regs));

	// convert to os units
	tbx::BBox box;
	box.min.x = round_down_mp_to_os(block[5]);
	box.min.y = round_down_mp_to_os(block[6]);
	box.max.x = round_up_mp_to_os(block[7]);
	box.max.y = round_up_mp_to_os(block[8]);

	return box.size();
}

/**
 * Get size of string in os units
 *
 * @param text string to measure
 * @param length length of string or -1 to measure to terminating char 0
 * @param max_width maximum width of string is os units or -1 for maximum possible
 * @returns size of of string in os units
 */
tbx::Size Font::string_size_os(const char *text, int length /*= -1*/, int max_width /* = -1*/) const
{
	_kernel_swi_regs regs;
	int block[9];
	regs.r[0] = _font_ref->handle;
	regs.r[1] = reinterpret_cast<int>(text);
	regs.r[2] = 256 | 512; // Handle is in r0 and kerning is on
	regs.r[3] = (max_width == -1) ? 0x70000000 : max_width * 400;
	regs.r[4] = -0x70000000;
	if (length > 0) regs.r[2] |= (1<<7); // Use string length
	regs.r[7] = length;
	regs.r[2] |= (1<<5) | (1<< 18); // use r5 and return bounding block
	regs.r[5] = reinterpret_cast<int>(block);
	block[0] = block[1] = 0; // additional x, y offset on space
	block[2] = block[3] = 0; // additional x, y offset between each letter
	block[4] = -1;           // split character (-1 => none)

	// Font_ScanString
	swix_check(_kernel_swi(0x400A1, &regs, &regs));

	// convert to os units
	tbx::BBox box;
	box.min.x = round_down_mp_to_os(block[5]);
	box.min.y = round_down_mp_to_os(block[6]);
	box.max.x = round_up_mp_to_os(block[7]);
	box.max.y = round_up_mp_to_os(block[8]);

	return box.size();
}

/**
 * Get bounding box of a string in os units
 *
 * @param text string to measure
 * @param max_width maximum width of string is os units or -1 for maximum possible
 * @returns bounding box of string in os units
 */
tbx::BBox Font::string_bounds_os(const std::string &text, int max_width /*= -1*/) const
{
	_kernel_swi_regs regs;
	int block[9];
	regs.r[0] = _font_ref->handle;
	regs.r[1] = reinterpret_cast<int>(text.c_str());
	regs.r[2] = 256 | 512; // Handle is in r0 and kerning is on
	regs.r[3] = (max_width == -1) ? 0x70000000 : max_width * 400;
	regs.r[4] = -0x70000000;
	regs.r[2] |= (1<<7); // Use string length
	regs.r[7] = text.size();
	regs.r[2] |= (1<<5) | (1<< 18); // use r5 return bounding block
	regs.r[5] = reinterpret_cast<int>(block);
	block[0] = block[1] = 0; // additional x, y offset on space
	block[2] = block[3] = 0; // additional x, y offset between each letter
	block[4] = -1;           // split character (-1 => none)

	// Font_ScanString
	swix_check(_kernel_swi(0x400A1, &regs, &regs));

	// convert to os units
	tbx::BBox box;
	box.min.x = round_down_mp_to_os(block[5]);
	box.min.y = round_down_mp_to_os(block[6]);
	box.max.x = round_up_mp_to_os(block[7]);
	box.max.y = round_up_mp_to_os(block[8]);

	return box;
}

/**
 * Get bounding box of string in os units
 *
 * @param text string to measure
 * @param length length of string or -1 to measure to terminating char 0
 * @param max_width maximum width of string is os units or -1 for maximum possible
 * @returns bounding box of string in os units
 */
tbx::BBox Font::string_bounds_os(const char *text, int length /*= -1*/, int max_width  /*= -1*/) const
{
	_kernel_swi_regs regs;
	int block[9];
	regs.r[0] = _font_ref->handle;
	regs.r[1] = reinterpret_cast<int>(text);
	regs.r[2] = 256 | 512; // Handle is in r0 and kerning is on
	regs.r[3] = (max_width == -1) ? 0x70000000 : max_width * 400;
	regs.r[4] = -0x70000000;
	if (length > 0) regs.r[2] |= (1<<7); // Use string length
	regs.r[7] = length;
	regs.r[2] |= (1<<5) | (1<< 18); // use r5 and return bounding block
	regs.r[5] = reinterpret_cast<int>(block);
	block[0] = block[1] = 0; // additional x, y offset on space
	block[2] = block[3] = 0; // additional x, y offset between each letter
	block[4] = -1;           // split character (-1 => none)

	// Font_ScanString
	swix_check(_kernel_swi(0x400A1, &regs, &regs));

	// convert to os units
	tbx::BBox box;
	box.min.x = round_down_mp_to_os(block[5]);
	box.min.y = round_down_mp_to_os(block[6]);
	box.max.x = round_up_mp_to_os(block[7]);
	box.max.y = round_up_mp_to_os(block[8]);

	return box;
}

/**
 * Find index where to split a string to fit in the given width.
 *
 * @param text text to measure
 * @param length to text or -1 to measure all
 * @param width to fit text in in os units
 * @param split_char character that causes a split before the line end
 *         or -1 if none.
 *
 * @returns offset of character for split
 */
int Font::find_split_os(const char *text, int length, int width, int split_char /*=-1*/)
{
	_kernel_swi_regs regs;
	int block[8];

	block[0] = 0;
	block[1] = 0;
	block[2] = 0;
	block[3] = 0;
	block[4] = split_char;

	regs.r[0] = _font_ref->handle;
	regs.r[1] = reinterpret_cast<int>(text);
	regs.r[2] = (1<<5); // Use block in r5
	if (length > 0) regs.r[2] |= (1<<7); // Use string length
	regs.r[2] |= 256 | 512; // Always use handle in r0 and kerning is on
	regs.r[3] = os_to_millipoints(width);
	regs.r[4] = 0x7FFFFFFF;
	regs.r[5] = (int)block;
	regs.r[7] = length;

	// Font_ScanString
	swix_check(_kernel_swi(0x400A1, &regs, &regs));

	return (unsigned int)regs.r[1] - (unsigned int)text;
}

/**
 * Find index in a string of the given co-ordinate, rounded to the nearest
 * caret position.
 *
 * @param text text to measure
 * @param length to text or -1 if text is zero terminated
 * @param x x position relative to beginning of string in OS units.
 * @param y y position relative to beginning of string in OS units.
 *
 * @returns index in string of the given position.
 */
int Font::find_index_xy_os(const char *text, int length, int x, int y)
{
	_kernel_swi_regs regs;

	regs.r[0] = _font_ref->handle;
	regs.r[1] = reinterpret_cast<int>(text);
	regs.r[2] = (1<<17); // Return nearest caret position
	if (length > 0) regs.r[2] |= (1<<7); // Use string length
	regs.r[2] |= 256 | 512; // Always use handle in r0 and kerning is on
	regs.r[3] = os_to_millipoints(x);
	regs.r[4] = os_to_millipoints(y);
	regs.r[7] = length;

	// Font_ScanString
	swix_check(_kernel_swi(0x400A1, &regs, &regs));

	return (unsigned int)regs.r[1] - (unsigned int)text;
}

/**
 * Set the colours used to paint this font.
 *
 * The call creates a gradient of colours from the foreground colour in
 * the centre of the letters to the background colour from the current
 * palette.
 * The shades between the colours are used for anti-aliasing which improves
 * the look of the edges on the screen.
 *
 * @param fore The foreground colour
 * @param back The background colour
 * @param colour_offset The number of colours between the foreground and
 * the background. Defaults to 14 (the maximum).
 */
void Font::set_colours(Colour fore, Colour back, int colour_offset /*= 14*/)
{
	_kernel_swi_regs regs;
	regs.r[0] = _font_ref->handle;
	regs.r[1] = (int)(unsigned)back;
	regs.r[2] = (int)(unsigned)fore;
	regs.r[3] = colour_offset;

	// ColourTrans_SetFontColours
	swix_check(_kernel_swi(0x4074F, &regs, &regs));
}

/**
 * Assign a font to the same OS font as another Font object
 *
 * @param other font to copy
 * @returns reference to this object
 */
Font &Font::operator=(const Font &other)
{
	other._font_ref->add_ref(); // Add Ref first in case setting font to itself
	_font_ref->release();
	_font_ref = other._font_ref;

	return *this;
}

/**
 * Check if this object refers to the same OS font as another object.
 *
 * @param other font to compare with
 * @returns true if this font refers to the same font as the other
 */
bool Font::operator==(const Font &other)
{
	return (_font_ref->handle == other._font_ref->handle);
}

/**
 * Check if this object does not refer to the same OS font as another object.
 *
 * @param other font to compare with
 * @returns true if this font does not refer to the same font as the other
 */
bool Font::operator!=(const Font &other)
{
	return (_font_ref->handle != other._font_ref->handle);
}

/**
 *   Get bounding box for a character in millipoints
 *
 *   @param bounds bounding box to update with the character size
 *   @param c character to get the size for.
 */
void Font::get_char_bounds_mp(BBox &bounds, char c)
{
	_kernel_swi_regs regs;

	regs.r[0] = _font_ref->handle;
	regs.r[1] = c;
	regs.r[2] = 0;

	swix_check(_kernel_swi(0x4008E, &regs, &regs));

	bounds.min.x = regs.r[1]; // minimum x of bounding box (inclusive)
	bounds.min.y = regs.r[2]; // minimum y of bounding box (inclusive)
	bounds.max.x = regs.r[3]; // maximum x of bounding box (exclusive)
	bounds.max.y = regs.r[4]; // maximum y of bounding box (exclusive)
}

/**
 *   Get bounding box for a character in OS Units
 *
 *   @param bounds bounding box to update with the character size
 *   @param c character to get the size for.
 */
void Font::get_char_bounds_os(BBox &bounds, char c)
{
	_kernel_swi_regs regs;

	regs.r[0] = _font_ref->handle;
	regs.r[1] = c;
	regs.r[2] = (1 << 4); // return in OS units

	swix_check(_kernel_swi(0x4008E, &regs, &regs));

	bounds.min.x = regs.r[1]; // minimum x of bounding box (inclusive)
	bounds.min.y = regs.r[2]; // minimum y of bounding box (inclusive)
	bounds.max.x = regs.r[3]; // maximum x of bounding box (exclusive)
	bounds.max.y = regs.r[4]; // maximum y of bounding box (exclusive)
}

/**
 * Read the details of the font
 *
 * @param details structure to fill in with the details
 */
void Font::read_details(FontDetails &details) const
{
    _kernel_swi_regs regs;
    regs.r[0] = _font_ref->handle;
    regs.r[1] = 0; // Only interested in basic information
    regs.r[2] = 0;

    swix_check(_kernel_swi(0x40083, &regs, &regs));

    details.x_point_size = regs.r[2];
    details.y_point_size = regs.r[3];
    details.x_resolution = regs.r[4];
    details.y_resolution = regs.r[5];
    details.font_age     = regs.r[6];
    details.usage_count  = regs.r[7];
}

/**
 * Read the full details of the font
 *
 * The full details of the font are the details plus the
 * font identifier
 *
 * @param details structure to fill in with the details
 */
void Font::read_details(FullFontDetails &details) const
{
    _kernel_swi_regs regs;
    regs.r[0] = _font_ref->handle;
    regs.r[1] = 0; // Only interested in basic information
    regs.r[3] = 0x4C4C5546; // 'FULL'

    swix_check(_kernel_swi(0x40083, &regs, &regs));

    int size = regs.r[2];
    char buffer[size+1];
    regs.r[1] = reinterpret_cast<int>(buffer);

    swix_check(_kernel_swi(0x40083, &regs, &regs));

    buffer[size] = 0;
    details.identifier = buffer;
    details.x_point_size = regs.r[2];
    details.y_point_size = regs.r[3];
    details.x_resolution = regs.r[4];
    details.y_resolution = regs.r[5];
    details.font_age     = regs.r[6];
    details.usage_count  = regs.r[7];
}

/**
* Return full font identifier string for this font
*
* The full font identifier includes the name, transformation matrix
* and encoding
*
* @returns font identifier string
*/
std::string Font::identifier() const
{
    _kernel_swi_regs regs;
    regs.r[0] = _font_ref->handle;
    regs.r[1] = 0; // Only interested in basic information
    regs.r[3] = 0x4C4C5546; // 'FULL'

    swix_check(_kernel_swi(0x40083, &regs, &regs));

    int size = regs.r[2];
    char buffer[size+1];
    regs.r[1] = reinterpret_cast<int>(buffer);

    swix_check(_kernel_swi(0x40083, &regs, &regs));

    buffer[size] = 0;
    return std::string(buffer);
}

/**
 *   Get the size of the font
 *
 * @returns size of the font in 16ths of a point
 */
Size Font::size() const
{
	Size sz;
	_kernel_swi_regs regs;

	regs.r[0] = _font_ref->handle;
	regs.r[1] = 0; // Only interested in basic information
	regs.r[2] = 0;

	swix_check(_kernel_swi(0x40083, &regs, &regs));

	sz.width = regs.r[2]; // x point size � 16 (ie in 1/16ths point)
	sz.height = regs.r[3]; // y point size � 16 (ie in 1/16ths point

	return sz;
}


/**
 *    Get the height of the font
 *
 *@returns the height in 1/16th points
 */
int Font::height() const
{
	_kernel_swi_regs regs;

	regs.r[0] = _font_ref->handle;
	regs.r[1] = 0; // Only interested in basic information
	regs.r[2] = 0;

	swix_check(_kernel_swi(0x40083, &regs, &regs));

	return regs.r[3];
}


/**
 *    Get the width of the font
 *
 *@returns the width in 1/16th points
 */
int Font::width() const
{
	_kernel_swi_regs regs;

	regs.r[0] = _font_ref->handle;
	regs.r[1] = 0; // Only interested in basic information
	regs.r[2] = 0;

	swix_check(_kernel_swi(0x40083, &regs, &regs));

	return regs.r[2];
}

/**
 *   Get the size of the font
 *
 *@returns size of the font in OS Units
 */

Size Font::size_os() const
{
	Size sz;
	_kernel_swi_regs regs;

	regs.r[0] = _font_ref->handle;
	regs.r[1] = 0; // Only interested in basic information
	regs.r[2] = 0;

	swix_check(_kernel_swi(0x40083, &regs, &regs));

	sz.width= (regs.r[2] * 5 + 31) / 32; // 1152 16ths of a point = 180 os units
	sz.height = (regs.r[3] * 5 + 31) / 32;

	return sz;
}


/**
 *    Get the height of the font
 *
 *@returns the height in OS units
 */
int Font::height_os() const
{
	_kernel_swi_regs regs;

	regs.r[0] = _font_ref->handle;
	regs.r[1] = 0; // Only interested in basic information
	regs.r[2] = 0;

	swix_check(_kernel_swi(0x40083, &regs, &regs));

	return (regs.r[3] * 5 + 31) / 32;
}


/**
 *    Get the width of the font
 *
 *@returns the width in OS units
 */
int Font::width_os() const
{
	_kernel_swi_regs regs;

	regs.r[0] = _font_ref->handle;
	regs.r[1] = 0; // Only interested in basic information
	regs.r[2] = 0;

	swix_check(_kernel_swi(0x40083, &regs, &regs));

	return (regs.r[2] * 5 + 31) / 32;
}

/**
 * Get the bounding box (in os coordinates) of the font.
 * The bounding box is the minimum sized box that will cover all characters
 * in the font.
 *
 * @return bounds bounding box (min inclusive, max exclusive)
 */
BBox Font::bounding_box() const
{
	BBox result;
	get_bounding_box(result);
	return result;
}

/**
 * Get the bounding box (in os coordinates) of the font.
 * The bounding box is the minimum sized box that will cover all characters
 * in the font.
 *
 * @param bounds updated with the bounding box (min inclusive, max exclusive)
 */
void Font::get_bounding_box(BBox &bounds) const
{
	_kernel_swi_regs regs;

	regs.r[0] = _font_ref->handle;

	swix_check(_kernel_swi(0x40084, &regs, &regs));

	bounds.min.x = regs.r[1];
	bounds.min.y = regs.r[2];
	bounds.max.x = regs.r[3];
	bounds.max.y = regs.r[4];
}



Font::FontRef::FontRef(int handle)
{
	this->handle = handle;

	ref_count = 1;
}

void Font::FontRef::release()
{
	if (--ref_count <= 0) delete this;
}

Font::FontRef::~FontRef()
{
	if (handle)
	{
		_kernel_swi_regs regs;
		regs.r[0] = handle;
		_kernel_swi(0x40082, &regs, &regs);
	}
}

/**
 * Set the foreground and background colour for the WIMP font.
 *
 * Note: If the current WIMP font is the system font this will
 * change the colours for following graphics methods.
 *
 * @param foreground foreground colour for the font
 * @param background background colour the font is going to be painted on.
 */
void WimpFont::set_colours(Colour foreground, Colour background)
{
	swix_check(_swix(Wimp_TextOp, _INR(0,2), 0,
			(unsigned)foreground,
			(unsigned)background));
}

/**
 * Get the width of the text in OS units
 *
 * @param text to measure
 * @param num_chars number of characters to measure or 0 for whole string.
 */
int WimpFont::string_width_os(const std::string &text, int num_chars /*= 0*/)
{
	int width;
	swix_check(_swix(Wimp_TextOp, _INR(0,2)|_OUT(0), 1,
			reinterpret_cast<int>(text.c_str()),
			num_chars, &width));

	return width;
}

/**
 * Paint the wimp font at the given screen coordinates.
 *
 * Before plotting the colour should be set using
 * Graphics::set_foreground/background(WimpColour) or the set_font_colours
 * member of this class.
 *
 * The background needs to be set as the it will be needed if the current
 * desktop font is an antialias font.
 *
 * @param x x coordinate
 * @param y y coordinate
 * @param text text to plot
 * @param flags flags to control painting, defaults to WFPF_NONE
 */
void WimpFont::paint(int x, int y, const std::string &text, int flags /*= WimpFont::WFPF_NONE*/) const
{
	swix_check(_swix(Wimp_TextOp, _INR(0,5), 2 | flags,
			reinterpret_cast<int>(text.c_str()),
			-1,-1,
			x, y));
}

/**
 * Paint the wimp font at the given screen coordinates.
 *
 * Before plotting the colour should be set using
 * Graphics::set_foreground/background(WimpColour) or the set_font_colours
 * member of this class.
 *
 * The background needs to be set as the it will be needed if the current
 * desktop font is an antialias font.
 *
 * @param x x coordinate
 * @param y y coordinate
 * @param text text to plot
 * @param length length of text or -1 to plot to end of null terminated text
 * @param flags flags to control painting, defaults to WFPF_NONE
 */
void WimpFont::paint(int x, int y, const char *text, int length /*= -1*/, int flags /*= WimpFont::WFPF_NONE*/) const
{
	std::string ttext;
	if (length > 0) ttext.assign(text, length);
	else ttext.assign(text);
	paint(x,y,ttext, flags);
}
