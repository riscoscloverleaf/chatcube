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
 * Document.cpp
 *
 *  Created on: 8 Oct 2010
 *      Author: alanb
 */

#include "document.h"
#include "docmanager.h"

#include "../stringutils.h"
#include "../path.h"

#include <fstream>

namespace tbx
{
namespace doc
{
// Next untitled number
unsigned int Document::_untitled = 0;

/**
 * Default constructor.
 * Adds document to the document manager
 */
Document::Document()
{
	_modified = false;
	DocManager::instance()->add_document(this);
}

/**
 * Default destructor.
 * Removes document from the document manager
 */
Document::~Document()
{
	DocManager::instance()->remove_document(this);
}

/**
 * Called after a new document has been created and before it
 * is attached to a window.
 *
 * The default sets the file_name to "Untitled" with a count
 */
void Document::new_document()
{
	_file_name = "Untitled" + tbx::to_string(++_untitled);
}

/**
 * Add a listener to detect when a document goes to and from its modified
 * state.
 */
void Document::add_modified_changed_listener(DocModifiedChangedListener *listener)
{
	_modified_listeners.push_back(listener);
}

/**
 * Remove a listener to detect when a document goes to and from its modified
 * state.
 */
void Document::remove_modified_changed_listener(DocModifiedChangedListener *listener)
{
	_modified_listeners.remove(listener);
}

/**
 * Set the document modified flag
 *
 * If it has changed from it's old state the DocModifiedChangedListeners are
 * fired.
 */
void Document::modified(bool changed)
{
	if (_modified != changed)
	{
		_modified = changed;
		tbx::SafeList<DocModifiedChangedListener>::Iterator iter(_modified_listeners);
		DocModifiedChangedListener *listener;
		while ((listener = iter.next()) != 0)
		{
		     listener->document_modified_changed(this);
		}
	}
}

/**
 * Called to save document to the given file.
 *
 * This version opens a binary output stream on the file
 * and calls save(std::ostream&).
 */
bool Document::save(std::string file_name)
{
	std::ofstream os(file_name.c_str(), std::ios_base::binary);
	if (!os) return false;
	return save(os);
}

/**
 * This is called when a save is successfully completed.
 *
 * The default sets the file type, updates the file name and sends a
 * modified event so listeners can update their titles.
 *
 * @param file_name file name used for the save
 * @param safe true if save was to a "safe" location (e.g. the filer)
 */
void Document::save_completed(std::string file_name, bool safe)
{
	if (safe)
	{
		// Set file type
		tbx::Path pth(file_name);
		pth.file_type(file_type());

		bool fname_changed = (_file_name != file_name);
		_file_name = file_name;
		if (_modified) modified(false);
		else if (fname_changed)
		{
			// Send a modified event even though the modified has not changed
			// so the event can be used to update titles.
			tbx::SafeList<DocModifiedChangedListener>::Iterator iter(_modified_listeners);
			DocModifiedChangedListener *listener;
			while ((listener = iter.next()) != 0)
			{
				 listener->document_modified_changed(this);
			}
		}
	}
}

/**
 * Load document from given file name.
 *
 * This version opens a binary input stream on the file name
 * and passes the loading onto load(std::istream*, int)
 * If estimated_size == -1 it also calculates the length of the file
 * before passing it on.
 *
 * @param file_name name of file to load
 * @param estimated_size - estimated size of file or -1 if not specified
 */
bool Document::load(std::string file_name, int estimated_size /*= -1*/)
{
	std::ifstream is(file_name.c_str(), std::ios::binary);
	if (!is) return false;
	if (estimated_size == -1)
	{
		is.seekg (0, std::ios::end);
		estimated_size = is.tellg();
		is.seekg (0, std::ios::beg);
		if (!is) return false;
	}

	return load(is, estimated_size);
}

/**
 * This is called after a document has successfully been loaded.
 * It is called before the window is shown.
 *
 * If you override this method you should call this base version
 * to update the file name.
 */
void Document::load_completed(std::string file_name, bool from_filer)
{
	if (from_filer) _file_name = file_name;
}

/**
 * Save selection.
 *
 * This version opens a binary output stream and
 * calls save_selection(std::ostream&)
 *
 * @param file_name file_name to save to
 * @return true if save successful
 */
bool Document::save_selection(std::string file_name)
{
	std::ofstream os(file_name.c_str(), std::ios::binary);
	if (!os) return false;
	return save_selection(os);
}


}
}
