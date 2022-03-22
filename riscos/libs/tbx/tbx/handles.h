/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2014 Alan Buckley   All Rights Reserved.
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

#ifndef TBX_HANDLES_H_
#define TBX_HANDLES_H_

namespace tbx
{
	/** Type for underlying toolbox object id */
	typedef unsigned int ObjectId;
	 /** Type for underlying toolbox component id */
	typedef int ComponentId;
	/** Type for WIMP windows handle */
	typedef int WindowHandle;
	/** Type for WIMP icon handle */
	typedef int IconHandle;

	/**
	* NULL object id.
	*/
	const ObjectId NULL_ObjectId = ObjectId(0);
	/**
	* NULL component id.
	*/
	const ComponentId NULL_ComponentId = ComponentId(-1);

	/**
	 * Type for Resource handle return from the toolbox
	 */
	typedef void *ResHandle;

    /**
    * Type for a Wimp Task
    */
    typedef int TaskHandle;
}

#endif /* HANDLES_H_ */
