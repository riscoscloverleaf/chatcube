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

#include "menu.h"
#include "swixcheck.h"
#include "swis.h"
#include "abouttobeshownlistener.h"
#include "hasbeenhiddenlistener.h"
#include "submenulistener.h"
#include "menuselectionlistener.h"
#include "command.h"
#include "commandrouter.h"
#include "tbxexcept.h"
#include "res/resmenu.h"

#include <cstring>

namespace tbx {

static void submenu_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	SubMenuEvent event(id_block, data);
	static_cast<SubMenuListener *>(listener)->submenu(event);
}

static void menu_selection_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	tbx::EventInfo event(id_block, data);
	static_cast<MenuSelectionListener *>(listener)->menu_selection(event);
}

/**
 * Retrieve the menu item for a given component id.
 *
 * Note: There is no check the id is valid.
 */
MenuItem Menu::item(ComponentId id)
{
	return MenuItem(_handle, id);
}

/**
 * Add listener triggered when a menu is about to be shown
 *
 * This event is only triggered if the menu has been set to deliver
 * the default event before showing.
 *
 * @param listener to add
 */
void Menu::add_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	add_listener(0x828c0, listener, about_to_be_shown_router);
}

/**
 * Remover listener triggered when a menu is about to be shown
 *
 * @param listener to remove
 */
void Menu::remove_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	remove_listener(0x828c0, listener);
}

/**
 * Add listener triggered when a menu is hidden
 *
 * This event is only triggered if the menu has been set to deliver
 * the default event when hidden.
 *
 * @param listener to add
 */

void Menu::add_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	add_listener(0x828c1, listener, has_been_hidden_router);
}

/**
 * Remove listener triggered when a menu is about to be shown
 *
 * @param listener to remove
 */
void Menu::remove_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	remove_listener(0x828c1, listener);
}

/**
 * Add listener for mouse moving over the menu arrow when no other toolbox
 * event is assosiated with this event.
 *
 * @param listener to add
 */
void Menu::add_submenu_listener(SubMenuListener *listener)
{
	add_listener(0x828c2, listener, submenu_router);
}

/**
 * Remove listener for mouse moving over the menu arrow when no other toolbox
 * event is assosiated with this event.
 *
 * @param listener to remove
 */
void Menu::remove_submenu_listener(SubMenuListener *listener)
{
	remove_listener(0x828c2, listener);
}

/**
 * Add listener for menu item selection. This is only generated if no other
 * toolbox event is assosiated with this event.
 *
 * This will be called for any menu item that has no other event
 * associated with it. (including those with their own menu selection
 * listener).
 *
 * @param listener to add
 */
void Menu::add_selection_listener(MenuSelectionListener *listener)
{
	add_listener(0x828c3, listener, menu_selection_router);
}

/**
 * Remove menu selection listener
 *
 * @param listener to remove
 */
void Menu::remove_selection_listener(MenuSelectionListener *listener)
{
    remove_listener(0x828c3, listener);
}

/**
 * Add a new menu entry to the menu
 *
 * If the component id of the menu item definition is set to -1 then the
 * lowest available component id is used.
 *
 * @param res_item definition of new entry.
 * @param after item to add it after or 0 (the default) to add to end of the menu
 *
 * @returns MenuItem added
 * @throws OsError add failed.
 */
MenuItem Menu::add(const res::ResMenuItem &res_item, MenuItem *after /*= 0*/)
{
	ComponentId id;
	swix_check(_swix(0x44ec6, _INR(0,4)|_OUT(0),
			0,
			_handle,
			20,
			(after) ? after->id() : -2,
			res_item.header(),
			&id
			));

	return MenuItem(_handle, id);
}

/**
 * Insert a new menu entry in the menu
 *
 * If the component id of the menu item definition is set to -1 then the
 * lowest available component id is used.
 *
 * @param res_item definition of new entry.
 * @param before item to insert it before or 0 (the default) to insert
 *        at the beginning of the menu
 *
 * @returns MenuItem inserted
 * @throws OsError insert failed.
 */

MenuItem Menu::insert(const res::ResMenuItem &res_item, MenuItem *before /* = 0*/)
{
	ComponentId id;
	swix_check(_swix(0x44ec6, _INR(0,4)|_OUT(0),
			1,
			_handle,
			20,
			(before) ? before->id() : -1,
			res_item.header(),
			&id
			));

	return MenuItem(_handle, id);
}

/**
 * Erase the menu item with the given id
 *
 * @throws OsError erase failed.
 */
void Menu::erase(ComponentId id)
{
	swix_check(_swix(0x44ec6, _INR(0,3), 0, _handle, 21, id));
}

/**
 * Erase the menu item given.
 *
 * Sets item to uninitialised state.
 *
 * @throws OsError erase failed.
 */
void Menu::erase(MenuItem &item)
{
	erase(item.id());
	item._handle = NULL_ObjectId;
	item._id = NULL_ComponentId;
}

/**
 * Put a tick by the menu item.
 */
void MenuItem::tick(bool set)
{
	swix_check(_swix(0x44ec6, _INR(0,4), 0, _handle, 0, _id, set));
}

/**
 * Check if the menu item is ticked.
 */
bool MenuItem::ticked() const
{
	int ticked;
	swix_check(_swix(0x44ec6, _INR(0,3) | _OUT(0), 0, _handle, 1, _id, &ticked));

	return (ticked != 0);
}

/**
 * Fade the menu item
 */
void MenuItem::fade(bool set)
{
	swix_check(_swix(0x44ec6, _INR(0,4), 0, _handle, 2, _id, set));
}

/**
 * Check if the menu item is faded
 */
bool MenuItem::faded() const
{
	int faded;
	swix_check(_swix(0x44ec6, _INR(0,3) | _OUT(0), 0, _handle, 3, _id, &faded));

	return (faded != 0);
}

/**
 * Set the text for the menu item.
 *
 * @throws OsError the entries text buffer is not large enough to hold the text
 *                 or the entry is a sprite entry.
 */
void MenuItem::text(const std::string &text)
{
	return string_property(4, text);
}

/**
 * Get the text of the menu item.
 */
std::string MenuItem::text() const
{

	return string_property(5);
}

/**
 * Get the length of the buffer for the menu item text.
 */
int MenuItem::text_length() const
{
	return string_property_length(5);
}

/**
 * Set the sprite name for the menu item.
 *
 * @throws OsError the entries sprite name buffer is not large enough to hold
 *                 the name or the entry is a sprite entry.
 */
void MenuItem::sprite_name(const std::string &name)
{
	string_property(6, name);
}

/**
 * Get the sprite name for a sprite menu item
 */
std::string MenuItem::sprite_name() const
{
	return string_property(7);
}

/**
 * Get the length of the buffer for the sprite name.
 */

int MenuItem::sprite_name_length() const
{
	return string_property_length(7);
}

/**
 * Set the object to show as a submenu from this item
 */
void MenuItem::submenu(const Object &object)
{
	int_property(8, (int)object.handle());
}

/**
 * Clear the submenu from this item
 */
void MenuItem::clear_submenu()
{
	int_property(8, 0);
}

/**
 * Returns true if this object has a submenu
 */
bool MenuItem::has_submenu() const
{
	return (int_property(9) != 0);
}

/**
 * Return submenu or Object::null if none
 */
Object MenuItem::submenu()
{
	return Object((ObjectId)int_property(9));
}

/**
 * Set event to raise when mouse if moved over the submenu arrow.
 *
 * If id is 0 then Menu_SubMenu toolbox event is raised instead
 */
void MenuItem::submenu_event(int id)
{
	int_property(10, id);
}

/**
 * Get the event raised when the mouse is moved over the submenu arrow.
 *
 * If no event has been specified then 0 is returned.
 */
int MenuItem::submenu_event() const
{
	return int_property(11);
}


/**
 * Set object to show if menu item is clicked.
 *
 * @param object object to show
 * @param transient show object as transient (defaults to true)
 */
void MenuItem::click_show(const Object &object, bool transient /*= true*/)
{
	swix_check(_swix(0x44ec6, _INR(0,5),
			0,
			_handle,
			12,
			_id,
			(int)object.handle(),
			transient
			));
}

/**
 * Clear the click show object
 */
void MenuItem::clear_click_show()
{
	swix_check(_swix(0x44ec6, _INR(0,5),
			0,
			_handle,
			12,
			_id,
			0,
			0
			));
}

/**
 * Check if menu item has a click show object
 *
 * @returns true if click show defined
 */
bool MenuItem::has_click_show() const
{
	int handle, transient;
	swix_check(_swix(0x44ec6, _INR(0,3) |_OUTR(0,1),
			0,
			_handle,
			13,
			_id,
			&handle,
			&transient
			));
	return (handle != 0);
}

/**
 * Get click show object for menu item.
 *
 * @param transient pointer to variable to receive transient flag
 *  or 0 if not interested in transient flag. Defaults to 0
 * @returns object to be shown or Object::null if nothing shown
 */
Object MenuItem::click_show(bool *transient /*= 0*/)
{
	int handle, tranval;
	swix_check(_swix(0x44ec6, _INR(0,3) |_OUTR(0,1),
			0,
			_handle,
			13,
			_id,
			&handle,
			&tranval
			));
	if (transient) *transient = ((tranval & 1)!=0);

	return tbx::Object(handle);
}

/**
 * Set click event for menu item.
 *
 * @param id event id or 0 to generate Menu_Selection toolbox event
 */
void MenuItem::click_event(int id)
{
	int_property(14, id);
}

/**
 * Get the click event for this menu item
 *
 * @return click event or 0 if no event was specified
 */
int MenuItem::click_event() const
{
	return int_property(15);
}

/**
 * Set the help message for a menu item.
 *
 * @throws OsError the entries help text buffer is not large enough to hold
 *                 the message.
 */
void MenuItem::help_message(const std::string &msg)
{
	string_property(18, msg);

}

/**
 * Get the items help message
 */
std::string MenuItem::help_message() const
{
	return string_property(19);
}

/**
 * Get the length of the buffer for the help message
 */
int MenuItem::help_message_length() const
{
	return string_property_length(19);
}

/**
 * Add listener for mouse moving over the menu arrow when no other toolbox
 * event is associated with this event.
 *
 * @param listener to add
 */
void MenuItem::add_submenu_listener(SubMenuListener *listener)
{
	add_listener(0x828c2, listener, submenu_router);
}

/**
 * Remove listener for mouse moving over the menu arrow when no other toolbox
 * event is associated with this event.
 *
 * @param listener to remove
 */
void MenuItem::remove_submenu_listener(SubMenuListener *listener)
{
	remove_listener(0x828c2, listener);
}

/**
 * Add listener for menu item selection. This is only generated if no other
 * toolbox event is assosiated with this event.
 *
 * @param listener to add
 */
void MenuItem::add_selection_listener(MenuSelectionListener *listener)
{
	add_listener(0x828c3, listener, menu_selection_router);
}

/**
 * Remove menu selection listener
 *
 * @param listener to remove
 */
void MenuItem::remove_selection_listener(MenuSelectionListener *listener)
{
    remove_listener(0x828c3, listener);
}

/**
 * Add command to be run when this menu item is selected.
 * This is only generated if no other toolbox event is associated
 * with the menu items selection.
 *
 * @param command command to add
 */
void MenuItem::add_selection_command(Command *command)
{
	add_listener(0x828c3, command, command_router);
}

void MenuItem::remove_selection_command(Command *command)
{
    remove_listener(0x828c3, command);
}



}
