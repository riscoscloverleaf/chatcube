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
 * DocManager.cpp
 *
 *  Created on: 8 Oct 2010
 *      Author: alanb
 */

#include "docmanager.h"
#include "document.h"
#include "../application.h"
#include "../deleteonhidden.h"
#include "../stringutils.h"
#include "../reporterror.h"
#include "../message.h"

#include <algorithm>

namespace tbx
{
namespace doc
{

DocManager *DocManager::_instance = 0;

/**
 * Construct the document manager for the application.
 *
 * @param doc_creator - class to help in creating documents and their windows
 */
DocManager::DocManager(DocCreatorBase *doc_creator)
{
   _instance = this;
   _doc_creator = doc_creator;
   tbx::app()->add_prequit_listener(this);
}

DocManager::~DocManager()
{
  _instance = 0;
}

/**
 * Add document to list of documents.
 *
 * Automatically called by Document constructor
 */
void DocManager::add_document(Document *doc)
{
	_documents.push_back(doc);
}

/**
 * Remove document from list of documents
 *
 * Automatically called by Document destructor
 */
void DocManager::remove_document(Document *doc)
{
	std::list<Document *>::iterator i = std::find(_documents.begin(), _documents.end(), doc);
	if (i != _documents.end()) _documents.erase(i);
}

/**
 * Return number of documents modified
 */
unsigned int DocManager::modified_count() const
{
	unsigned int modified = 0;
	for (std::list<Document *>::const_iterator i = _documents.begin(); i != _documents.end(); ++i)
	{
		if ((*i)->modified()) modified++;
	}
	return modified;
}

/**
 * Create a new document and it's assosiated window
 */
void DocManager::NewDocumentCommand::execute()
{
	DocCreatorBase *doc_creator = DocManager::instance()->doc_creator();
	if (doc_creator)
	{
		Document *doc = 0;
		try
		{
			doc = doc_creator->create_document();
			doc->new_document();
			doc_creator->create_window(doc);
		} catch (std::exception &e)
		{
			tbx::report_error(
				tbx::message("TbxNewDocFailed:Unable to create document %0", e.what())
			);
			delete doc;
		}
	}
}


/**
 * Called when the prequit message is received by the application.
 *
 * Check if we have any modified packages and if so cancel the
 * quit and show the quit dialog box.
 */
void DocManager::pre_quit(tbx::PreQuitEvent &event)
{
	unsigned int modified = modified_count();
	if (modified)
	{
		event.cancel_quit();
		new QuitPrompt(event.quit_restarter(), modified);
	}
}

/**
 * Code to execute the DocManager quit command
 */
void DocManager::QuitCommand::execute()
{
	unsigned int modified = DocManager::instance()->modified_count();
	tbx::QuitRestarter restart;

	restart.close_application_only(true);

	if (modified)
	{
		new QuitPrompt(restart, modified);
	} else
	{
		// In this case we need to do the quit ourselves
		restart.restart_quit();
	}
}

/**
 * Class to hand the quit prompt when system or application is closing
 */
DocManager::QuitPrompt::QuitPrompt(const tbx::QuitRestarter restart, int num_modified) :
		_quit("Quit"), _restart(restart)
{
	std::string msg(_quit.message());
	if (!msg.empty())
	{
		std::string::size_type num_pos = msg.find("%0");
		if (num_pos != std::string::npos)
		{
			msg.replace(num_pos,2, tbx::to_string(num_modified));
			_quit.message(msg);
		}
	}
	_quit.add_quit_listener(this);
	_quit.add_has_been_hidden_listener(new tbx::DeleteClassOnHidden<QuitPrompt>(this));
	_quit.show_as_menu();
}

/**
 * Delete toolbox object when this is deleted
 */
DocManager::QuitPrompt::~QuitPrompt()
{
	_quit.delete_object();
}

/**
 * Quit is chosen so exit the application
 */
void DocManager::QuitPrompt::quit_quit(const tbx::EventInfo &info)
{
	_restart.restart_quit();
}

/**
* Override to do the actual file load.
*
* The file name for the load is in event.file_name().
*
* @return true if file is loaded.
*/
bool DocManager::FileLoader::load_file(tbx::LoadEvent &event)
{
	return DocManager::instance()->load_file(event.file_name(), event.estimated_size(), event.from_filer());
}

/**
 * Load multiple files into documents and show windows for them
 *
 * Display's an error for the first file it can not load and
 * stops loading following files
 *
 * @param num_files number of files in file_names array.
 * @param file_names names of files to load
 * @returns true if loaded successfully
 */
bool DocManager::load_files(int num_files, char *file_names[])
{
	for (int j = 0; j < num_files; j++)
	{
		if (!load_file(file_names[j])) return false;
	}

	return true;
}

/**
 * Load a file into a document and show it in a window
 *
 * Display's an error if the file can not be loaded
 *
 * @param file_name name of file to load
 * @param estimated_size - estimated size of file if given
 * @param from_filer true if document was loaded from the filer (safe storage)
 * @returns true if loaded successfully
 */
bool DocManager::load_file(const std::string &file_name, int estimated_size /* = -1*/, bool from_filer /* = true */)
{
	if (!_doc_creator) return false;
	bool loaded = false;

	Document *doc = 0;

	try
	{
		doc = _doc_creator->create_document();
		loaded = doc->load(file_name, estimated_size);
		if (loaded)
		{
			doc->load_completed(file_name, from_filer);
			_doc_creator->create_window(doc);
		}
	} catch (std::exception &e)
	{
		tbx::report_error(tbx::message("TbxLoadDocFailed:Unable to load document %0", e.what()));
	}
	if (!loaded) delete doc;

	return loaded;
}

}
}
