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

#ifndef TBX_AUTOCREATE_LISTENER_H
#define TBX_AUTOCREATE_LISTENER_H

#include "listener.h"
#include "object.h"

namespace tbx
{
	/**
	 * Base Listener for auto create events.
	 *
	 * Derive from this class to create an auto create listener and
	 * override the auto_created method.
	 *
	 * @see Application::set_autocreate_listener
	 */
	class AutoCreateListener : public Listener
	{
	public:
		AutoCreateListener() {}
		virtual ~AutoCreateListener() {}

		/**
		 * This is called when an object has been automatically created
		 * by the toolbox if the listener has been registered with the
		 * application method set_autocreate_listener.
		 *
		 * @param template_name The name of the template used to create
		 *        the object.
		 * @param object The object that has been created.
		 */
		virtual void auto_created(std::string template_name, Object object) = 0;
	};
}

#endif

