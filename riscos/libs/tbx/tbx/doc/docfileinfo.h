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
 * docfileinfo.h
 *
 *  Created on: 13 Oct 2010
 *      Author: alanb
 */

#ifndef TBX_DOCFILEINFO_H_
#define TBX_DOCFILEINFO_H_

#include "../autocreatelistener.h"
#include "../abouttobeshownlistener.h"

namespace tbx
{
namespace doc
{

/**
 * Show the File information dialog box and automatically
 * fill it in from the document in it's ancestor window.
 *
 * Requires a resource called "FileInfo" which is shared and
 * has generate event before showing set.
 */
class DocFileInfo:
	public tbx::AutoCreateListener,
	public tbx::AboutToBeShownListener
{
public:
	DocFileInfo();
	virtual ~DocFileInfo() {};

	virtual void auto_created(std::string template_name, tbx::Object object);
	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
};

}
}

#endif /* TBX_DOCFILEINFO_H_ */
