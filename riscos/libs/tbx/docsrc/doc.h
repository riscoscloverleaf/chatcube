/**
 * @namespace doc Helper classes to create a document based application
 *
 * The doc namespace provides helper classes to make it easy to produce
 * a document editing application.
 *
 * Using these classes means you can concentrate on application functionality
 * as they handle all the plumbing for you.
 *
 * e.g.
 *  - Creating a new document on clicking on the iconbar
 *  - Creating a window to show it in.
 *  - Updating the title of the window with the filename and a "*" if it is modified.
 *  - Loading a document if dropped on the iconbar.
 *  - Opening a document if it is double clicked in the filer.
 *  - Showing the Discard/Cancel/Save dialogue when trying to close a window with
 *     modified content.
 *  - Showing the Quit dialogue if you try to quit the application with unsaved
 *   documents
 *  - Saving and loading just becomes a matter of overriding the save and load
 *   methods in your class derived from tbx::doc::Document.
 *
 * The main entry point to it then becomes
 * @code
 * 	tbx::Application docex_app("<TbxDocEx$Dir>");
 *	tbx::doc::DocCreator<TbxExDoc, TbxExWnd> doc_creator(0x010);
 *	tbx::doc::DocManager doc_manager(&doc_creator);
 *	tbx::doc::DocIconbar doc_iconbar;
 *	tbx::doc::DocSaveAs doc_saveas;
 *	tbx::doc::DocFileInfo doc_fileinfo;
 *
 *	if (argc > 1) doc_manager.load_files(argc-1, argv+1);
 *
 *	docex_app.run();
 * @endcode
 *
 * All that is then needed is to derive your document from tbx::doc::Document
 * and overload the document_size, load(std::istream &is) and save(std::ostream &os)
 * functions.
 *
 * The window to display the document should be derived from tbx::doc::DocWindow
 * and modified to show the data when it is created.
 *
 * If more functionality is required from the iconbar you may want to derive
 * a class from DocIconbar as well.
 *
 * For this functionality to work you need to have the following resources.
 * - An Iconbar named "Iconbar"
 * - A main Window (the name of it is passed to the DocWindow constructor)
 * - A SaveAs dialogue called "SaveAs"
 * - A DCS dialogue called "DCS"
 * - A Quit dialogue called "Quit"
 * - A FileInfo dialogue called "FileInfo"
 *
 * Also you will want to create a icon bar menu with the Quit option on and a
 * menu for the main window that has entries to show SaveAs and FileInfo.
 *
 * Set the Doc... class descriptions for other settings needed for the resources.
 */

