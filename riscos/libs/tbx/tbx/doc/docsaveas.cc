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
 * DocSaveAs.cpp
 *
 *  Created on: 8 Oct 2010
 *      Author: alanb
 */

#include "../application.h"
#include "../res/ressaveas.h"

#include "docsaveas.h"
#include "docwindow.h"

namespace tbx
{
namespace doc
{
/**
 * Constructor. Sets up listener for when toolbox save as object is created
 *
 */
DocSaveAs::DocSaveAs()
{
	tbx::res::ResSaveAs res_save_as = tbx::app()->resource("SaveAs");
	_has_selection = !res_save_as.no_selection();
	tbx::app()->set_autocreate_listener("SaveAs", this);
}

DocSaveAs::~DocSaveAs()
{
}

/**
 * SaveAs has been created so attach listeners for saving documents.
 *
 * As the save as object is shared, this routine can be called multiple times.
 * However the old object would have been deleted before this happened which
 * automatically removes the old listeners.
 */
void DocSaveAs::auto_created(std::string template_name, tbx::Object object)
{
	tbx::SaveAs save_as(object);

	save_as.add_about_to_be_shown_listener(this);
	save_as.set_save_to_file_handler(this);
	save_as.add_save_completed_listener(this);
}

/**
 * Interrogate document for parameters for save and fill in save as dialog
 */
void DocSaveAs::about_to_be_shown(tbx::AboutToBeShownEvent &event)
{
	tbx::SaveAs saveas(event.id_block().self_object());
	Document *doc = DocWindow::document(event.id_block().ancestor_object());

	saveas.file_name(doc->file_name());
	saveas.file_type(doc->file_type());
	saveas.file_size(doc->document_size());
	if (_has_selection) saveas.selection_available(doc->has_selection());
}

/**
 * Save the document or selection to a file
 */
void DocSaveAs::saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string filename)
{
	Document *doc = DocWindow::document(saveas.ancestor_object());
	bool saved_ok = false;
	if (selection) saved_ok = doc->save_selection(filename);
	else saved_ok = doc->save(filename);

	saveas.file_save_completed(saved_ok, filename);
}

/**
 * Called when document has been successfully saved
 */
void DocSaveAs::saveas_save_completed(tbx::SaveAsSaveCompletedEvent &event)
{
	Document *doc = DocWindow::document(event.id_block().ancestor_object());
	if (event.selection_saved()) doc->save_selection_completed(event.file_name());
	else doc->save_completed(event.file_name(), event.safe());
}

}
}
