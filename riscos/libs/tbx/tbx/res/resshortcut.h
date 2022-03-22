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

#ifndef TBX_RES_SHORTCUT_H
#define TBX_RES_SHORTCUT_H

#include "resbase.h"
#include <cstring>

namespace tbx
{
class Window;
namespace res
{

class ResWindow;
const int SHORTCUT_SIZE = 16;

/**
 * Keyboard short cut
 */
class ResShortcut : public ResBase
{
	friend class ResWindow;

	ResShortcut(void *item_header, ResData *data)
		: ResBase(new ResImpl(item_header, 0, SHORTCUT_SIZE, data))
	{
	}

	// Allow Window to access shortcut
	friend class tbx::Window;
	void *header() const {return _impl->header();}

public:
	/**
	 * Construct shortcut as a copy of another
	 *
	 * @param other shortcut to copy
	 */
	ResShortcut(const ResShortcut &other) : ResBase(other)
	{
	}

	virtual ~ResShortcut() {}

	/**
	 * Assign this shortcut to be a copy of another
	 *
	 * @param other shortcut to copy
	 */
	ResShortcut &operator=(const ResShortcut &other)
	{
		ResBase::operator=(other);
		return *this;
	}
	
	/**
	 * Construct a new shortcut for the given key code
	 *
	 * @param code WIMP key code for the short cut
	 */
	ResShortcut(int code)
		: ResBase(new ResImpl(new char[SHORTCUT_SIZE], 0, SHORTCUT_SIZE, new ResData()))
	{
		memset(_impl->body(), 0, SHORTCUT_SIZE);
		key_code(code);
		init_string(12, 0);  // show object
	}

protected:
	/**
	 * Make item writeable.
	 */
	void make_writeable()
	{
		if (!_impl->unique())
		{
			ResImpl *new_impl = new ResImpl(*_impl);
			_impl->release();
			_impl = new_impl;
		}
	}

public:
  	  /**
	   * Get all flags as a word raw access.
	   *
	   * Not normally used as there are named methods that allow access
	   * to the individual items in the flags
	   * 
	   */
	  unsigned int flags() const {return uint_value(0);}

	  /**
	   * Set all flags as a word
	   *
	   * Not normally used as there are named methods that allow access
	   * to the individual items in the flags
	   */
	   void flags(unsigned int value) {uint_value(0, value);}

	   /**
	    * Check if show object will be shown transiently.
	    *
	    * @returns true if show object will be shown transiently
		*/
	   bool transient() const {return flag(0,1);}
	   /**
	    * Set if show object will be shown transiently.
	    *
	    * transient means the show object will automatically be closed
	    * if a click occurs outside it.
	    *
	    * @param b set to true if show object should be shown transiently
		*/
	   void transient(bool b) {flag(0,1, b);}

	   /**
	    * Get the WIMP key code for shortcut
	    *
	    * @returns WIMP key code
		*/
	   int key_code() const {return int_value(4);}
	   /**
	    * Set the WIMP key code for shortcut
	    *
	    * @param code WIMP key code
		*/
	   void key_code(int code) {int_value(4, code);}
	   /**
	    * Get the event ID to be generated for key
	    *
	    * @returns event ID or 0 if none
		*/
	   int event_id() const {return int_value(8);}
	   /**
	    * Set the event ID to be generated for key
	    *
	    * @param id event ID or 0 if none
		*/
	   void event_id(int id) {int_value(8, id);}
	   /**
	    * Get the Toolbox object to show
	    *
	    * @returns pointer to zero terminated object name or 0 if none
		*/
	   const char *show() const {return string(12);}
	   /**
	    * Set the Toolbox object to show
	    *
	    * @param obj pointer to zero terminated object name or 0 if none
		*/
	   void show(const char *obj) {string(12, obj);}
	   /**
	    * Set the Toolbox object to show
	    *
	    * @param obj object name
		*/
	   void show(const std::string &obj) {string(12, obj);}
};

}
}

#endif
