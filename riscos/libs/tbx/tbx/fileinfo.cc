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
 * fileinfo.cc
 *
 *  Created on: 01-Jul-2010
 *      Author: alanb
 */

#include "fileinfo.h"
#include "abouttobeshownlistener.h"
#include "hasbeenhiddenlistener.h"
#include "swixcheck.h"
#include "tbxexcept.h"
#include "res/resfileinfo.h"

#include <swis.h>

namespace tbx {

FileInfo::FileInfo(const res::ResFileInfo &object_template) : ShowFullObject(object_template) {}

/**
 * Set the date
 */
void FileInfo::date(const UTCTime &date)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

   // Run Toolbox_ObjectMiscOp
    swix_check(_swix(0x44ec6, _INR(0,3), 0, _handle, 9, date.buffer()));
}

/**
 * Get the date
 */
UTCTime FileInfo::date() const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	UTCTime value;

    // Run Toolbox_ObjectMiscOp
    swix_check(_swix(0x44ec6, _INR(0,2) | _OUT(0), 0, _handle, 10, value.buffer()));

    return value;
}

/**
 * This event is raised just before the FileInfo underlying window is
 * about to be shown.
 */
void FileInfo::add_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	add_listener(0x82ac0, listener, about_to_be_shown_router);
}

/**
 * Remove about to be shown listener
 */
void FileInfo::remove_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	remove_listener(0x82ac0, listener);
}

/**
 * This event is raised after the FileInfo dialog has been completed
 */
void FileInfo::add_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	add_listener(0x82ac1, listener, has_been_hidden_router);
}

/**
 * Remove has been hidden listener
 */
void FileInfo::remove_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	remove_listener(0x82ac1, listener);
}

}
