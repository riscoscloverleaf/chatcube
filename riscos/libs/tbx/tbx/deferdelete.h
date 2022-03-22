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
/*
 * deferdelete.h
 *
 *  Created on: 9 Feb 2011
 *      Author: alanb
 */

#ifndef TBX_DEFERDELETE_H_
#define TBX_DEFERDELETE_H_

#include "application.h"

namespace tbx
{

//! @cond INTERNAL

/**
 * Internal command to implement defered delete functionality
 */
template<class T> class DeferDeleteCommand : public Command
{
	T *_to_delete;
	DeferDeleteCommand(T *obj) : _to_delete(obj) {}
public:
	static void Create(T *obj)
	{
		DeferDeleteCommand *ddc = new DeferDeleteCommand(obj);
		tbx::app()->add_idle_command(ddc);
	}

	/**
	 * Command executed so delete object and this
	 */
	virtual void execute()
	{
		tbx::app()->remove_idle_command(this);
		delete _to_delete;
		delete this;
	}
};

//! @endcond

/**
 * Function to defer the deletion of an object until the
 * next idle (null) wimp poll.
 *
 * @param obj object to delete.
 */
template<class T> void defer_delete(T * obj)
{
	DeferDeleteCommand<T>::Create(obj);
}

}



#endif /* DEFERDELETE_H_ */
