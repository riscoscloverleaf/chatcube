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

#include "reswindow.h"
#include "resexcept.h"

namespace tbx
{
namespace res
{

/**
 * Get gadget size including for built in types is size is not specified
 */
int ResGadget::gadget_size(unsigned int type_and_size)
{
	int size = type_and_size >> 16;
	if (size) return size;
	switch(type_and_size & 0xFFFF)
	{
	case 128: size = 36+16; break; // Action Button  
	case 192: size = 36+20; break; // Option Button  
	case 256: size = 36+4; break; // Labelled Box  
	case 320: size = 36+4; break; // Label    
	case 384: size = 36+20; break; // Radio Button  
	case 448: size = 36+8; break; // Display Field  
	case 512: size = 36+24; break; // Writable Field  
	case 576: size = 36+20; break; // Slider    
	case 640: size = 36+20; break; // Draggable    
	case 704: size = 36+4; break; // PopUp Menu  
	case 768: size = 36+0; break; // Adjuster Arrow  
	case 832: size = 36+32; break; // Number Range  
	case 896: size = 36+28; break; // String Set  
	case 960: size = 36+20; break; // Button
	default:
		throw std::invalid_argument("gadget size request for non-standard gadget");
		break;
	}

    return size;
}


/**
 * Protected constructor used by ResWindow
 */
ResGadget::ResGadget(void *item_header, int size, ResData *data)
: ResBase(new ResImpl(item_header, 0, size, data))
{
}

/**
 * Construct an empty gadget of the given type and size
 *
 * The gadget is constructed with all fields 0 except
 * the type and size field and the help message field
 * registered as a message string.
 *
 * @param type gadget type
 * @param size gadget size. This must always be specified it will not be deduced from the type.
 */
ResGadget::ResGadget(int type, int size)
: ResBase(new ResImpl(new char[size], 0, size, new ResData()))
{
	memset(_impl->header(), 0, size);
	int_value(4, type + (size << 16));
	init_message(28, 0); // Help message
}

/**
 * Check if this gadgets type is as specified.
 *
 * @param type_id the gadget type id to check against
 * @throws ResGadgetTypeMismatch if the types do not match
 */
void ResGadget::check_type(int type_id) const
{
	if (type() != type_id) throw ResGadgetTypeMismatch(type(), type_id);
}


/**
 * Get a copy of the gadget at the given offset.
 *
 * This is for specialised use only - use gadget iterators for
 * normal access to the gadgets in this window
 */
ResGadget ResWindow::gadget_at_offset(int item_offset) const
{
	int *gadget_header = (int *)(object_header()->body + item_offset);
	int size = ResGadget::gadget_size(gadget_header[1]);

	char *new_gadget = new char[size];
	memcpy(new_gadget, gadget_header, size);

	ResData *item_data;

	if (_impl->read_only())
	{
		item_data = ResData::copy_component_from_read_only(new_gadget, _impl->header(), item_offset, size);
	} else
	{
		item_data = component_data(new_gadget, item_offset, size);
	}
	ResGadget gadget(new_gadget, size, item_data);

    return gadget;
}

/**
 * Get offset to first gadget
 *
 * @returns offset to first gadget or 0 if no gadgets
 */
int ResWindow::first_gadget_offset() const
{
	return(gadget_start()) ? gadget_start() - object_header()->body : 0;
}

/**
 * Get offset to end of gadgets
 *
 * @returns offset to end of gadgets or 0 if no gadgets
 */
int ResWindow::end_gadget_offset() const
{
	return (gadget_start()) ? object_header()->body_size : 0;
}

/**
 * Get offset to next gadget in window
 *
 * @param item_offset current offset. Updated to next offset.
 */
void ResWindow::next_gadget(int &item_offset) const
{
	item_offset += ResGadget::gadget_size(*((int *)(object_header()->body + item_offset + 4)));
}

/**
 * Find gadget with given id
 *
 * @param component_id component ID of gadget to find
 */
ResWindow::gadget_iterator ResWindow::find_gadget(ComponentId component_id)
{
	gadget_iterator i = gadget_begin();
	for (; i != gadget_end(); ++i)
	{
		if (int_value(i._offset + 24) == component_id) break;
	}

	return i; 
}

/**
 * Find gadget with given id
 *
 * @param component_id component ID of gadget to find
 */
ResWindow::const_gadget_iterator ResWindow::find_gadget(ComponentId component_id) const
{
	const_gadget_iterator i = gadget_begin();
	for (; i != gadget_end(); ++i)
	{
		if (int_value(i._offset + 24) == component_id) break;
	}

	return i; 
}

/**
* Get a copy of gadget with given component id
* @param component_id to find
* @throws ResGadgetNotFound if component can't be found
*/
ResGadget ResWindow::gadget(ComponentId component_id) const
{
	const_gadget_iterator i = find_gadget(component_id);
	if (i == gadget_end()) throw ResGadgetNotFound(name(), component_id);
	return *i;
}

/**
 * Check if the window contains a gadget with the given id.
 *
 * @param component_id component ID of gadget to check
 * @returns true if window contains a gadget with the given ID
 */
bool ResWindow::contains_gadget(ComponentId component_id) const
{
	return find_gadget(component_id) != gadget_end();
}

/**
 * Adds the gadget to the end of the window
 *
 * @param gadget - gadget to add
 * @throws ResGadgetExists if the gadget's component id is already in the window
 */
void ResWindow::add_gadget(const ResGadget &gadget)
{
	insert_gadget(gadget_end(), gadget);
}

/**
 * Replace menu gadget with same component id as given gadget
 *
 * @param gadget gadget to replace
 * @throws ResGadgetNotFound if a gadget with the same component ID does not exist
 */
void ResWindow::replace_gadget(const ResGadget &gadget)
{
	gadget_iterator pos = find_gadget(gadget.component_id());
	if (pos == gadget_end()) throw ResGadgetNotFound(name(), gadget.component_id());

	make_writeable();

	int old_size = ResGadget::gadget_size(*((int *)(object_header()->body + pos._offset + 4)));
	replace_component(pos._offset, old_size, gadget._impl);
}


/**
 * Erase gadget with specific id
 *
 * @param id component id
 * @throws ResGadgetNotFound if id is not in the Window
 */
void ResWindow::erase_gadget(ComponentId id)
{
	gadget_iterator pos = find_gadget(id);
	if (pos == gadget_end()) throw ResGadgetNotFound(name(), id);
	erase_gadget(pos);
}

/**
 * Insert gadget at given position
 *
 * @param pos position to insert the gadget before
 * @param gadget gadget to insert
 * @returns iterator to inserted gadget
 * @throws ResGadgetExists if a gadget with the same component ID already exists in the window
 */
ResWindow::gadget_iterator ResWindow::insert_gadget(gadget_iterator pos, const ResGadget &gadget)
{
	gadget_iterator i = find_gadget(gadget.component_id());
	if (i != gadget_end()) throw ResGadgetExists(name(), gadget.component_id());

	make_writeable();

	int insert_location = pos._offset;
	if (insert_location == 0)
	{
		if (gadget_start()) insert_location = gadget_start() - object_header()->body;
		else
		{
			insert_location = object_header()->body_size;
			int_value(44, (int)(object_header()->body + insert_location));
		}
		pos._offset = insert_location;
	}
	insert_component(insert_location, gadget._impl);

	int_value(40, num_gadgets()+1); // Update gadget count

	return pos;
}

/**
 * Replace the gadget at the given iterator position
 *
 * @param pos iterator pointing at gadget to replace
 * @param gadget gadget to use as replacement
 * @returns iterator to replacement gadget
 * @throws ResGadgetExists if component ID of replacement gadget matches an
 * existing gadget in the window apart from the one replaced.
 */
ResWindow::gadget_iterator ResWindow::replace_gadget(gadget_iterator pos, const ResGadget &gadget)
{
	if (gadget.component_id() != int_value(pos._offset + 24)
		&& find_gadget(gadget.component_id()) != gadget_end())
	{
		throw ResGadgetExists(name(), gadget.component_id());
	}

	make_writeable();

	int old_size = ResGadget::gadget_size(*((int *)(object_header()->body + pos._offset + 4)));
	replace_component(pos._offset, old_size, gadget._impl);

	return pos;
}

/**
 * Delete the component at the given location
 *
 * @param pos iterator to gadget to be deleted
 * @return iterator to gadget following deleted gadget.
 */
ResWindow::gadget_iterator ResWindow::erase_gadget(gadget_iterator pos)
{
	make_writeable();
	int gadget_size = ResGadget::gadget_size(*((int *)(object_header()->body + pos._offset + 4)));
	erase_component(pos._offset, gadget_size);
	int count = num_gadgets()-1;
	int_value(40, count); // Update gadget count
	if (count == 0) int_value(44, 0); // Update gadget pointer

	return pos;
}

/**
 * Get a copy of the shortcut at the given offset.
 *
 * This is for specialised use only - use shortcut iterators for
 * normal access to the shortcuts in this window
 *
 * @param item_offset offset to shortcut in the window
 * @return copy of shortcut at the given offset.
 */
ResShortcut ResWindow::shortcut_at_offset(int item_offset) const
{
	int *shortcut_header = (int *)(object_header()->body + item_offset);

	char *new_shortcut = new char[SHORTCUT_SIZE];
	memcpy(new_shortcut, shortcut_header, SHORTCUT_SIZE);

	ResData *item_data;

	if (_impl->read_only())
	{
		item_data = ResData::copy_component_from_read_only(new_shortcut, _impl->header(), item_offset, SHORTCUT_SIZE);
	} else
	{
		item_data = component_data(new_shortcut, item_offset, SHORTCUT_SIZE);
	}
	ResShortcut shortcut(new_shortcut, item_data);

    return shortcut;
}

/**
 * Get offset of first shortcut in window
 *
 * @returns offset to first shortcut or 0 if none
 */
int ResWindow::first_shortcut_offset() const
{
	return(shortcut_start()) ? shortcut_start() - object_header()->body : 0;
}

/**
 * Get offset to end of shortcuts in window
 *
 * @returns offset to end of shortcuts or 0 if none
 */
int ResWindow::end_shortcut_offset() const
{
	int offset = 0;
	if (shortcut_start())
	{
		if (gadget_start()) offset = gadget_start() - object_header()->body;
		else offset = object_header()->body_size;
	}
	return offset;
}

/**
 * Find shortcut for the given key
 *
 * @param key_code key code for shortcut
 * @returns shortcut_iterator or shortcut_end() if not found
 */
ResWindow::shortcut_iterator ResWindow::find_shortcut(int key_code)
{
	shortcut_iterator i = shortcut_begin();
	for (; i != shortcut_end(); ++i)
	{
		if (int_value(i._offset + 4) == key_code) break;
	}

	return i; 
}

/**
 * Find shortcut with given key
 *
 * @param key_code key code for shortcut
 * @returns const_shortcut_iterator or shortcut_cend() if not found
 */
ResWindow::const_shortcut_iterator ResWindow::find_shortcut(int key_code) const
{
	const_shortcut_iterator i = shortcut_begin();
	for (; i != shortcut_end(); ++i)
	{
		if (int_value(i._offset + 4) == key_code) break;
	}

	return i; 
}

/**
* Get a copy of shortcut with key_code
* @param key_code to find
* @throws ResShortcutNotFound if shortcut can't be found
*/
ResShortcut ResWindow::shortcut(int key_code) const
{
	const_shortcut_iterator i = find_shortcut(key_code);
	if (i == shortcut_end()) throw ResShortcutNotFound(name(), key_code);
	return *i;
}

/**
 * Returns true if Window contains a shortcut
 * with the given id
 */
bool ResWindow::contains_shortcut(int key_code) const
{
	return find_shortcut(key_code) != shortcut_end();
}

/**
 * Adds the shortcut to the end of the window
 *
 * @param shortcut - shortcut to add
 * @throws ResShortcutExists if the shortcut's key_code is already in the window
 */
void ResWindow::add_shortcut(const ResShortcut &shortcut)
{
	insert_shortcut(shortcut_end(), shortcut);
}

/**
 * Replace menu shortcut with same key_code as given shortcut
 *
 * @param shortcut shortcut to replace
 * @throws ResShortcutNotFound if shortcut with same key code is not in the window
 */
void ResWindow::replace_shortcut(const ResShortcut &shortcut)
{
	shortcut_iterator pos = find_shortcut(shortcut.key_code());
	if (pos == shortcut_end()) throw ResShortcutNotFound(name(), shortcut.key_code());

	make_writeable();

	replace_component(pos._offset, SHORTCUT_SIZE, shortcut._impl);
}


/**
 * Erase shortcut with specific key_code
 *
 * @param key_code WIMP key code of shortcut to erase
 * @throws ResShortcutNotFound if key_code is not in the Window
 */
void ResWindow::erase_shortcut(int key_code)
{
	shortcut_iterator pos = find_shortcut(key_code);
	if (pos == shortcut_end()) throw ResShortcutNotFound(name(), key_code);
	erase_shortcut(pos);
}

/**
 * Insert shortcut at given position
 *
 * @param pos iterator to position to insert the shortcut
 * @param shortcut shortcut to insert
 * @returns iterator to inserted shortcut
 * @throws ResShortcutExists if a shortcut with the same key code already exists.
 */
ResWindow::shortcut_iterator ResWindow::insert_shortcut(shortcut_iterator pos, const ResShortcut &shortcut)
{
	shortcut_iterator i = find_shortcut(shortcut.key_code());
	if (i != shortcut_end()) throw ResShortcutExists(name(), shortcut.key_code());

	make_writeable();

	int insert_location = pos._offset;
	if (insert_location == 0)
	{
		if (shortcut_start()) insert_location = shortcut_start() - object_header()->body;
		if (insert_location == 0)
		{
			if (gadget_start()) insert_location = gadget_start() - object_header()->body; // Insert before gadgets
			else insert_location = object_header()->body_size;
			int_value(36, (int)(object_header()->body + insert_location));
		}
		pos._offset = insert_location;
	}
	insert_component(insert_location, shortcut._impl);

	int_value(32, num_shortcuts()+1); // Update shortcut count

	return pos;
}

/**
 * Replace the shortcut at the given iterator position
 *
 * @param pos position of shortcut to replace
 * @param shortcut replacement shortcut
 * @returns iterator to replacement shortcut
 * @throws ResShortcutExists if a shortcut apart from the replaced shortcut exists
 * with the same key code as the replacement shortcut.
 */
ResWindow::shortcut_iterator ResWindow::replace_shortcut(shortcut_iterator pos, const ResShortcut &shortcut)
{
	if (shortcut.key_code() != int_value(pos._offset + 4)
		&& find_shortcut(shortcut.key_code()) != shortcut_end())
	{
		throw ResShortcutExists(name(), shortcut.key_code());
	}

	make_writeable();

	replace_component(pos._offset, SHORTCUT_SIZE, shortcut._impl);

	return pos;
}

/**
 * Delete the component at the given location
 *
 * @param pos iterator to shortcut to erase
 * @returns iterator to shortcut following erased shortcut
 */
ResWindow::shortcut_iterator ResWindow::erase_shortcut(shortcut_iterator pos)
{
	make_writeable();
	erase_component(pos._offset, SHORTCUT_SIZE);
	int count = num_shortcuts()-1;
	int_value(32, count); // Update shortcut count
	if (count == 0) int_value(36, 0); // Update shortcut pointer

	return pos;
}


}
}
