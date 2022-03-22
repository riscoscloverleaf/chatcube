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

#ifndef TBX_OBJECTDELETE_H_
#define TBX_OBJECTDELETE_H_

#include "application.h"
#include "objectdeletedlistener.h"

namespace tbx
{
	/**
	 * Templated object deleted listener to delete a C++ object when a
	 * toolbox object gets deleted
	 *
	 * e.g.
	 * @code
	 *   // Delete this C++ object of type MainWindow when the toolbox object is deleted
	 *   window.add_object_deleted_listener(new ObjectDeleteClass<MainWindow>(this));
	 * @endcode
	 */
	template<class T> class ObjectDeleteClass : public tbx::ObjectDeletedListener
	{
		T *_class_to_delete;
	public:
		/**
		 * Constructor with C++ object to delete.
		 *
		 * This should always be allocated using the new operator as this
		 * class is automatically deleted when the listener is triggered.
		 *
		 * @param class_to_delete instance of C++ object to delete
		 */
		ObjectDeleteClass(T *class_to_delete) : _class_to_delete(class_to_delete) {}
		/**
		 * Delete the given C++ class and this object on event received
		 *
		 * @param object object that has been deleted.
		 */
		virtual void object_deleted(Object &object)
		{
			delete _class_to_delete;
			delete this;
		}
	};

}

#endif /* OBJECTDELETE_H_ */
