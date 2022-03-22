/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2013 Alan Buckley   All Rights Reserved.
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
 * scrolllist.cc
 *
 *  Created on: 25-Mar-2009
 *      Author: alanb
 */

#include "scrolllist.h"
#include "swixcheck.h"
#include "kernel.h"
#include <memory>

namespace tbx {

/**
 * Add a text item to the scroll list
 *
 * @param text Text of item to add
 * @param index location to insert or -1 (the default) for the end of the list
 */
void ScrollList::add_item(const std::string &text, int index /*= -1*/)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x401c;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = reinterpret_cast<int>(const_cast<char *>(text.c_str() ));
	regs.r[5] = 0;
	regs.r[6] = 0;
	regs.r[7] = index;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Add an item with a sprite from the WIMP sprite pool
 *
 * @param text Text of item to add
 * @param sprite WimpSprite to display with the item
 * @param index location to insert or -1 (the default) for the end of the list
 */
void ScrollList::add_item(const std::string &text, const WimpSprite &sprite, int index /*= -1*/)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x401c;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = reinterpret_cast<int>(const_cast<char *>(text.c_str() ));
	regs.r[5] = 0;
	regs.r[6] = reinterpret_cast<int>(const_cast<char *>(sprite.name().c_str() ));
	regs.r[7] = index;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Add an item with a use sprite
 *
 * @param text Text of item to add
 * @param sprite UserSprite to display with the item
 * @param index location to insert or -1 (the default) for the end of the list
 */
void ScrollList::add_item(const std::string &text, const UserSprite &sprite, int index /*= -1*/)
{
	_kernel_swi_regs regs;
	regs.r[0] = 1;
	regs.r[2] = 0x401c;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = reinterpret_cast<int>(const_cast<char *>(text.c_str() ));
	regs.r[5] = reinterpret_cast<int>(sprite.get_sprite_area()->pointer());
//	regs.r[6] = reinterpret_cast<int>(sprite.pointer());
    std::string name = sprite.name();
	regs.r[6] = reinterpret_cast<int>(const_cast<char *>(name.c_str()));
	regs.r[7] = index;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}


/**
 * Delete an item
 *
 * @param index index of item to remove from the scroll list
 */
void ScrollList::delete_item(int index)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x401d;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = index;
	regs.r[5] = index;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Delete a range of items
 *
 * @param start first item to delete
 * @param end last item to delete
 */
void ScrollList::delete_items(int start, int end)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x401d;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = start;
	regs.r[5] = end;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Delete all the items
 */
void ScrollList::clear()
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x401d;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = 0;
	regs.r[5] = -1;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Select an item, clearing any other selection
 *
 * @param index new item that will make up the selection
 */
void ScrollList::select_unique_item(int index)
{
	_kernel_swi_regs regs;
	regs.r[0] = 1;
	regs.r[2] = 0x401E;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = index;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Add the specified item to the selection
 *
 * @param index item to add to the selection
 */
void ScrollList::select_item(int index)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x401E;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = index;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Remove an item from the selection
 *
 * @param index index of item to remove from the selection
 */
void ScrollList::deselect_item(int index)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x401F;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = 0;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Return the first selected item
 *
 * @return first selected item id or -1 if none
 */
int ScrollList::first_selected() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x4020;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = -1;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return regs.r[0];
}

/**
 * Return the next selected item id
 *
 * @param from location to search for the selection from
 * @returns next selected item id or -1
 */
int ScrollList::next_selected(int from) const
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x4020;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = from;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return regs.r[0];
}

/**
 * Ensure an item is visible in the scrolllist
 *
 * @param index index of item to make visible
 */
void ScrollList::make_visible(int index)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x4021;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = index;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Update the text of an item
 *
 * @param index index of item to update
 * @param text new text
 */
void ScrollList::item_text(int index, const std::string &text)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x4027;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = reinterpret_cast<int>(const_cast<char *>(text.c_str() ));
	regs.r[5] = index;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get the text for an item
 *
 * @param index index of item to retrieve text from
 * @return text for the item
 */
std::string ScrollList::item_text(int index) const
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x4025;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = 0;
	regs.r[5] = 0;
	regs.r[6] = index;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    std::string value;
    int len = regs.r[5];
    if (len)
    {
       std::auto_ptr<char> m_buffer(new char[len]);
       regs.r[4] = reinterpret_cast<int>(m_buffer.get());
       regs.r[5] = len;

       // Run Toolbox_ObjectMiscOp to get the buffer
       swix_check(_kernel_swi(0x44ec6, &regs, &regs));

       value = m_buffer.get();
    }

    return value;

}

/**
 * Get the number of items in the scroll list
 *
 * @returns number of items in the scrolllist
 */
int ScrollList::count_items() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x4026;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return regs.r[0];
}
/**
 * Set the font used for the scroll list items
 *
 * @param name The name of the font
 * @param width font width in 16ths of a point
 * @param height font height in 16ths of a point
 */
void ScrollList::font(const std::string &name, int width, int height)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x4024;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = reinterpret_cast<int>(const_cast<char *>(name.c_str() ));
	regs.r[5] = width;
	regs.r[6] = height;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Set the scroll list to use the system font to display the text
 *
 * @param width font width in 16ths of a point
 * @param height font height in 16ths of a point
 */
void ScrollList::system_font(int width, int height)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x4024;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = 0;
	regs.r[5] = width;
	regs.r[6] = height;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Set the colours of the scroll list using WIMP colours
 *
 * @param foreground foreground/text colour
 * @param background background colour
 */
void ScrollList::set_colour(WimpColour foreground, WimpColour background)
{
	_kernel_swi_regs regs;
	regs.r[0] = 1;
	regs.r[2] = 0x401F;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = foreground;
	regs.r[4] = background;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Set the colours of the scroll list using RGB colours
 *
 * @param foreground foreground/text colour
 * @param background background colour
 */
void ScrollList::set_colour(Colour foreground, Colour background)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x401F;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	regs.r[4] = foreground;
	regs.r[4] = background;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get the colours used in the scrolllist
 *
 * @param foreground updated to foreground/text colour
 * @param background updated to background/text colour
 */
void ScrollList::get_colour(Colour &foreground, Colour &background) const
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 0x401F;
	regs.r[1] = _handle;
	regs.r[3] = _id;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
	foreground = regs.r[0];
	background = regs.r[1];
}

static void scrolllist_selection_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
 	 ScrollListSelectionEvent event(id_block, data);
 	 static_cast<ScrollListSelectionListener *>(listener)->scrolllist_selection(event);
}


/**
 * Add listener for when the selected items changes
 *
 * The default Selection listener event must have been enabled in the resources
 * for this scroll list for the event listener to be called.
 *
 * @param listener listener to add
 */
void ScrollList::add_selection_listener(ScrollListSelectionListener *listener)
{
	add_listener(0x140181, listener, scrolllist_selection_router);
}

/**
 * Remove listener for when the selected items changes
 *
 * @param listener listener to remove
 */
void ScrollList::remove_selection_listener(ScrollListSelectionListener *listener)
{
	remove_listener(0x140181, listener);
}

}
