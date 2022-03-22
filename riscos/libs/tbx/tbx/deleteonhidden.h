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

#ifndef TBX_DELETEONHIDDEN_H_
#define TBX_DELETEONHIDDEN_H_

#include "hasbeenhiddenlistener.h"
#include "object.h"

namespace tbx
{
	/**
	 * Templated object has been hidden listener to delete a C++ object when a
	 * toolbox object gets hidden
	 *
	 * This class deletes itself once used so should always be
	 * allocated with new.
	 */
	template<class T> class DeleteClassOnHidden : public tbx::HasBeenHiddenListener
	{
		T *_class_to_delete;
	public:
		/**
		 * Constructor parsing object to delete when the toolbox object
		 * is hidden.
		 *
		 * @param class_to_delete C++ object to delete.
		 */
		DeleteClassOnHidden(T *class_to_delete) : _class_to_delete(class_to_delete) {}
		/**
		 * Delete the given C++ class and this object on event received
		 *
		 * @param hidden_event details of the has been hidden event
		 */
		virtual void has_been_hidden(const EventInfo &hidden_event)
		{
			delete _class_to_delete;
			delete this;
		}
	};

	/**
	 * Class to delete a toolbox object when it has been hidden.
	 *
	 * This class deletes itself once used so should always be
	 * allocated with new.
	 */
	class DeleteObjectOnHidden  : public tbx::HasBeenHiddenListener
	{
	public:
		virtual ~DeleteObjectOnHidden() {}

		/**
		 * Overridden has_been_hidden call back to delete the toolbox
		 * object that raised the event
		 *
		 * @param hidden_event details of the has been hidden event
		 */
		virtual void has_been_hidden(const EventInfo &hidden_event)
		{
			hidden_event.id_block().self_object().delete_object();
			delete this;
		}
	};

	/**
	 * Templated object has been hidden listener to delete a C++ object and
	 * the toolbox object that generated the event when a toolbox object
	 * gets hidden
	 *
	 * This class deletes itself once used so should always be
	 * allocated with new.
	 */
	template<class T> class DeleteClassAndObjectOnHidden : public tbx::HasBeenHiddenListener
	{
		T *_class_to_delete;
	public:
		/**
		 * Constructor parsing object to delete when the toolbox object
		 * is hidden.
		 *
		 * @param class_to_delete C++ object to delete.
		 */
		DeleteClassAndObjectOnHidden(T *class_to_delete) : _class_to_delete(class_to_delete) {}
		/**
		 * Delete the given C++ class, the toolbox object and this
		 * C++ object on event received
		 *
		 * @param hidden_event details of the has been hidden event
		 */
		virtual void has_been_hidden(const EventInfo &hidden_event)
		{
			delete _class_to_delete;
			hidden_event.id_block().self_object().delete_object();
			delete this;
		}
	};
}

#endif /* DELETEONHIDDEN_H_ */
