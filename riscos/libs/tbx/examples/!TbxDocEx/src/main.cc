/*
 * main.cc
 */

#include "tbx/application.h"
#include "tbx/doc/docmanager.h"
#include "tbx/doc/dociconbar.h"
#include "tbx/doc/docsaveas.h"
#include "tbx/doc/docfileinfo.h"

#include "tbxexdoc.h"
#include "tbxexwnd.h"

/**
 * Main entry point to the application.
 *
 * Use the classes from the tbx::doc namespace to
 * provide all the plumbing for a document type
 * application.
 */
int main(int argc, char *argv[])
{
	tbx::Application docex_app("<TbxDocEx$Dir>");
	// DocCreator required to let the the document manager know
	// how to create a document and its window.
	tbx::doc::DocCreator<TbxExDoc, TbxExWnd> doc_creator(0x010);

	// Document manager that provides commands and loaders to
	// create new and load documents.
	// It also handles quit processing by shown the Quit dialogue
	// if necessary. (i.e. there are some modified documents).
	tbx::doc::DocManager doc_manager(&doc_creator);

	// Standard icon bar that provides click for new document
	// and load document when a file is dragged to it.
	tbx::doc::DocIconbar doc_iconbar;

	// Standard save as dialogue. It identifies the document it
	// is over and interrogates it for the parameters it needs.
	// If also calls the save methods on the document to do the
	// saving.
	tbx::doc::DocSaveAs doc_saveas;

	// Standard file info dialog. It get's all the information
	// it needs from methods on the document.
	tbx::doc::DocFileInfo doc_fileinfo;

	// Load any files from the command line
	if (argc > 1) doc_manager.load_files(argc-1, argv+1);

	// Run the application
	docex_app.run();

	return 0;
}
