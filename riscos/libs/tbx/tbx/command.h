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


#ifndef TBX_COMMAND_H
#define TBX_COMMAND_H

#include "listener.h"

namespace tbx
{
	/**
	 * Base class for commands in tbx
	 */
	class Command : public Listener
	{
	public:
		Command() {}
		virtual ~Command() {};

		/**
		 * This method is call to execute the command
		 */
		virtual void execute() = 0;
	};


	/**
	 * Template to create a command that calls a member function
	 */
	template<class T> class CommandMethod : public Command
	{
		T *_call;
		void (T::*_method)();

	public:
		/**
		 * Construct the Command method for the given object and method
		 * on it.
		 *
		 * @param call The object the method will be called on
		 * @param method The method on the object to call
		 */
		CommandMethod(T *call, void (T::*method)()) : _call(call), _method(method) {}

		/**
		 * Calls the method on the object given in the constructor
		 */
		virtual void execute()
		{
			(_call->*_method)();
		}
	};
}

#endif

