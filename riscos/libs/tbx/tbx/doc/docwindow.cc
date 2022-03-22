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
 * DocWindow.cpp
 *
 *  Created on: 8 Oct 2010
 *      Author: alanb
 */

#include "docwindow.h"
#include "../saveas.h"

namespace tbx
{
namespace doc
{

/**
 * Create the main window for a given document using a resource template
 *
 * @param doc Document to create the window for
 * @param template_name name of Window template in resources to use
 */
DocWindow::	DocWindow(Document *doc, std::string template_name) :
   _window(template_name)
{
	_document = doc;
	_window.client_handle(doc);
	doc->add_modified_changed_listener(this);
	_window.title(doc->file_name());
	_window.add_close_window_listener(this);
}

DocWindow::~DocWindow()
{
	// Ensure underlying toolbox window is deleted as well
	_window.delete_object();

	// delete the document this window is showing
	delete _document;
}

/**
 * Close window request from window
 */
void DocWindow::close_window(const tbx::EventInfo &close_event)
{
	if (_document->modified())
	{
		tbx::DCS dcs("DCS");
		dcs.add_discard_listener(this);
		dcs.add_save_listener(this);
		dcs.add_cancel_listener(this);
		dcs.show_as_menu();
	} else
	{
		// Safe to delete
		delete this;
	}
}

/**
 * Discard selected from DCS prompt so just delete the window
 */
void DocWindow::dcs_discard(const tbx::EventInfo &info)
{
	tbx::DCS dcs = info.id_block().self_object();
	dcs.delete_object(); // No longer need the DCS
	delete this;
}

/**
 * Save selected from the DCS prompt so show save window
 */
void DocWindow::dcs_save(const tbx::EventInfo &info)
{
	tbx::DCS dcs = info.id_block().self_object();
	dcs.delete_object(); // No longer need the DCS

	// Save as is shared and already created automatically when
	// the document was created so the following line will get a
	// reference to the already created DocSaveAs
	tbx::SaveAs save_as("SaveAs");
	save_as.show_as_menu(_window);
}

/**
 * Cancel selected from the DCS prompt so just delete dcs object
 */
void DocWindow::dcs_cancel(const tbx::EventInfo &info)
{
	tbx::DCS dcs = info.id_block().self_object();
	dcs.delete_object(); // No longer need the DCS
}

/**
 * Get the document showed by a window
 */
Document *DocWindow::document(tbx::Window window)
{
	return reinterpret_cast<Document *>(window.client_handle());
}

/**
 * Document modified flag has changed
 */
void DocWindow::document_modified_changed(Document *doc)
{
	std::string title(doc->file_name());
	if (doc->modified()) title += " *";
	_window.title(title);
}

}
}
