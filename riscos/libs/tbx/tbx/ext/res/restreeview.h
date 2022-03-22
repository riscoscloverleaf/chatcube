/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2020 Alan Buckley   All Rights Reserved.
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

#ifndef TBX_RES_RESTREEVIEW_H
#define TBX_RES_RESTREEVIEW_H

#include "tbx/res/resgadget.h"

namespace tbx {
namespace ext {
namespace res {

/**
 * Class for Tabs gadget template
 */
class ResTreeView : public tbx::res::ResGadget
{

public:
	enum {TYPE_ID = 0x402c };

	/**
	 * Construct a tree view gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not a tree view
	 */
	ResTreeView(const tbx::res::ResGadget &other) : tbx::res::ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct a tree view gadget resource
	 *
	 * @param other tree view to copy resource from
	 */
	ResTreeView(const ResTreeView &other) : tbx::res::ResGadget(other)
	{
	}

	virtual ~ResTreeView() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not a tree view
	 */
	ResTreeView &operator=(const tbx::res::ResGadget &other)
	{
		other.check_type(TYPE_ID);
		tbx::res::ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another tree view gadget resource
	 *
	 * @param other tree view gadget resource to copy
	 */
	ResTreeView &operator=(const ResTreeView &other)
	{
		tbx::res::ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a tree view gadget resource.
	 *
	 * All options are false, default wimp colours set and messages are null.
	 */
	ResTreeView()
	  : tbx::res::ResGadget(0x4028,52)
	{
		foreground(7); // Black
		background(1); // grey
		linecolour(3);
		version(1);
		vertical_spacing(12);
		horizontal_spacing(256);
		flags(0); // Turn off all flags
		// Note: offset 44 is in the header, but unused
	}

	bool allow_vscroll() const {return flag(0, 1);}
	void allow_vscroll(bool value) {flag(0, 1, value);}
	bool allow_hscroll() const {return flag(0, 2);}
	void allow_hscroll(bool value) {flag(0, 2, value);}
	bool auto_update() const {return flag(0, 4);}
	void auto_update(bool value) {flag(0, 4, value);}
	bool show_current_node() const {return flag(0, 8);}
	void show_current_node(bool value) {flag(0, 8, value);}
	bool plot_lines() const {return flag(0, 16);}
	void plot_lines(bool value) {flag(0, 16, value);}
	bool allow_selection() const {return flag(0, 32);}
	void allow_selection(bool value) {flag(0, 32,value);}
	bool notify_selection() const {return flag(0, 64);}
	void notify_selection(bool value) {flag(0, 64, value);}
	bool select_children() const {return flag(0, 1u<<7);}
	void select_children(bool value) {flag(0, 1u<<7, value);}
	bool allow_expand() const {return flag(0, 1u<<8);}
	void allow_expand(bool value) {flag(0, 1u<<8, value);}
	bool notify_expansion() const {return flag(0, 1u<<9);}
	void notify_expansion(bool value) {flag(0, 1u<<9, value);}
	bool expand_by_default() const {return flag(0, 1u<<10);}
	void expand_by_default(bool value) {flag(0, 1u<<10, value);}
	bool allow_rename() const {return flag(0, 1u<<11);}
	void allow_rename(bool value) {flag(0, 1u<<11, value);}
	bool notify_rename() const {return flag(0, 1u<<12);}
	void notify_rename(bool value) {flag(0, 1u<<12, value);}
	bool allow_drags() const {return flag(0, 1u<<13);}
	void allow_drags(bool value) {flag(0, 1u<<13, value);}
	bool notify_drag_end() const {return flag(0, 1u<<14);}
	void notify_drag_end(bool value) {flag(0, 1u<<14, value);}
	bool all_events() const {return flag(0, 1u<<15);}
	void all_events(bool value) {flag(0, 1u<<15,value);}
	bool double_click_expands() const {return flag(0, 1u<<16);}
	void double_click_expands(bool value) {flag(0, 1u<<16,value);}
	bool flat_mode() const {return flag(0, 1u<<17);}
	void flat_mode(bool value) {flag(0, 1u<<17,value);}
	bool text_below_sprite() const {return flag(0, 1u<<18);}
	void text_below_sprite(bool value) {flag(0, 1u<<18,value);}
	bool sort_display() const {return flag(0, 1u<<19);}
	void sort_display(bool value) {flag(0, 1u<<19,value);}
	bool sort_reversed() const {return flag(0, 1u<<20);}
	void sort_reversed(bool value) {flag(0, 1u<<20,value);}
	bool sort_by_sprite() const {return flag(0, 1u<<21);}
	void sort_by_sprite(bool value) {flag(0, 1u<<21,value);}
	/**
	 * Get foreground/text colour for the Tabs
	 *
	 * @returns foreground/text wimp colour
	 */
	int foreground() const {return flag_value(36,0xF);}
	/**
	 * Set foreground/text colour for the Tabs
	 *
	 * @param value foreground/text wimp colour
	 */
	void foreground(int value) {flag_value(36,0xF,value);}
	/**
	 * Get background colour for the Tabs
	 *
	 * @returns background
	 */
	int background() const {return flag_value(36,0xF0)>>4;}
	/**
	 * Set background colour for the Tabs
	 *
	 * @param value background wimp colour
	 */
	void background(int value) {flag_value(36,0xF0, value<<4);}
	/**
	 * Get linecolour colour for the Tabs
	 *
	 * @returns linecolour
	 */
	int linecolour() const {return flag_value(36,0xF00)>>8;}
	/**
	 * Set linecolour colour for the Tabs
	 *
	 * @param value linecolour wimp colour
	 */
	void linecolour(int value) {flag_value(36,0xF00, value<<8);}
	/**
	 * Get tree view version
	 *
	 * @returns tree view version number
	 */
	int version() const {return byte_value(39);}
	/**
	 * Set tree view version
	 *
	 * @param value version of tree view
	 */
	void version(int value) {byte_value(39, value);}
	
	unsigned int vertical_spacing() const {return uint_value(40);}
	void vertical_spacing(unsigned int value) {return uint_value(40,value);}
	unsigned int horizontal_spacing() const {return uint_value(44);}
	void horizontal_spacing(unsigned int value) {return uint_value(44,value);}
};

}
}
}

#endif // TBX_RES_RESTREEVIEW_H

