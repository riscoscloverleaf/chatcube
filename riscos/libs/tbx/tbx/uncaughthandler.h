/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2014 Alan Buckley   All Rights Reserved.
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
/*
 * UncaughtHandler.h
 *
 *  Created on: 16 Jan 2014
 *      Author: alanb
 */

#ifndef TBX_UNCAUGHTHANDLER_H_
#define TBX_UNCAUGHTHANDLER_H_

#include <stdexcept>

namespace tbx
{
/**
 * Interface to process uncaught exceptions in an application
 *
 * @see Application::uncaught_handler
 */
class UncaughtHandler
{
public:
	virtual ~UncaughtHandler() {}
	/**
	 * Method called when an uncaught exception has been detected.
	 *
	 * This method should report the error to the user and take
	 * any appropriate action.
	 *
	 * @param e pointer to std::exception caught or 0 if the exception
	 *  wasn't a standard exception.
	 */
	virtual void uncaught_exception(std::exception *e, int event_code) = 0;
};

}



#endif /* TBX_UNCAUGHTHANDLER_H_ */
