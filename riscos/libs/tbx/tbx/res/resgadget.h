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

#ifndef TBX_RES_RESGADGET_H
#define TBX_RES_RESGADGET_H

#include "resbase.h"
#include "../handles.h"

namespace tbx {

class Window;

namespace res {

class ResWindow;

/**
 * Base class for gadget memory resources
 */
class ResGadget : public ResBase
{
	friend class ResWindow;

	ResGadget(void *item_header, int size, ResData *data);

	// Access to allow window object to add a gadget from
	// this template
	friend class tbx::Window;
	void *header() const {return _impl->header();}

public:
	/**
	 * Construct a resource gadget as a copy of another
	 *
	 * @param other gadget to copy
	 */
	ResGadget(const ResGadget &other) : ResBase(other)
	{
	}

	virtual ~ResGadget() {}

	/**
	 * Assign a resource gadget as a copy of another
	 *
	 * @param other gadget to copy
	 */
	ResGadget &operator=(const ResGadget &other)
	{
		ResBase::operator=(other);
		return *this;
	}
	
	ResGadget(int type, int size);

	void check_type(int type) const;

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
	   *
	   * @param value unsigned 32 bit values with the flags
	   */
	  void flags(unsigned int value) {uint_value(0, value);}

	  /**
	   * Check if gadget is at the back (created first)
	   *
	   * @returns true if gadget is at the back
	   */
	  bool back() const {return flag(0, (1<<30));}
	  /**
	   * Set if gadget should be at the back
	   *
	   * @param value set to true if the gadget should be at the back
	   */
	  void back(bool value) {flag(0, (1<<30), value);}
	  
	  /**
	   * Check if the gadget is faded
	   *
	   * @returns true if the gadget is faded
	   */
	  bool faded() const {return flag(0, (1<<31));}
	  /**
	   * Set if the gadget is faded
	   *
	   * @param value set to true if the gadget should be faded
	   */
	  void faded(bool value) {flag(0, (1<<31), value);}

	  /**
	   * Get the gadget type
	   *
	   * @returns Toolbox gadget type ID for this gadget
	   */
	  unsigned short type() const {return ushort_value(4);}

	  /**
	   * Get the size stored in template.
	   *
	   * Standard gadgets have a known size so this method may return
	   * 0 for standard gadgets.
	   *
	   * @returns the size stored in the gadget template.
	   */
	  unsigned short stored_size() const {return ushort_value(6);}

	  /**
	   * Size of gadget (including header). Takes into account size of
	   * standard controls.
	   *
	   * @returns size of gadget
	   */
	  int size() const {return gadget_size(uint_value(4));}

	  /**
	   * Get gadget size given its type and size value.
	   *
	   * @returns gadget size
	   */
	  static int gadget_size(unsigned int type_and_size);

	  /**
	   * Get the minimum x coordinate of the gadget's bounding box
	   *
	   * @returns minimum x coordinate of bounding box (in window work area coordinates).
	   */
	  int xmin() const {return int_value(8);}
	  /**
	   * Set the minimum x coordinate of the gadget's bounding box
	   *
	   * @param x minimum x coordinate of bounding box (in window work area coordinates).
	   */
	  void xmin(int x) {int_value(8, x);}
	  /**
	   * Get the minimum y coordinate of the gadget's bounding box
	   *
	   * @returns minimum y coordinate of bounding box (in window work area coordinates).
	   */
	  int ymin() const {return int_value(12);}
	  /**
	   * Set the minimum y coordinate of the gadget's bounding box
	   *
	   * @param y minimum y coordinate of bounding box (in window work area coordinates).
	   */
	  void ymin(int y) {int_value(12, y);}
	  /**
	   * Get the maximum x coordinate of the gadget's bounding box
	   *
	   * @returns maximum x coordinate of bounding box (in window work area coordinates).
	   */
	  int xmax() const {return int_value(16);}
	  /**
	   * Set the maximum x coordinate of the gadget's bounding box
	   *
	   * @param x maximum x coordinate of bounding box (in window work area coordinates).
	   */
	  void xmax(int x) {int_value(16, x);}
	  /**
	   * Get the maximum y coordinate of the gadget's bounding box
	   *
	   * @returns maximum y coordinate of bounding box (in window work area coordinates).
	   */
	  int ymax() const {return int_value(20);}
	  /**
	   * Set the maximum y coordinate of the gadget's bounding box
	   *
	   * @param y maximum y coordinate of bounding box (in window work area coordinates).
	   */
	  void ymax(int y) {int_value(20, y);}

	  /**
	   * Get the Component ID
	   *
	   * @returns Component ID for the gadget
	   */
	  ComponentId component_id() const {return ComponentId(int_value(24));}
	  /**
	   * Set the Component ID
	   *
	   * @param value Component ID for the gadget
	   */
	  void component_id(ComponentId value) {int_value(24, (int)value);}
	 
	  /**
	   * Get the help message for the gadget
	   *
	   * @returns pointer to zero terminated help message or 0 if none
	   */
	  const char *help_message() const {return message(28);}
	  /**
	   * Set the help message for the gadget
	   *
	   * @param msg pointer to zero terminated help message or 0 if none
	   * @param max_length maximum length the help message will be changed to
	   * or -1 (the default) to use the length of msg.
	   */
	  void help_message(const char *msg, int max_length = -1) {message_with_length(28, msg, max_length);}
	  /**
	   * Set the help message for the gadget
	   *
	   * @param msg help message
	   * @param max_length maximum length the help message will be changed to
	   * or -1 (the default) to use the length of msg.
	   */
	  void help_message(const std::string &msg, int max_length = -1) {message_with_length(28, msg, max_length);}

	  /**
	   * Max size of message
	   */
	  int max_help() const {return int_value(32);}
};

}
}

#endif // TBX_RES_RESGADGET_H
