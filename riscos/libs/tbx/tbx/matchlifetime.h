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

#ifndef TBX_MATCHLIFETIME_H_
#define TBX_MATCHLIFETIME_H_

#include "autocreatelistener.h"
#include "objectdelete.h"
#include "application.h"

namespace tbx
{
     /**
	 * Templated class to match the lifetime of a toolbox object to
	 * a c++ class.
	 *
	 * Add it to the application as an auto create listener.
	 *
	 * The C++ object must have a constructor that takes an Object argument
	 */
	template<class T> class MatchLifetime : public tbx::AutoCreateListener
	{
	public:
		/**
		 * Constructor with no parameters. Add to application
		 * with Application::set_autocreate_listener
		 */
		MatchLifetime() {};
		/**
		 * Constructor with template name automatically sets
		 * autocreate listener on the application.
		 *
		 * @param template_name name of object template in resource
		 */
		MatchLifetime(const char *template_name)
		{
			app()->set_autocreate_listener(template_name, this);
		}

		/**
		 * Create a new C++ class for the auto created object
		 */
		virtual void auto_created(std::string template_name, Object object)
		{
			T *c = new T(object);
			object.add_object_deleted_listener(new tbx::ObjectDeleteClass<T>(c));
		}
	};
}

#endif /* TBX_MATCHLIFETIME_H_ */
