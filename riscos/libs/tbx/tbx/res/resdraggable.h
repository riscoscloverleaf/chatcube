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

#ifndef TBX_RES_RESDRAGGABLE_H
#define TBX_RES_RESDRAGGABLE_H

#include "resgadget.h"

namespace tbx {
namespace res {

/**
 * Class for Draggable gadget template
 */
class ResDraggable : public ResGadget
{

public:
	enum {TYPE_ID = 640 };

	/**
	 * Construct an draggable gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an draggable
	 */
	ResDraggable(const ResGadget &other) : ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct an draggable gadget resource
	 *
	 * @param other draggable to copy resource from
	 */
	ResDraggable(const ResDraggable &other) : ResGadget(other)
	{
	}

	virtual ~ResDraggable() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an draggable
	 */
	ResDraggable &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another draggable gadget resource
	 *
	 * @param other draggable gadget resource to copy
	 */
	ResDraggable &operator=(const ResDraggable &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a draggable gadget resource.
	 *
	 * All options are false, events unset and messages are null.
	 */
	ResDraggable()
	  : ResGadget(640,52)
	{
		int_value(0, 0x80); // drop_shadow
		init_message(36,0); // text
		init_string(44,0); // sprite
	}

	/**
	 * Check if draggable should generate an event when the drag has started
	 *
	 * @returns true if the draggable will generate the drag started event
	 */
	bool generate_drag_started() const {return flag(0, 1<<0);}
	/**
	 * Set if the draggable should generate an event when the drag has started
	 *
	 * @param value true if the draggable should generate the drag started event
	 */
	void generate_drag_started(bool value) {flag(0,1<<0,value);}
	/**
	 * Check if the draggable includes a sprite
	 *
	 * @returns true if the draggable includes a sprite
	 */
	bool has_sprite() const {return flag(0, 1<<1);}
	/**
	 * Set if the draggable includes a sprite
	 *
	 * Use the sprite() method to set the sprite name
	 *
	 * @param value set to true if the draggable includes a sprite
	 */
	void has_sprite(bool value) {flag(0,1<<1,value);}
	/**
	 * Check if the draggable includes text
	 *
	 * @returns true if the draggable includes text
	 */
	bool has_text() const {return flag(0, 1<<2);}
	/**
	 * Set if the draggable includes text
	 *
	 * Use the text() method to set the text
	 *
	 * @param value set to true if the draggable includes text
	 */
	void has_text(bool value) {flag(0,1<<2,value);}

	/**
	 * Enumerations for mouse actions allowed on the draggable
	 */
	enum DraggableType
	{
		 DRAG_ONLY, //!< only allows a drag
		 DRAG_CLICK_DOUBLECLICK, //!< click, drag and double click allowed
		 DRAG_SELECT_DOUBLECLICK //!< click selects, double click and drag allowed
	};
	/**
	 * Get the interaction with the mouse for this draggable
	 *
	 * @returns DraggableType enumeration value
	 */
	DraggableType draggable_type() const {return DraggableType(flag_value(0, 56)>>3);}
	/**
	 * Set the interaction with the mouse for this draggable
	 *
	 * @param value DraggableType enumeration value
	 */
	void draggable_type(DraggableType value) {flag_value(0,56,((int)value)<<3);}

	/**
	 * Check if the draggable will deliver Toolbox IDs on drag completion
	 *
	 * @returns true to return Toolbox IDs, false to return WIMP handles.
	 */
	bool deliver_toolbox_ids() const {return flag(0, 1<<6);}
	/**
	 * Set if the draggable will deliver Toolbox IDs on drag completion
	 *
	 * @param value set to true to return Toolbox IDs or false to return WIMP handles.
	 */
	void deliver_toolbox_ids(bool value) {flag(0,1<<6,value);}
	/**
	 * Check if the draggable will show a drop shadow when dragged
	 *
	 * @returns true if a drop shadow will be shown
	 */
	bool drop_shadow() const {return flag(0, 1<<7);}
	/**
	 * Set if the draggable will show a drop shadow when dragged
	 *
	 * @param value true to show a drop shadow
	 */
	void drop_shadow(bool value) {flag(0,1<<7,value);}
	/**
	 * Check if the draggable will be dithered when dragged
	 *
	 * @returns true if a the draggable will be dithered
	 */
	bool not_dithered() const {return flag(0, 1<<8);}
	/**
	 * Set if the draggable will be dithered when dragged
	 *
	 * @param value set to true if dithering is required
	 */
	void not_dithered(bool value) {flag(0,1<<8,value);}

	/**
	 * Get the text for the draggable
	 *
	 * @returns text for draggable or 0 if text not set
	 */
	const char *text() const {return message(36);}
	/**
	 * Set the text for the draggable
	 *
	 * The text will only be used if has_text(true) has been set.
	 *
	 * @param value text for the draggable or 0 if text none
	 * @param max_length the maximum length the text will be changed to after
	 * the gadget is created. The default (-1) will use the length of value.
	 */
	void text(const char *value, int max_length = -1) {message_with_length(36, value, max_length);}
	/**
	 * Set the text for the draggable
	 *
	 * The text will only be used if has_text(true) has been set.
	 *
	 * @param value text for the draggable
	 * @param max_length the maximum length the text will be changed to after
	 * the gadget is created. The default (-1) will use the length of value.
	 */
	void text(const std::string &value, int max_length = -1) {message_with_length(36, value, max_length);}
	/**
	 * Get the maximum length for the text
	 *
	 * @returns maximum buffer size for the text.
	 */
	int max_text_len() const {return int_value(40);}
	/**
	 * Get the sprite name for the draggable
	 *
	 * @returns sprite name for draggable or 0 if sprite name not set
	 */
	const char *sprite() const {return string(44);}
	/**
	 * Set the sprite name for the draggable
	 *
	 * The sprite will only be used if has_sprite(true) has been set.
	 *
	 * @param value sprite name for the draggable or 0 if none
	 * @param max_length the maximum length the spritename will be changed to after
	 * the gadget is created. The default (-1) will use the length of value.
	 */
	void sprite(const char *value, int max_length = -1) {string_with_length(44, value, max_length);}
	/**
	 * Set the sprite name for the draggable
	 *
	 * The sprite will only be used if has_sprite(true) has been set.
	 *
	 * @param value sprite name for the draggable
	 * @param max_length the maximum length the spritename will be changed to after
	 * the gadget is created. The default (-1) will use the length of value.
	 */
	void sprite(const std::string &value, int max_length = -1) {string_with_length(44, value, max_length);}
	/**
	 * Get the maximum length of the sprite name for the draggable
	 */
	int max_sprite_len() const {return int_value(48);}
};

}
}

#endif // TBX_RES_RESDRAGGABLE_H

