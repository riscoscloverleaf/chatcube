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
 * DocSaveAs.h
 *
 *  Created on: 8 Oct 2010
 *      Author: alanb
 */

#ifndef TBX_DOCSAVEAS_H_
#define TBX_DOCSAVEAS_H_

#include "../autocreatelistener.h"
#include "../abouttobeshownlistener.h"
#include "../saveas.h"

namespace tbx
{
namespace doc
{

/**
 * Class to use save as dialogue box to save the document.
 *
 * Requires a SaveAs resource called "SaveAs" that has the follow flags:
 *  shared
 *  delivers event before shown
 *  client participates in transfer
 *  RAM transfer must not be set
 *
 * TODO: Currently only supports save to file, should support ram transmit as well
 */
class DocSaveAs :
	public tbx::AutoCreateListener,
	public tbx::AboutToBeShownListener,
	public tbx::SaveAsSaveToFileHandler,
	public tbx::SaveAsSaveCompletedListener
{
private:
	bool _has_selection;

public:
	DocSaveAs();
	virtual ~DocSaveAs();

	virtual void auto_created(std::string template_name, tbx::Object object);

	virtual void about_to_be_shown(tbx::AboutToBeShownEvent &event);
	virtual void saveas_save_to_file(tbx::SaveAs saveas, bool selection, std::string filename);
	virtual void saveas_save_completed(tbx::SaveAsSaveCompletedEvent &event);
};

}
}

#endif /* TBX_DOCSAVEAS_H_ */
