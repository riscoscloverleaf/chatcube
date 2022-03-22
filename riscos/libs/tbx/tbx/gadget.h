/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2015 Alan Buckley   All Rights Reserved.
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

#ifndef TBX_GADGET_H
#define TBX_GADGET_H

#include "window.h"
#include "component.h"
#include <vector>

namespace tbx
{
	class Loader;

    /**
     * This is the base class for all Gadgets.
     *
     * Gadget are the UI elements that are contained in a window.
     * e.g. Text fields, buttons etc.
     *
     * Each gadget has its own ComponentId that is set in the toolbox
     * resources.
     *
     * A gadget can be retrieved by using its ComponentId in the
     * window.gadget() method call.
     */
	class Gadget : public tbx::Component
	{
	private:
		Gadget(ObjectId handle, ComponentId id) : Component(handle, id) {};
		friend class Window;

	public:
		/**
		 * Construct an uninitialised gadget.
		 *
		 * No other methods should be used until this has been assigned
		 * to a an initialised gadget.
		 */
		Gadget() {}

		/**
		 * Construct a gadget as a copy of another gadget object.
		 *
		 * Both this and the original refer to the same underlying toolbox gadget.
		 */
		Gadget(const Gadget &other) : Component(other.handle(), other.id()) {};

		/**
		 * Construct a gadget as a copy of another component object.
		 *
		 * Both this and the original refer to the same underlying toolbox gadget.
		 *
		 * @throws ObjectClassError if this is not a gadget.
		 *                          i.e. it is not a Component of a window.
		 */
		Gadget(const Component &other) : Component(other) {Window check(_handle);}

		/**
		 * Get the window that contains this gadget
		 */
		Window window() {return Window(_handle);}

		/**
		 * Get the window that contains this gadget
		 */
		Window window() const {return Window(_handle);}

		/**
		 * Assign this gadget to refer to the same underlying toolbox gadget
		 * as another gadget.
		 */
		Gadget &operator=(const Gadget &other) {_handle = other._handle; _id = other._id; return *this;}

		/**
		 * Assign this gadget to refer to the same underlying toolbox gadget
		 * as the specified Component.
		 *
		 * @throws ObjectClassError if this is not a gadget.
		 *                          i.e. it is not a Component of a window.
		 */
		Gadget &operator=(const Component &other) {Window check(_handle); _handle = other.handle(); _id = other.id(); return *this;}

		/**
		 * Check if this gadget refers to the same underlying toolbox component
		 * as the given Component.
		 * @returns true if both refer to the same underlying toolbox component
		 */
		bool operator==(const Component &other) const {return (_handle == other.handle() && _id == other.id());}

		/**
		 * Check if this gadget refers to the same underlying toolbox component
		 * as the given Component.
		 * @returns true if both do not refer to the same underlying toolbox component
		 */
		bool operator!=(const Component &other) const {return (_handle != other.handle() || _id != other.id());}

		/**
		 * Check if this gadget refers to the same underlying toolbox component
		 * as the given Gadget.
		 * @returns true if both refer to the same underlying toolbox component
		 */
		bool operator==(const Gadget &other) const {return (_handle == other.handle() && _id == other.id());}

		/**
		 * Check if this gadget refers to the same underlying toolbox component
		 * as the given Gadget.
		 * @returns true if both do not refer to the same underlying toolbox component
		 */
		bool operator!=(const Gadget &other) const {return (_handle != other.handle() || _id != other._id);}

		void check_toolbox_class(int class_id) const;

		int toolbox_class() const;
		int toolbox_class_and_size() const;

		// Attributes
		unsigned int flags() const;
		void set_flag(unsigned int set);
		void clear_flag(unsigned int clear);

		void fade();
		void unfade();
		void fade(bool fade);
		bool faded() const;

		void focus();

		std::vector<IconHandle> icon_list();

		BBox bounds() const;
		void bounds(const BBox &bounds);
		Point bottom_left() const;
		Point top_left() const;
		void move_to(const Point &pos);
		void move_to(int x, int y);
		void move_by(int bx, int by);
		Size size() const;
		void size(const Size &sz);

		void add_loader(Loader *loader, int file_type = -2);
		void remove_loader(Loader *loader, int file_type = -2);

	};
}

#endif
