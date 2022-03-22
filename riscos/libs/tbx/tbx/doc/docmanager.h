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
 * DocManager.h
 *
 *  Created on: 8 Oct 2010
 *      Author: alanb
 */

#ifndef TBX_DOCMANAGER_H_
#define TBX_DOCMANAGER_H_

#include <list>
#include "../prequitlistener.h"
#include "../quit.h"
#include "../command.h"
#include "../loader.h"

namespace tbx
{
namespace doc
{

class Document;

/**
 * Base class to help create a document and a window to show it in
 */
class DocCreatorBase
{
	int _file_type;
public:
	/**
	 * Construct a document and window creator for a file type
	 *
	 * @param file_type file type for this type of document
	 */
	DocCreatorBase(int file_type) : _file_type(file_type) {}
	virtual ~DocCreatorBase() {}

	/**
	 * Return the file type handled by this document creator
	 */
	int file_type() const {return _file_type;}

	/**
	 * Override this method to create the document object
	 *
	 * @returns Document derived object to handle the document data
	 */
	virtual Document *create_document() = 0;
	/**
	 * Create the window used to display the given document
	 *
	 * @param doc document to create the main window for
	 */
	virtual void create_window(Document *doc) = 0;

};

/**
 * Templated class to provide information on a documents file type
 *  and how to create the document and its window.
 */
template<class T, class W> class DocCreator : public DocCreatorBase
{
public:
	/**
	 * Construct document creator for a file type
	 *
	 * @param file_type file type of documents created by this object
	 */
	DocCreator(int file_type) : DocCreatorBase(file_type) {}
	virtual ~DocCreator() {}
	virtual Document *create_document() {return new T();}
	virtual void create_window(Document *doc) {new W((T *)doc);}
};


/**
 * Class to manage all the documents.
 *
 * It keeps track of the documents created and process the
 * application pre-quit message to prompt the user if an
 * attempt to quit the application is made when there are unsaved
 * documents.
 *
 * It also provides commands to implement:
 *   Quit from the menu
 *   Creating a new document
 * A loader to load a file to a new document,
 * and traps the data open WIMP message to open a document.
 *
 * The document manager requires a Quit resource called "Quit"
 * that it will use if multiple documents are open when
 * the application is quit.
 *
 * If the message in the Quit resource contains "%0" this will
 * be replaced by the number of unsaved documents.
 *
 * It uses the following message tokens if defined in the Messages file;
 * TbxNewDocFailed - new document creation failed
 * TbxLoadDocFailed - Loading of a document failed.
 * In both cases the %0 parameter is replace by the exception message.
 */
class DocManager :
	public tbx::PreQuitListener
{
private:
	static DocManager *_instance;
	DocCreatorBase *_doc_creator;
	std::list<Document *> _documents;

	/**
	 * Command to create a new documnet
	 */
	class NewDocumentCommand : public tbx::Command
	{
	public:
		virtual ~NewDocumentCommand() {}
		virtual void execute();
	} _new_doc_command;

	/**
	 * Command to quit the application
	 */
	class QuitCommand : public tbx::Command
	{
	public:
		virtual ~QuitCommand() {};
		virtual void execute();
	} _quit_command;

	/**
	 * Class to load a file
	 */
	class FileLoader : public tbx::Loader
	{
	public:
		virtual ~FileLoader() {}
		bool load_file(tbx::LoadEvent &event);
	} _file_loader;

	void add_document(Document *doc);
	void remove_document(Document *doc);
	friend class Document;

public:
	DocManager(DocCreatorBase *doc_creator);
	virtual ~DocManager();

	/**
	 * Get single instance of the document manager
	 */
	static DocManager *instance() {return _instance;}
	/**
	 * Get the object that creates documents and their main window
	 */
	DocCreatorBase *doc_creator() {return _doc_creator;}

	unsigned int modified_count() const;

	/**
	 * Return a command that can be executed to create a new
	 * document with its window
	 */
	tbx::Command *new_document_command() {return &_new_doc_command;}
	/**
	 * Return a command that can be executed to quit the application
	 * showing a Quit dialogue if necessary.
	 */
	tbx::Command *quit_command() {return &_quit_command;}

	/**
	 * Return a loader to load a document into a new window
	 */
	tbx::Loader *file_loader() {return &_file_loader;}

	/**
	 * Get the document file type
	 */
	int file_type() const {return _doc_creator->file_type();}

	bool load_files(int num_files, char *file_names[]);
	bool load_file(const std::string &file_name, int estimated_size = -1, bool from_filer = true);

	virtual void pre_quit(tbx::PreQuitEvent &event);

private:
	/**
	 * Internal class to deal with quit prompt
	 */
	class QuitPrompt :
		public tbx::QuitQuitListener
	{
		tbx::Quit _quit;
		tbx::QuitRestarter _restart;
	public:
		QuitPrompt(const tbx::QuitRestarter restart, int num_modified);
		~QuitPrompt();
		virtual void quit_quit(const tbx::EventInfo &info);
	};


};

}
}

#endif /* TBX_DOCMANAGER_H_ */
