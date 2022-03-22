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

#include "oleclient.h"

#include "../application.h"
#include "../path.h"

#include "swis.h"
#include <stdexcept>
#include <cstdlib>
#include <cstdio>

namespace tbx
{
namespace ext
{
// Next unique id for client session
int OleClient::_next_session_id = 1;
// Count of all the open sessions
int OleClient::_open_count = 0;

/**
 * Construct OleClient for a file type
 *
 * @param file_type Type of file to use client
 * @param temp_prefix prefix used for temporary file used in edit
 * (or 0 for application directory name)
 */
OleClient::OleClient(int file_type, const char *temp_prefix /*= 0*/) :
  _file_type(file_type),
  _handler(0),
  _session_id(0),
  _text_edit(false),
  _my_ref(0),
  _editor_task(0),
  _in_progress(false),
  _delete_file(false)
{
	if (temp_prefix) _tmp_prefix = temp_prefix;
	else
	{
		// Use location of !Run file to find path directory
		tbx::Path app_path(tbx::app()->directory() + "!Run");
		try
		{
			app_path.canonicalise();
			app_path.up();
			_tmp_prefix = app_path.leaf_name();
			if (!_tmp_prefix.empty() && _tmp_prefix[0] == '!') _tmp_prefix.erase(0,1);
		} catch(...)
		{
			// Ignore exceptions - will fall back to default path
		}
		if (_tmp_prefix.empty()) _tmp_prefix = "OleTbx";
	}
}

OleClient::~OleClient()
{
	edit_finished();
}

/**
 * Find the server for this OleClient
 *
 * Should be called before each edit as the server may change during
 * the lifetime of the application.
 *
 * @param use_ole_support use OLESupport module if no registered server
 */
bool OleClient::find_server(bool use_ole_support /*= true*/)
{
	char server_var[24];
	std::sprintf(server_var, "OLEServer$Type_%X", _file_type);
	char *server_val = getenv(server_var);

	_server_name.clear();

	if (server_val == 0 && use_ole_support)
	{
		// Start OLE support module to simulate ole with a non-ole editor
		int major, minor, task;
		// use OLE_Version to check if OLE support is loaded
		bool ole_support_started = false;
		if (_swix(0x67b00, _OUTR(0,2), &major, &minor, &task) == 0)
		{
			ole_support_started = true;
		} else
		{
			try
			{
				task = tbx::app()->start_wimp_task("System:Modules.OLESupport");
				if (task != 0) ole_support_started = true;
			} catch(...)
			{
				// do nothing - ole support hasn't started
			}
		}
		if (ole_support_started)
		{
			// use OLE_SimulateSession to set up a server variable
			if (_swix(0x47B05,_IN(0), _file_type) == 0)
			{
				server_val = getenv(server_var);
			}
		}
	}

	if (server_val)
	{
		while (*server_val)
		{
			while (*server_val == ' ') server_val++;
			if (*server_val == '-')
			{
			   char type = *(++server_val);
			   if (type)
			   {
				  server_val++;
				  while (*server_val == ' ') server_val++;
				  std::string arg;
				  if (type == 'N')
				  {
					  while (*server_val && *server_val != ' ')
					  {
						 arg += *server_val++;
					  }
					  _server_name = arg;
				  }
				  else if (type == 'R')
				  {
					  // -R may include spaces
					  while (*server_val) arg += *server_val++;
					  _server_start = arg;
				  }
			   }
			}
		}
	}

	return !_server_name.empty();
}

/**
* Get file name suitable for scrap file for edit session
*
* A new file name is returned with every call.
*
* @returns temporary file name in Wimp scrap directory
*/
std::string OleClient::get_scrap_file_name()
{
	return tbx::Path::temporary(_tmp_prefix.c_str());
}

/**
 * Edit the given text using OLE
 *
 * The text is written to a temporary file (which will be deleted at the
 * end of the session).
 *
 * @param text the text to edit
 * @param file_window the window the text is from
 * @param x_offset the x-coordinate of the text in the window (or 0 if none)
 * @param y_offset the y-coordinate of the text in the window (or 0 if none)
 * @param handler OleClientHandler to be informed of progress and updates to the text
 */
void OleClient::edit_text(std::string text, tbx::Window file_window, int x_offset, int y_offset, OleClientHandler *handler)
{
	std::string fname=get_scrap_file_name();
	tbx::Path f(fname);
	f.save_file(text.c_str(), text.size(), _file_type);
	_text_edit = true;
	_delete_file = true;
	edit_file(fname, file_window, x_offset, y_offset, handler);
}

/**
 * Edit the given file using OLE
 *
 * If editing a part of the document a suitable file name can be obtained
 * from the get_scrap_file_name method. If the file is temporary, set
 * delete_file(true) if you want the client to delete the file after use.
 *
 * @param file_name the name of the file containing the value to edit
 * @param file_window the window the text is from
 * @param x_offset the x-coordinate of the text in the window (or 0 if none)
 * @param y_offset the y-coordinate of the text in the window (or 0 if none)
 * @param handler OleClientHandler to be informed of progress and updates to the text
 */
void OleClient::edit_file(std::string file_name, tbx::Window file_window, int x_offset, int y_offset, OleClientHandler *handler)
{
	if (_server_name.empty()) throw std::logic_error("Must call find_server to locate a server before edit_file");
	if (_in_progress) throw std::logic_error("OLE edit already in progress");

	_handler = handler;
	_file_name = file_name;

	tbx::WimpMessage open_session(0x80E21, // Message_OLEOpenSession
			15 + ((file_name.length() + 4) >> 2) // Calculate size
			);

	// Make sure following is zero padded
	for (int word = 5; word <= 8; word++) open_session[word] = 0;
	_server_name.copy(open_session.str(5), 16);
	open_session[9] = file_window.window_handle();
	open_session[10] = x_offset;
	open_session[11] = y_offset;
	open_session[12] = 0; // Format number
	_session_id = _next_session_id++;
	if (_next_session_id > 0xFFFFFF) _next_session_id = 1; // Maximum 24 bits for id
	open_session[13] = _session_id;
	open_session[14] = _file_type;
	int len = file_name.copy(open_session.str(15), 195);
	open_session.str(15)[len] = 0;

	tbx::app()->add_acknowledge_message_listener(0x80E21, this);
	tbx::app()->add_user_message_listener(0x80E22, this);
	tbx::app()->add_user_message_listener(0x80e1e, this);
	tbx::app()->add_user_message_listener(0x80e23, this);

	_in_progress = true;
	open_session.send(tbx::WimpMessage::Recorded, tbx::WimpMessage::Broadcast);
	_my_ref = open_session.my_ref();
}

/**
 * Handle OLE acknowledge message
 */
void OleClient::acknowledge_message(tbx::WimpMessageEvent &event)
{
	// Received acknowledge so server didn't start
	if (event.message().my_ref() == _my_ref && event.message()[13] == _session_id)
	{
		// Server did not start
		bool cant_start = true;
		if (event.message()[12] == 0)
		{
			// Format 0 - first attempt so try to start server and retry with format 1
			try
			{
				_editor_task = tbx::app()->start_wimp_task(_server_start);
			} catch(...)
			{
				_editor_task = 0;
			}
			if (_editor_task)
			{
				tbx::WimpMessage second_go(event.message());
				second_go[12] = 1; // Retrying
				second_go.send(tbx::WimpMessage::Recorded, _editor_task);
				_my_ref = second_go.my_ref();
				cant_start = false;
			}
		}
		if (cant_start)
		{
			edit_finished();
			_handler->failed_to_start_server(*this);
			// Note: this may be deleted after the above call
		}
	}
}

/**
 * Handle messages from the OLE server
 */
void OleClient::user_message(tbx::WimpMessageEvent &event)
{
	const tbx::WimpMessage &msg = event.message();
	switch(msg.message_id())
	{
	case 0x80e22: // Message_OLEOpenSessionAck
		if (msg.your_ref() == _my_ref && msg[13] == _session_id)
		{
			_editor_task = msg[1];
			_handler->edit_started(*this);
			_open_count++;
			event.claim();
		}
		break;

	case 0x80e1e: // Message_OLEFileChanged
		if (msg[6] == _session_id)
		{
			std::string file_name = _file_name;
			if (msg[5] == 0) file_name = msg.str(7);

			if (_text_edit)
			{
				tbx::Path file(file_name);
				int len;
				char *data = file.load_file(&len);
				std::string text(data, len);
				delete [] data;
				_handler->edit_text_changed(*this, text);
			} else
			{
				_handler->edit_file_changed(*this, file_name);
			}
			event.claim();
		}
		break;

	case 0x80e23: // Message_OLECloseSession
		if (msg[6] == _session_id || msg[6] == -1)
		{
			edit_finished();
			_open_count--;
			_handler->edit_closed(*this);
			// Note: this may be deleted in edit_closed
		}
		break;
	}
}

/**
 * Called to close OLE editing from the client
 */
void OleClient::closed_in_client()
{
	if (_in_progress)
	{
		tbx::WimpMessage close_session(0x80E23, 7); // Message_OLECloseSession
		close_session[5] = 0; // format number
		close_session[6] = _session_id;
		close_session.send(tbx::WimpMessage::User, _editor_task);
		_open_count--;
		edit_finished();
	}
}

/**
 * Remove the OLE message listeners and delete temp file
 */
void OleClient::edit_finished()
{
	if (_in_progress)
	{
		tbx::app()->remove_acknowledge_message_listener(0x80E21, this);
		tbx::app()->remove_user_message_listener(0x80E22, this);
		tbx::app()->remove_user_message_listener(0x80e1e, this);
		tbx::app()->remove_user_message_listener(0x80e23, this);
		_in_progress = false;
		if (_delete_file && !_file_name.empty())
		{
			try
			{
				tbx::Path(_file_name).remove();
			} catch(...)
			{
				// Nothing we can do if it can't be deleted
				// It may have been deleted by the server when using
				// the OLE support module
			}
		}
	}
}

/**
 * Client application has closed so broadcast a message
 * to close any open OLE sessions in the third party
 * applications.
 *
 * Does nothing if no sessions are open
 */
void OleClient::client_closed()
{
	if (_open_count > 0)
	{
		tbx::WimpMessage close_session(0x80E23, 7); // Message_OLECloseSession
		close_session[5] = 0; // format number
		close_session[6] = -1;
		close_session.send(tbx::WimpMessage::User, tbx::WimpMessage::Broadcast);
		_open_count = 0;
	}
}

}
}
