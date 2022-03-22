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

#include "iconbar.h"
#include "swixcheck.h"
#include "swis.h"
#include "iconbarclicklistener.h"
#include "command.h"
#include "commandrouter.h"
#include "loadermanager.h"
#include "res/resiconbar.h"


namespace tbx {

Iconbar::Iconbar(const res::ResIconbar &object_template) : Object(object_template) {}

// Internal event/command routers
static void iconbar_clicked_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	IconbarClickEvent event(id_block, data);
	static_cast<IconbarClickListener *>(listener)->iconbar_clicked(event);
}

static void iconbar_select_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	if ((data.word[3] & 1) == 0)
		static_cast<Command *>(listener)->execute();
}

static void iconbar_adjust_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	if ((data.word[3] & 1) != 0)
		static_cast<Command *>(listener)->execute();
}

/************************** PROPERTIES ******************/

/**
 * Return the underling WIMP icon handle of the icon bar
 */
int Iconbar::icon_handle() const
{
	return int_property(0);
}

/**
 * Set the menu for the icon bar
 *
 * @param m new menu for icon bar
 */
void Iconbar::menu(Menu m)
{
	int_property(1, (int)m.handle());
}

/**
 * Get the menu from the icon bar
 *
 * @returns current menu (may be a null menu if none has been assigned)
 */
Menu Iconbar::menu() const
{
	Menu m((ObjectId)int_property(2));
	return m;
}

/**
 * Set the help message of the icon bar
 *
 * @param value new help message
 */
void Iconbar::help_message(const std::string &value)
{
	string_property(7, value);
}

/**
 * Get the current help message for the icon bar
 *
 * @returns current help message
 */
std::string Iconbar::help_message() const
{
	return string_property(8);
}


/**
 * Return the length of the current help message
 *
 * @returns Number of characters for this property
 */
int Iconbar::help_message_length() const
{
	return string_property_length(8);
}


/**
 * Set the text for a text & sprite icon bar
 *
 * @param value new value of text
 * @throws OsError if the text is longer than the maximum size
 *                 specified when the Icon bar icon was created
 */
void  Iconbar::text(std::string value)
{
	string_property(9, value);
}

/**
 * Get the current text of the icon bar for a text & sprite icon bar
 *
 * @returns current text
 */
std::string Iconbar::text() const
{
	return string_property(10);
}
/**
 * Get the length of the icon bar text
 *
 * @return icon bar text length
 */
int Iconbar::text_length() const
{
	return string_property_length(10);
}

/**
 * Set the sprite to use for the icon bar
 *
 * @param value new sprite name
 */
void  Iconbar::sprite(const std::string &value)
{
	string_property(11, value);
}

/**
 * Get the sprite used by the icon bar
 *
 * @returns name of the sprite
 */
std::string Iconbar::sprite() const
{
	return string_property(12);
}

/**
 * Get sprite name length
 */
int Iconbar::sprite_length() const
{
	return string_property_length(12);
}


/************************** METHODS ******************/


/**
 * Set object to show when the icon bar is clicked with select
 *
 * @param show_object object to show
 */
void Iconbar::select_show(Object show_object)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 3;
	regs.r[1] = _handle;
	regs.r[0] = 1;
	regs.r[3] = show_object.handle();
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get object shown when the icon bar is clicked with select
 *
 * @returns object to show (may be null)
 */
Object Iconbar::select_show()
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 4;
	regs.r[1] = _handle;
	regs.r[0] = 1;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
	return Object((ObjectId)regs.r[3]);
}
/**
 * Set the object to show when the icon bar is clicked with adjust
 *
 * @param show_object object to show
 */
void Iconbar::adjust_show(Object show_object)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 3;
	regs.r[1] = _handle;
	regs.r[0] = 2;
	regs.r[3] = show_object.handle();
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}
/**
 * Get object shown when the icon bar is clicked with adjust
 *
 * @returns object to show (may be null)
 */
Object Iconbar::adjust_show()
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[2] = 4;
	regs.r[1] = _handle;
	regs.r[0] = 2;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
	return Object((ObjectId)regs.r[3]);
}

/**** EVENTS *****/


/**
 * Add listener for button click with the select or adjust mouse buttons.
 *
 * Requires the icon bar to generate the default clicked event.
 *
 * @param listener listener to add
 */
void Iconbar::add_click_listener(IconbarClickListener *listener)
{
	add_listener(0x82900, listener, iconbar_clicked_router);
}

/**
 * Remove listener for button click with the select or adjust mouse buttons.
 *
 * @param listener listener to remove
 */
void Iconbar::remove_click_listener(IconbarClickListener *listener)
{
	remove_listener(0x82900, listener);
}

/**
 * Add command to be run if the iconbar is clicked with the select or adjust
 * mouse buttons
 * @param command command to add
 */
void Iconbar::add_click_command(Command *command)
{
	add_listener(0x82900, command, command_router);
}

/**
 * Remove command to be run if the iconbar is clicked with the select or adjust
 * mouse buttons.
 *
 * @param command command to remove
 */
void Iconbar::remove_click_command(Command *command)
{
	remove_listener(0x82900, command);
}

/**
 * Add command to be run if the button is selected with the select mouse button
 *
 * @param command command to add
 */
void Iconbar::add_select_command(Command *command)
{
	add_listener(0x82900, command, iconbar_select_router);
}

/**
 * Remove command to be run if the button is selected with the select mouse button
 *
 * @param command command to remove
 */
void Iconbar::remove_select_command(Command *command)
{
	remove_listener(0x82900, command);
}

/**
 * Add command to be run if the button is selected with the adjust mouse button
 * only
 *
 * @param command command to add
 */
void Iconbar::add_adjust_command(Command *command)
{
	add_listener(0x82900, command, iconbar_adjust_router);
}

/**
 * Remove command to be run if the button is selected with the adjust mouse button
 * only
 *
 * @param command command to remove
 */
void Iconbar::remove_adjust_command(Command *command)
{
	remove_listener(0x82900, command);
}


/**
 * Add a file loader.
 *
 * @param loader the loader to add
 * @param file_type the file type for the loader or -2 (the default) for
 *        any type.
 */
void Iconbar::add_loader(Loader *loader, int file_type /*=-2*/)
{
	LoaderManager *manager = LoaderManager::instance();
	if (manager == 0) manager = new LoaderManager();
	manager->add_loader(_handle, NULL_ComponentId, file_type, loader);
}

/**
 * Remove a file loader.
 *
 * @param loader the loader to remove
 * @param file_type the file type for the loader or -2 (the default) for
 *        any type.
 */
void Iconbar::remove_loader(Loader *loader, int file_type /*=-2*/)
{
	LoaderManager *manager = LoaderManager::instance();
	if (manager != 0)
		manager->remove_loader(_handle, NULL_ComponentId, file_type, loader);
}

}

