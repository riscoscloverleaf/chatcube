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
 * DocWindow.h
 *
 *  Created on: 8 Oct 2010
 *      Author: alanb
 */

#ifndef TBX_DOCWINDOW_H_
#define TBX_DOCWINDOW_H_

#include "../window.h"
#include "../closewindowlistener.h"
#include "../dcs.h"

#include "document.h"

namespace tbx
{
namespace doc
{

/**
 * Base class for the document window.
 *
 * This provides standard document window processing;
 *   Shows a "Discard/Cancel/Quit" dialogue if the document is modified when
 *   trying to close it.
 *   Updates the title to the file name and puts a "*" after it when it is modified.
 *
 *   It requires a Window resource (the template name is given in the constructor)
 *   that is an ancestor object/automatically shows and has the auto close flag
 *   unset.
 *
 *   For the DCS processing it needs a DCS resource called "DCS"
 *   For saving from the DCS it needs a SaveAs resource called "SaveAs"
 */
class DocWindow :
	public DocModifiedChangedListener,
	public tbx::CloseWindowListener,
	public tbx::DCSDiscardListener,
	public tbx::DCSSaveListener,
	public tbx::DCSCancelListener
{
protected:
	tbx::Window _window; //!< Window showing document
	Document *_document; //!< Document that is being shown

public:
	DocWindow(Document *doc, std::string template_name);
	virtual ~DocWindow();

    virtual void close_window(const tbx::EventInfo &close_event);
    void dcs_discard(const tbx::EventInfo &info);
    void dcs_save(const tbx::EventInfo &info);
    void dcs_cancel(const tbx::EventInfo &info);

	/**
	 * Return pointer to the document interface.
	 */
	Document *document_interface() {return _document;}

	/**
	 * Return pointer to the document interface.
	 */
	Document *document_interface() const {return _document;}

	virtual void document_modified_changed(Document *doc);
	static Document *document(tbx::Window window);
};

}
}

#endif /* TBX_DOCWINDOW_H_ */
