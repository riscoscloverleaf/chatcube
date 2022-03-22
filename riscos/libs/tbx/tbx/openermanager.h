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
 * openermanager.h
 *
 *  Created on: 23 Nov 2010
 *      Author: alanb
 */

#ifndef TBX_OPENERMANAGER_H_
#define TBX_OPENERMANAGER_H_

#include "wimpmessagelistener.h"
#include "loader.h"

namespace tbx {

/**
 * Internal class to help implement data openers
 */
class OpenerManager: public tbx::WimpRecordedMessageListener
{
	static OpenerManager *_instance;
	struct OpenerItem
	{
		int file_type;
		Loader *loader;
		OpenerItem *next;
	} *_first;

public:
	OpenerManager();
	virtual ~OpenerManager();

	/**
	 * Get the single instance of the opener manager
	 * used for this application
	 *
	 * @returns the OpenerManager instance
	 */
	static OpenerManager *instance() {return _instance;}

	virtual void recorded_message(WimpMessageEvent &event, int reply_to);

	void add_opener(int file_type, Loader *loader);
	void remove_opener(int file_type, Loader *loader);


};

}

#endif /* OPENERMANAGER_H_ */
