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

#ifndef TBX_ICONBAR_H_
#define TBX_ICONBAR_H_

#include "object.h"
#include "menu.h"

namespace tbx
{
	class Loader;
	class IconbarClickListener;

	namespace res
	{
	    class ResIconbar;
	}

	/**
	 * Class to represent toolbox iconbar icon.
	 */
	class Iconbar : public Object
	{
	public:
		enum {TOOLBOX_CLASS = 0x82900};

		/**
		 * Construct an uninitiased Iconbar.
		 *
		 * The Iconbar should not be used until it has been initialised by
		 * assigning it to another initialised Iconbar.
		 */
		Iconbar() {};

		/**
		 * Construct a Iconbar from another Iconbar.
		 *
		 * Both Iconbars will refer to the same underlying toolbox Iconbar
		 */
		Iconbar(const Iconbar &other) : Object(other._handle) {}

		/**
		 * Construct a Iconbar from another toolbox object.
		 *
		 * The Iconbar and the object will refer to the same underlying toolbox Iconbar
		 * @throws ObjectClassError The object assigned to does not refer to a toolbox Iconbar
		 */
		Iconbar(const Object &other) : Object(other)	{check_toolbox_class(Iconbar::TOOLBOX_CLASS);}

		/**
		 * Construct a iconbar creating a toolbox iconbar object
		 *
		 * @param template_name name of icon bar template to use
		 */
		Iconbar(const char *template_name) : Object(template_name)	{check_toolbox_class(Iconbar::TOOLBOX_CLASS);}

		/**
		 * Construct an iconbar creating a toolbox iconbar object
		 *
		 * @param object_template in memory template used to create the toolbox iconbar
		 */
		Iconbar(const res::ResIconbar &object_template);

		/**
		 * Assign a Iconbar to an existing Iconbar.
		 *
		 * Both Iconbars will refer to the same underlying toolbox Iconbar
		 */
		Iconbar &operator=(const Iconbar &other) {_handle = other.handle(); return *this;}

		/**
		 * Assign a Iconbar to an existing object.
		 *
		 * The Iconbar and the object will refer to the same underlying toolbox Iconbar
		 * @throws ObjectClassError The object assigned to does not refer to a toolbox Iconbar
		 */
		Iconbar &operator=(const Object &other) {_handle = other.handle(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

		/**
		 * Checks if this Iconbar refers to the same underlying toolbox object as
		 * the given object.
		 * @returns true if the Iconbar and object refer to the same thing.
		 */
		bool operator==(const Object &other) const {return (_handle == other.handle());}

		/**
		 * Checks if this Iconbar refers to the same underlying toolbox object as
		 * the given object.
		 * @returns true if the Iconbar and object do not refer to the same thing.
		 */
		bool operator!=(const Object &other) const {return (_handle != other.handle());}


// Properties
		int icon_handle() const;
		void menu(Menu m);
		Menu menu() const;
		void  help_message(const std::string &value);
		std::string help_message() const;
		int help_message_length() const;
		void text(std::string value);
		std::string text() const;
		int text_length() const;
		void  sprite(const std::string &value);
		std::string sprite() const;
		int sprite_length() const;
		void select_show(Object show_object);
		Object  select_show();
		void adjust_show(Object show_object);
		Object adjust_show();

//TODO: void show(...) - priority show

// Listeners
		void add_click_listener(IconbarClickListener *listener);
		void remove_click_listener(IconbarClickListener *listener);
		void add_click_command(Command *command);
		void remove_click_command(Command *command);
		void add_select_command(Command *command);
		void remove_select_command(Command *command);
		void add_adjust_command(Command *command);
		void remove_adjust_command(Command *command);
		void add_loader(Loader *loader, int file_type = -2);
		void remove_loader(Loader *loader, int file_type = -2);
	};
};

#endif
