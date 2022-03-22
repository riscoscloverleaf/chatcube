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
 * Document.h
 *
 *  Created on: 8 Oct 2010
 *      Author: alanb
 */

#ifndef TBX_DOCUMENT_H_
#define TBX_DOCUMENT_H_

#include "../safelist.h"
#include "../listener.h"
#include <string>
#include <iostream>

namespace tbx
{
namespace doc
{
class DocModifiedChangedListener;

/**
 * Base class for a document.
 *
 * Automatically adds and removes documents from the document
 * manager and provides the interfaces required for the
 * Document manager and other doc classes.
 */
class Document
{
private:
	std::string _file_name;
	bool _modified;
	tbx::SafeList<DocModifiedChangedListener> _modified_listeners;
	static unsigned int _untitled;

public:
	Document();
	virtual ~Document();

	virtual void new_document();

	void add_modified_changed_listener(DocModifiedChangedListener *listener);
	void remove_modified_changed_listener(DocModifiedChangedListener *listener);

	/**
	 * Return true if document has been modifed since the last save
	 */
	bool modified() const {return _modified;}

	/**
	 * Return the name of the document file
	 */
	const std::string &file_name() const {return _file_name;}

	/**
	 * Return the file type for this document
	 */
	virtual int file_type() const = 0;

	/**
	 * Return the size of the document.
	 *
	 * If the size can not be accurately calculated an
	 * estimate should be returned.
	 */
	virtual int document_size() const = 0;

	/**
	 * Check if the document has a selection
	 */
	virtual bool has_selection() const {return false;}

	virtual bool save(std::string file_name);

	/**
	 * Save document to the given stream.
	 */
	virtual bool save(std::ostream &os) = 0;
	virtual void save_completed(std::string file_name, bool safe);

	/**
	 * Save selection.
	 * @param file_name file name to save to
	 * @return true if save successful
	 */
	 virtual bool save_selection(std::string file_name);
	 /**
	  * Save selection to the given output stream
	  * Default does nothing
	  */
	 virtual bool save_selection(std::ostream &os) {return false;}

	 /**
	  * Called when selection save has finished
	  *
	  * Base does nothing
	  */
	 virtual void save_selection_completed(std::string file_name) {}

	 virtual bool load(std::string file_name, int estimated_size = -1);
	 /**
	  * Load document from the given input stream.
	  *
	  * @param is input stream to load from
	  * @param estimated_size estimated size of data or -1 if estimate not given
	  */
	 virtual bool load(std::istream &is, int estimated_size) = 0;
	 virtual void load_completed(std::string file_name, bool from_filer);


protected:
	void modified(bool changed);
};


/**
 * Listener for document modified state changed
 */
class DocModifiedChangedListener : public tbx::Listener
{
public:
	virtual ~DocModifiedChangedListener() {};

	/**
	 * This routine is called when the documents modified flag
	 * changes.
	 */
	virtual void document_modified_changed(Document *doc) = 0;
};


}
}

#endif /* TBX_DOCUMENT_H_ */
