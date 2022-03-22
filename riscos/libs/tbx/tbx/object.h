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

#ifndef TBX_OBJECT_H
#define TBX_OBJECT_H

#include <string>
#include "handles.h"
#include "pollinfo.h"

namespace tbx
{
	class Application;
	class Command;
	class UserEventListener;
	class ObjectDeletedListener;
	class Component;

	namespace res
	{
	    class ResObject;
	}

	/**
	 * Class to manipulate a toolbox object.
	 *
	 * This is the base class of all toolbox objects
	 */
	class Object
	{
	protected:
		Object(const res::ResObject &object_template);

	public:
		/**
		 * Constructs an object unattached to a toolbox object
		 *
		 * This object should be assigned to an object that is
		 * attached before calling any of its methods apart from
		 * null()
		 */
		Object() : _handle(NULL_ObjectId) {}

		/**
		 * Construct an object referencing the given toolbox object
		 *
		 * @param handle ObjectId of the toolbox object to reference
		 */
		Object(ObjectId handle) : _handle(handle) {};

		/**
		 * Construct an object referencing the same toolbox object as
		 * another
		 */
		Object(const Object &other) : _handle(other._handle) {};

		Object(const std::string &template_name);

		void delete_object();

		/**
		 * Check if object has been initialised
		 *
		 * @returns true if the object has not been initialised
		 */
		bool null() const {return (_handle == NULL_ObjectId);}
		/**
		 * Return the underlying toolbox object id this object references
		 *
		 * @returns ObjectId of the toolbox object
		 */
		ObjectId handle() const {return _handle;}

		/**
		 * Assign this object to reference the same toolbox object as another
		 *
		 * @param other object to assign reference from
		 */
		Object &operator=(const Object &other) {_handle = other._handle; return *this;}
		/**
		 * Check if this object references the same toolbox object as another
		 *
		 * @param other object to compare against
		 * @returns true if both objects refer to the same toolbox object
		 */
		bool operator==(const Object &other) const {return (_handle == other._handle);}
		/**
		 * Check if this object does not reference the same toolbox object as another
		 *
		 * @param other object to compare against
		 * @returns true if the objects refer to different toolbox object
		 */
		bool operator!=(const Object &other) const {return (_handle != other._handle);}

		/**
		 * Operator to check if an object has been initialised
		 *
		 * @returns true if the object has been initialised
		 */
		operator bool() const {return (_handle != NULL_ObjectId);}

		int toolbox_class() const;
		void check_toolbox_class(int class_id) const;

		void *client_handle() const;
		void client_handle(void *client_handle);

		Object parent_object() const;
		Component parent_component() const;
		Object ancestor_object() const;
		Component ancestor_component() const;

		// Visibility
		void show();
		void show_centered();
		void show_at_pointer();
		void hide();
		bool showing() const;

		// Listeners for all objects
		void add_command(int command_id, Command *command);
		void remove_command(int command_id, Command *command);

		void add_user_event_listener(int event_id, UserEventListener *listener);
		void remove_user_event_listener(int event_id, UserEventListener *listener);

		void add_object_deleted_listener(ObjectDeletedListener *listener);
		void remove_object_deleted_listener(ObjectDeletedListener *listener);

		void remove_all_listeners();

	protected:
		// Add listener helper
		void add_listener( int action, Listener *listener, RawToolboxEventHandler handler);
		void remove_listener(int action, Listener *listener);

		void set_handler(int action,  Listener *listener, RawToolboxEventHandler handler);

		// Helper functions for standard properties
		int int_property(int property_id) const;
		void int_property(int property_id, int value);

		std::string string_property(int property_id) const;
		void string_property(int property_id, const std::string &value);
		int string_property_length(int property_id) const;

	    bool bool_property(int property_id) const;
	    void bool_property(int property_id, bool value);

	protected:
	    /**
	     * Handle for toolbox object
	     */
		ObjectId _handle;
	};
}

#endif
