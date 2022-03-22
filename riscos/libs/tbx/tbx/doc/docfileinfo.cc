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
 * docfileinfo.cc
 *
 *  Created on: 13 Oct 2010
 *      Author: alanb
 */

#include "docfileinfo.h"
#include "document.h"
#include "docwindow.h"
#include "../application.h"
#include "../fileinfo.h"
#include "../path.h"

namespace tbx
{
namespace doc
{


/**
 * Constructor registers this to detect when resource "FileInfo" is created
 */
DocFileInfo::DocFileInfo()
{
	tbx::app()->set_autocreate_listener("FileInfo", this);
}

/**
 * FileInfo has been created so attach listeners for showing information
 * on the current document.
 *
 * As the file info object is shared, this routine can be called multiple times.
 * However the old object would have been deleted before this happens which
 * automatically removes the old listeners.
 */
void DocFileInfo::auto_created(std::string template_name, tbx::Object object)
{
	tbx::FileInfo file_info(object);

	file_info.add_about_to_be_shown_listener(this);
}

/**
 * Interrogate document for parameters for file info dialogue
 */
void DocFileInfo::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	tbx::FileInfo file_info(event.id_block().self_object());
	Document *doc = DocWindow::document(event.id_block().ancestor_object());

	file_info.file_name(doc->file_name());
	file_info.file_type(doc->file_type());
	file_info.file_size(doc->document_size());
	file_info.modified(doc->modified());

	tbx::Path path(doc->file_name());
	tbx::UTCTime date = path.modified_time();
	if (date.centiseconds() == 0) date = tbx::UTCTime::now();
	file_info.date(date);

}
}
}
