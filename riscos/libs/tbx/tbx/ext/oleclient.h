/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2014 Alan Buckley   All Rights Reserved.
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

#ifndef TBX_EXT_OLECLIENT_H
#define TBX_EXT_OLECLIENT_H

#include <string>
#include "../wimpmessagelistener.h"
#include "../window.h"

namespace tbx
{
namespace ext
{

class OleClientHandler;

/**
 * Class to provide an OleClient to edit a document
 * externally for an application
 *
 * An application that supports OLE editing for the given file
 * type needs to have been seen by the filer.
 *
 * The find_server method will also attempt to use the OLESupport
 * module (if available in !System) if no other OLE server is
 * registered for the file type.
 *
 * Pseudo code example of use
 * OleClient text_client(0xFFF); // 0xFFF is file type for text files
 *
 * if (text_client.find_server())
 * {
 *    std::string temp_file = text_client.get_scrap_file_name();
 *    // Save data to be edited to the file
 *    std::ofstream os(temp_file);
 *    os << text_to_be_edited;
 *    os.close();
 *
 *    text_client.edit_file(temp_name, from_window, 0, 0, my_handler)
 *    // see OleClientHandler for how to proceed
 *
 *    // A shortcut has been provided to just edit text in a string
 *    text_client.edit_text(text_to_be_edited, from_window, 0, 0, my_handler)
 * }
 */
class OleClient :
  private tbx::WimpAcknowledgeMessageListener,
  private tbx::WimpUserMessageListener
{
   int _file_type;
   std::string _tmp_prefix;
   OleClientHandler *_handler;
   int _session_id;
   static int _next_session_id;
   std::string _server_name;
   std::string _server_start;
   bool _text_edit;
   std::string _file_name;
   int _my_ref;
   int _editor_task;
   bool _in_progress;
   bool _delete_file;
   static int _open_count;

   public:
      OleClient(int file_type, const char *temp_prefix = 0);
      ~OleClient(); // TIDY up

      static void client_closed();
      /**
       * Return the number of currently open OLE sessions
       */
      static int open_count() {return _open_count;}

      bool find_server(bool use_ole_support = true);
      std::string get_scrap_file_name();

      void edit_file(std::string file_name, tbx::Window file_window, int x_offset, int y_offset, OleClientHandler *handler);
      void edit_text(std::string text, tbx::Window file_window, int x_offset, int y_offset, OleClientHandler *handler);

      void closed_in_client();

      /**
       * Check if file should be deleted
       *
       * @returns true if file used to pass data to OLE server
       * should be deleted when editing session is closed
       */
      bool delete_file() const {return _delete_file;}
      /**
       * Set if file used to pass data to OLE server
       * should be deleted when editing session is closed.
       *
       * This is normally false, but is set to true by the edit_text
       * method.
       *
       * @param del true if file should be deleted when closed
       */
      void delete_file(bool del) {_delete_file = del;}

      /**
       * Check if OLE edit is in progress
       *
       * @returns true if the edit is in progress
       */
      bool in_progress() const {return _in_progress;}

      virtual void acknowledge_message(tbx::WimpMessageEvent &event);
	  virtual void user_message(tbx::WimpMessageEvent &event);

   private:
	  void edit_finished();
};

/**
 * Interface called back from OleServer to inform of state of
 * edited file.
 */
class OleClientHandler
{
public:
	virtual ~OleClientHandler() {};

	/**
	 * Called if attempt to start the OLE server failed
	 *
	 * @param client OleClient object handling this edit
	 */
	virtual void failed_to_start_server(OleClient &client) = 0;
	/**
	 * Called once the OLE edit has successfully started
	 *
	 * @param client OleClient object handling this edit
	 */
    virtual void edit_started(OleClient &client) {}
    /**
     * Called when the OLE editor has been closed
	 *
	 * @param client OleClient object handling this edit
     */
    virtual void edit_closed(OleClient &client) {}
    /**
     * Called with the new text from the OLE server if the the edit was
     * started using the edit_text method.
	 *
	 * @param client OleClient object handling this edit
	 * @param text The new text of the edit
     */
    virtual void edit_text_changed(OleClient &client, const std::string &text) {}
    /**
     * Called when the file has been saved in the OLE server (when edit_file was
     * called).
	 *
	 * @param client OleClient object handling this edit
     * @param file_name the name of the file saved (may be different from file name from edit_file)
     */
    virtual void edit_file_changed(OleClient &client, const std::string &file_name) {}
};

}
}

#endif
