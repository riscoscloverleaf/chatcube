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


#include "resmenu.h"
#include "resexcept.h"

namespace tbx
{
namespace res
{

/**
 * Construct a ResMenuItem with flags unset, item text "" and show objects
 * and help message null.
 */
ResMenuItem::ResMenuItem()
: ResBase(new ResImpl(new char[MENU_ITEM_SIZE], 0, MENU_ITEM_SIZE, new ResData()))
{
	memset(_impl->body(), 0, MENU_ITEM_SIZE);
	init_message(8, ""); // Item text
	init_string(16, 0);  // click show
	init_string(20, 0);  // submenu show
	init_message(32, 0); // help message
}

/**
 * Protected constructor used by ResMenu
 */
ResMenuItem::ResMenuItem(void *item_header, int size, ResData *data)
: ResBase(new ResImpl(item_header, 0, size, data))
{
}

/**
 * Get a copy of item at given index
 *
 * @param index zero based item index.
 * @returns Copy of menu item at the given index
 * @throws std::range_error if index is not valid
 */
ResMenuItem ResMenu::item_at(int index) const
{
	if (index < 0 || index >= item_count()) throw std::range_error("Invalid menu index");
	int offset = index * MENU_ITEM_SIZE;
	return item_at_offset(offset + MENU_DATA_SIZE);
}


/**
 * Get a copy of an item at the given offset in the body
 *
 * There is no checking on the offset given so you must ensure it
 * is correct before using this function.
 *
 * @param item_offset offset of menu
 * @returns menu item
 */
ResMenuItem ResMenu::item_at_offset(int item_offset) const
{
	void *item_header = object_header()->body + item_offset;
	char *new_item = new char[MENU_ITEM_SIZE];
	memcpy(new_item, item_header, MENU_ITEM_SIZE);

	ResData *item_data;

	if (_impl->read_only())
	{
		item_data = ResData::copy_component_from_read_only(new_item, _impl->header(), item_offset, MENU_ITEM_SIZE);
	} else
	{
		item_data = component_data(new_item, item_offset, MENU_ITEM_SIZE);
	}
	ResMenuItem item(new_item, MENU_ITEM_SIZE, item_data);

    return item;
}

/**
 * Get constant iterator to first menu item
 *
 * @returns constant iterator to first menu item or end() if no menu items
 */
ResMenu::const_iterator ResMenu::begin() const
{
	return const_iterator(this, MENU_DATA_SIZE);
}

/**
 * Get constant iterator to end of menu items
 *
 * @returns constant iterator to then item after the last menu item
 */
ResMenu::const_iterator ResMenu::end() const
{
	return const_iterator(this, MENU_DATA_SIZE + MENU_ITEM_SIZE * item_count());
}

/**
 * Get constant iterator to first menu item
 *
 * @returns constant iterator to first menu item or end() if no menu items
 */
ResMenu::const_iterator ResMenu::cbegin() const
{
	return const_iterator(this, MENU_DATA_SIZE);
}

/**
 * Get constant iterator to end of menu items
 *
 * @returns constant iterator to then item after the last menu item
 */
ResMenu::const_iterator ResMenu::cend() const
{
	return const_iterator(this, MENU_DATA_SIZE + MENU_ITEM_SIZE * item_count());
}

/**
 * Find component with given id
 *
 * @param component_id id of component to find
 * @returns constant iterator to component or end()
 */
ResMenu::const_iterator ResMenu::find(ComponentId component_id) const
{
	const_iterator i = begin();
	for (; i != end(); ++i)
	{
		if (int_value(i._offset + 4) == component_id) break;
	}

	return i; 
}

/**
 * Get iterator to first menu item
 *
 * @returns iterator to first menu item or end() if no menu items
 */
ResMenu::iterator ResMenu::begin()
{
	return iterator(this, MENU_DATA_SIZE);
}

/**
 * Get iterator to end of menu items
 *
 * @returns iterator to then item after the last menu item
 */
ResMenu::iterator ResMenu::end()
{
	return iterator(this, MENU_DATA_SIZE + MENU_ITEM_SIZE * item_count());
}

/**
 * Find component with given id
 *
 * @param component_id id of component to find
 * @returns iterator to component or end()
 */
ResMenu::iterator ResMenu::find(ComponentId component_id)
{
	iterator i = begin();
	for (; i != end(); ++i)
	{
		if (int_value(i._offset + 4) == component_id) break;
	}

	return i; 
}

/**
* Get a copy of item with given component id
* @param component_id to find
* @throws ResMenuItemNotFound if component can't be found
*/
ResMenuItem ResMenu::item(ComponentId component_id) const
{
	const_iterator i = find(component_id);
	if (i == end()) throw ResMenuItemNotFound(name(), component_id);
	return *i;
}

/**
 * Check if menu contains a menu item
 * with the given id.
 *
 * @param component_id component ID to check
 * @returns true if the menu contains the component ID
 */
bool ResMenu::contains(ComponentId component_id) const
{
	return find(component_id) != end();
}

/**
 * Adds the item to the end of the menu
 *
 * @param item - menu item to add
 * @throws std::invalid_argument if the menu item component id is already in the menu
 */
void ResMenu::add(const ResMenuItem &item)
{
	insert(end(), item);
}

/**
 * Replace menu item with same component id as given item
 *
 * @param item Menu item resource to replace
 * @throws ResMenuItemNotFound if there is no existing item with
 * the same component ID
 */
void ResMenu::replace(const ResMenuItem &item)
{
	iterator pos = find(item.component_id());
	if (pos == end()) throw ResMenuItemNotFound(name(), item.component_id());

	make_writeable();
	replace_component(pos._offset, MENU_ITEM_SIZE, item._impl);
}


/**
 * Erase menu item with specific id
 *
 * @param component_id component id
 * @throws ResMenuItemNotFound if id is not in the menu
 */
void ResMenu::erase(ComponentId component_id)
{
	iterator pos = find(component_id);
	if (pos == end()) throw ResMenuItemNotFound(name(), component_id);
	erase(pos);
}

/**
 * Insert item at given position
 *
 * @param pos location to insert the item before
 * @param item menu item to insert
 * @throws ResMenuItemExists if the menu already contains an item with the
 * same component id
 */
ResMenu::iterator ResMenu::insert(iterator pos, const ResMenuItem &item)
{
	iterator i = find(item.component_id());
	if (i != end()) throw ResMenuItemExists(name(), item.component_id());

	make_writeable();

	int insert_location = pos._offset;
	insert_component(insert_location, item._impl);

	int_value(28, item_count()+1); // Update menu item count

	return pos;
}

/**
 * Replace the item at the given iterator position
 *
 * @param pos position of item to replace
 * @param item menu item to replace it with
 * @throws ResMenuItemExists if the menu already contains an item with the
 * same component ID unless it is the component being replaced.
 */
ResMenu::iterator ResMenu::replace(iterator pos, const ResMenuItem &item)
{
	if (item.component_id() != int_value(pos._offset + 4)
		&& find(item.component_id()) != end())
	{
		throw ResMenuItemExists(name(), item.component_id());
	}

	make_writeable();

	replace_component(pos._offset, MENU_ITEM_SIZE, item._impl);

	return pos;
}

/**
 * Delete the component at the given location
 *
 * @param pos location of item to delete
 * @returns iterator to next item in menu
 */
ResMenu::iterator ResMenu::erase(iterator pos)
{
	make_writeable();
	erase_component(pos._offset, MENU_ITEM_SIZE);
	int_value(28, item_count()-1); // Update menu item count

	return pos;
}


}
}
