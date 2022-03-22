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

#ifndef AUTOCREATE_H_
#define AUTOCREATE_H_

#include "application.h"
#include "autocreatelistener.h"

namespace tbx
{
	/**
	 * Templated auto create listener to create a C++ object when a
	 * toolbox object gets auto created.
	 *
	 * The C++ object must have a constructor that takes an Object argument
	 */
	template<class T> class AutoCreateClass : public tbx::AutoCreateListener
	{
	public:
		/**
		 * Create a new C++ class for the autocreated object
		 */
		virtual void auto_created(std::string template_name, Object object)
		{
			new T(object);
		}
	};

	/**
	 * Templated auto create listener to create a C++ object when a
	 * toolbox object gets auto created.
	 *
	 * The C++ object must have a constructor that takes an Object argument
	 *
	 * Once run this listener has run it removes itself from the application
	 * and deletes itself.
	 */
	template<class T> class AutoCreateClassOnce : public tbx::AutoCreateListener
	{
	public:
		/**
		 * Create a new C++ class for the auto created toolbox object
		 * and remove the listener from the application
		 */
		virtual void auto_created(std::string template_name, Object object)
		{
			new T(object);
			app()->clear_autocreate_listener(template_name);
			delete this;
		}
	};

}

#endif /* AUTOCREATE_H_ */
