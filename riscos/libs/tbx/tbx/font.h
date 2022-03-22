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

#ifndef TBX_FONT_H
#define TBX_FONT_H

#include <string>
#include "colour.h"
#include "bbox.h"
#include "size.h"

namespace tbx
{
  /**
  * Structure used to return details of a font
  */
  struct FontDetails
  {
    int x_point_size; ///< x point size in 16th of a point
    int y_point_size; ///< y point size in 16th of a point
    int x_resolution; ///< x resolution in dots per inch
    int y_resolution; ///< y resolution in dots per inch
    int font_age;     ///< age of font
    int usage_count;  ///< usage count of font
  };

  /**
  * Structure used to return full details of the font which also
  * includes the font identifier
  */
  struct FullFontDetails : public FontDetails
  {
     std::string identifier; ///< Full font identifier string
  };

    /**
     * Class to handle painting and measuring text using an outline font
     */
	class Font
	{
	public:
		Font();
		Font(const std::string &font_name, int width, int height = -1);
		Font(const Font &other);
		virtual ~Font();

		/**
		 * Get RISC OS font handle
		 *
		 * @returns RISC OS font handle for the font
		 */
		int handle() const {return _font_ref->handle;}
		/**
		 * Check if this object is assigned to an outline font on the system
		 *
		 * @returns true if this is valid
		 */
		bool is_valid() const {return (_font_ref->handle != 0);}
		void set_colours(Colour fore, Colour back, int colourOffset = 14);
		void read_details(FontDetails &details) const;
		void read_details(FullFontDetails &details) const;
		std::string identifier() const;
		Size size() const;
		int height() const;
		int width() const;
		Size size_os() const;
		int height_os() const;
		int width_os() const;

		// Operations
		bool find(const std::string &font_name, int width, int height = -1);
		bool desktop_font();

		/**
		 * Flags to control font painting operation
		 */
		enum PaintFlags {FPF_NONE = 0, FPF_JUSTIFY=1, FPF_RUBOUT= 2, FPF_OSUNITS=16};
		void paint(int x, int y, const std::string &text, int flags = Font::FPF_NONE) const;
		void paint(int x, int y, const char *text, int length = -1, int flags = Font::FPF_NONE) const;

		int string_width_mp(const std::string &text) const;
		int string_width_mp(const char *text, int length = -1) const;
		int string_width_os(const std::string &text) const;
		int string_width_os(const char *text, int length = -1) const;
		int string_height_os(const std::string &text, int max_width = -1) const;
		int string_height_os(const char *text, int length = -1, int max_width  = -1) const;
		tbx::Size string_size_os(const std::string &text, int max_width = -1) const;
		tbx::Size string_size_os(const char *text, int length = -1, int max_width  = -1) const;
		tbx::BBox string_bounds_os(const std::string &text, int max_width = -1) const;
		tbx::BBox string_bounds_os(const char *text, int length = -1, int max_width  = -1) const;

		void get_char_bounds_mp(BBox &bounds, char c);
		void get_char_bounds_os(BBox &bounds, char c);

		int find_split_os(const char *text, int length, int width, int split_char =-1);
		int find_index_xy_os(const char *text, int length, int x, int y);

		BBox bounding_box() const;
		void get_bounding_box(BBox &bounds) const;

		// Operators
		Font &operator=(const Font &other);
		bool operator==(const Font &other);
		bool operator!=(const Font &other);

	private:
		class FontRef
		{
			int ref_count;
		public:
			FontRef(int handle);

			void add_ref() {ref_count++;}
			void release();

			int handle;
		protected:
			~FontRef();
		} *_font_ref;

		static FontRef *s_invalid_font_ref;
		static FontRef *s_desktop_font_ref;
	};

	// Conversion functions for points
    // Notes: assumes 180 os units per inch - also rounds down
	//        1 points is 1/72 of an inch

	/**
	 * Convert from OS units to points
	 *
     * Note:  assumes 180 OS units per inch - also rounds down
	 *        1 points is 1/72 of an inch
	 *
	 * @param os OS units to convert
	 * @returns number of points equivalent
	 */
    inline int os_to_points(int os)              {return (os<<1) / 5;}
	/**
	 * Convert from points to OS units
	 *
     * Note:  assumes 180 OS units per inch - also rounds down
	 *        1 points is 1/72 of an inch
	 *
	 * @param points number of points to convert
	 * @returns number of OS units equivalent
	 */
    inline int points_to_os(int points)         {return (5 * points) >> 1;}

	/**
	 * Convert from OS units to 16th of a point
	 *
     * Note:  assumes 180 os units per inch - also rounds down
	 *        1 points is 1/72 of an inch
	 *
	 * @param os OS units to convert
	 * @returns number of 16ths of a point equivalent
	 */
    inline int os_to_points_16th(int os)          {return (os<<5) / 5;}
	/**
	 * Convert from 16ths of a point to OS units
	 *
     * Note:  assumes 180 OS units per inch - also rounds down
	 *        1 points is 1/72 of an inch
	 *
	 * @param point16th number of 16ths of a points to convert
	 * @returns number of OS units equivalent
	 */
    inline int points_16th_to_os(int point16th)   {return (5 * point16th) >> 5;}

	/**
	 * Convert from OS units to millipoints
	 *
     * Note:  assumes 180 os units per inch - also rounds down
	 *        1 points is 1/72 of an inch
	 *
	 * @param os OS units to convert
	 * @returns number of millipoints equivalent
	 */
    inline int os_to_millipoints(int os)         {return 400 * os;}
	/**
	 * Convert from millipoints to OS units
	 *
     * Note:  assumes 180 OS units per inch - also rounds down
	 *        1 points is 1/72 of an inch
	 *
	 * @param millipoint number of millipoints to convert
	 * @returns number of OS units equivalent
	 */
    inline int millipoints_to_os(int millipoint) {return millipoint / 400;}

    /**
     * Class to display and measure text using the current
     * desktop font.
     */
    class WimpFont
    {
    public:
    	void set_colours(Colour foreground, Colour background);

    	/**
    	 * Return height of the font
    	 *
    	 * @returns height of font in OS units
    	 */
    	int height_os() const {return 32;}
    	int string_width_os(const std::string &text, int num_chars = 0);

    	/**
    	 * Flags used for painting the WimpFont
    	 */
    	enum PaintFlags {WFPF_NONE, WFPF_VCENTRE = (1<<30), WFPF_RJUSTIFY = (1<<31)};
    	void paint(int x, int y, const std::string &text, int flags = WimpFont::WFPF_NONE) const;
    	void paint(int x, int y, const char *text, int length = -1, int flags = WimpFont::WFPF_NONE) const;
    };

    /**
     * fpcs namespace is used to provide control strings for font painting
     */
    namespace fpcs
    {
    	/**
    	 * Move position horizontally
    	 *
    	 * @param move distance to move in millipoints
    	 * @returns string to use for font paint
    	 */
    	inline std::string move_x(int move)
    	{
    		std::string t("\x09xxx");
    		t[1] = move & 0xFF;
    		t[2] = (move & 0xFF00) >> 8;
    		t[3] = (move & 0xFF0000) >> 16;
    		return t;
    	}

    	/**
    	 * Move position horizontally in OS units
    	 * @param move distance to move in OS units
    	 * @returns string to use for font paint
    	 */
    	inline std::string move_x_os(int move) {return move_x(tbx::os_to_millipoints(move));}

    	/**
    	 * Move position vertically
    	 *
    	 * @param move distance to move in millipoints
    	 * @returns string to use for font paint
    	 */
    	inline std::string move_y(int move)
    	{
    		std::string t("\x0bxxx");
    		t[1] = move & 0xFF;
    		t[2] = (move & 0xFF00) >> 8;
    		t[3] = (move & 0xFF0000) >> 16;
    		return t;
    	}

    	/**
    	 * Move position vertically in OS units
    	 * @param move distance to move in OS units
    	 * @returns string to use for font paint
    	 */
    	inline std::string move_y_os(int move) {return move_y(tbx::os_to_millipoints(move));}

    	/**
    	 * Set foreground and background font colours
    	 *
    	 * @param fore foreground colour
    	 * @param back background colour
    	 * @param offset number of colours to use for antialiasing
    	 * @returns string with sequence to change colours
    	 */
    	inline std::string colour(tbx::Colour fore, tbx::Colour back, int offset = 14)
    	{
    		std::string t("\x13rgbRGBo");
    		t[1] = char(back.red());
    		t[2] = char(back.green());
    		t[3] = char(back.blue());
    		t[4] = char(fore.red());
    		t[5] = char(fore.green());
    		t[6] = char(fore.blue());
    		t[7] = char(offset);
    		return t;
    	}

    	/**
    	 * Turn on or off underlining
    	 * @param base distance from base line for underline in 1/256th of current font size
    	 * @param thickness thickness of underline in 1/256th of current font size or 0 to turn it off
    	 * @returns string to set underline
    	 */
    	inline std::string underline(int base, int thickness)
    	{
    		std::string t("0x19bt");
    		t[1] = char(base);
    		t[2] = char(thickness);
    		return t;
    	}

    	/**
    	 * Change font in the string
    	 *
    	 * @param fnt - new font to use
    	 * @returns string to change to new font handle
    	 */
    	inline std::string font(const tbx::Font &fnt)
    	{
    		std::string t("0x1af");
    		t[1] = char(fnt.handle());
    		return t;
    	}
    }
};

#endif
