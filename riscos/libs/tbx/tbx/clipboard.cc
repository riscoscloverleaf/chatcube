/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2021 Alan Buckley   All Rights Reserved.
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

#include "clipboard.h"
#include "application.h"
#include "loadermanager.h"

#include <fstream>
#include <cstring>

namespace tbx
{

// Wimp messages used by the clipboard	
//! @cond INTERNAL
const int Message_DataSave = 1;
const int Message_ClaimEntity = 15;
const int Message_DataRequest = 16;
//! @endcond INTERNAL

Clipboard *Clipboard::s_instance = 0;

Clipboard::Clipboard() 
	: _owns_clipboard(false),
	  _owns_caret(false),
	  _native_file_type(-1),
	  _format_listeners(0)
{
	s_instance = this;
}	

Clipboard::~Clipboard()
{
	clear();
	s_instance = 0;
}

/**
 * Return the global clipboard instance, creating it if it does not exist.
 */
Clipboard *Clipboard::instance()
{
	if (!s_instance) s_instance = new Clipboard();
	return s_instance;
}

/**
 * Copy the given data to the local clipboard and make if available to
 * the global clipboard.
 *
 * The data is not copied to another application until, a paste request is received.
 *
 * @param file_type the file type of the data
 * @param data object to provide the data to the clipboard
 *            (the clipboard takes ownership of this object and will delete it when it is no longer required)
 * @param add if false (the default) the data will replace existing data and the native file type is set to file_type.
 *            if true the file_type will be added the list of supported types.
 */
void Clipboard::copy(int file_type, ClipboardData *data, bool add /*= false*/)
{
	if (!add)
	{
		clear();
		_native_file_type = file_type;
	}
	_data[file_type] = data;
	if (!_owns_clipboard) claim_clipboard();
}

/**
 * Convenience method to make it easy to copy text to the clipboard
 * @param text the text to copy.
 * @param file_type (default 0xFFF - Text)
 */
void Clipboard::copy(const std::string &text, int file_type /*= 0xFFF*/)
{
	copy(file_type, new ClipboardText(text));
}

/**
 * Convenience method to place bytes on to the clipboard
 *
 * @param file_type file type for the bytes
 * @param bytes bytes to place on the clipboard
 * @param size size of the bytes to place on the clipboard
 * @param copy_bytes true to make a copy of the bytes, 
 *                   false to take ownership and delete them with delete[] bytes when no longer required.
 */
void Clipboard::copy(int file_type, char *bytes, int size, bool copy_bytes/* = true*/)
{
	copy(file_type, new ClipboardBytes(bytes, size, copy_bytes));
}

/**
 * Paste data from global clipboard
 *
 * The Gadget to paste to must have a Loader with add loader to
 * receive the data. To receive data when this application owns
 * the clipboard the loader must support ram transfer.
 *
 * @param file_type file type to paste. If this type is not on the clipboard
 * it will try to load the native format stored on the clipboard. This means the gadget
 * loader routine may receive a different format if it is set to load all types.
 * @param gadget gadget to paste to
 * @param x x-coordinate for paste (defaults to 0)
 * @param y y-coordinate for paste (defaults to 0)
 */
void Clipboard::paste(int file_type, tbx::Gadget &gadget, int x, int y)
{
	paste(&file_type,1, gadget);
}

/**
 * Paste data from global clipboard
 *
 * The Gadget to paste to must have a Loader with add loader to
 * receive the data. To receive data when this application owns
 * the clipboard the loader must support ram transfer.
 *
 * @param file_types list of file type to paste in order of preference.
 * If none of these types are on the clipboard
 * it will try to load the native format stored on the clipboard.
 * This means the gadget
 * loader routine may receive a different format if it is set to load all types.
 * @param num_types number of file types in the list.
 * @param gadget gadget to paste to (must be in window or null)
 * @param x x-coordinate for paste
 * @param y y-coordinate for paste
 */
void Clipboard::paste(int *file_types, int num_types, Gadget &gadget, int x /*= 0*/, int y /*= 0*/)
{
	paste(file_types, num_types, gadget.window(), gadget, x,y);
}

/**
 * Paste data from global clipboard (helper routine called from public paste routines)
 *
 * The Gadget to paste to must have a Loader with add loader to
 * receive the data. To receive data when this application owns
 * the clipboard the loader must support ram transfer.
 *
 * @param file_types list of file type to paste in order of preference.
 * If none of these types are on the clipboard
 * it will try to load the native format stored on the clipboard.
 * This means the gadget
 * loader routine may receive a different format if it is set to load all types.
 * @param num_types number of file types in the list.
 * @param window window to paste to
 * @param gadget gadget to paste to (must be in window or null)
 * @param x x-coordinate for paste
 * @param y y-coordinate for paste
 */
void Clipboard::paste(int *file_types, int num_types, Window window, Gadget gadget, int x, int y)
{
	if (_owns_clipboard)
	{
		LoaderManager *manager = LoaderManager::instance();
		if (manager)
		{
			int file_type;
			ClipboardData*found_data = 0;
			for (int j = 0; j < num_types && found_data == 0; ++j)
			{
				file_type = file_types[j];
				found_data = clipboard()->data(file_type);
			}

            // According to the spec, return native format if requested
			// format is not available.
            if (!found_data)			
			{
				found_data = clipboard()->native_data();
				if (found_data) file_type = clipboard()->native_file_type();
			}
			if (found_data)
			{
				found_data->copy_started();
				manager->send_local(file_type, found_data->data(), found_data->size(), gadget.window(), gadget, x, y);
				found_data->copy_finished();
			}
		}
	} else
	{
		// Pasting from foreign application
		WimpMessage data_req(Message_DataRequest, 11 + num_types);
		data_req[5] = window.window_handle();
		if (gadget.null())
		{
			data_req[6] = -1;
		} else
		{
			std::vector<IconHandle> icons = gadget.icon_list();
			if (icons.empty()) return; // Nowhere to paste to
			data_req[6] = icons.front();
		}
		data_req[7] = x;
		data_req[8] = y;
		data_req[9] = 4; // bit 2, send data from clipboard
		for (int j = 0; j < num_types;j++)
		{
			data_req[10+j] = *file_types++;
		}
		data_req[10+num_types] = -1;
		
		data_req.send(WimpMessage::Recorded, WimpMessage::Broadcast);
		_message_handler.paste_ref(data_req.my_ref());
		LoaderManager *manager = LoaderManager::instance();
		if (manager == 0) manager = new LoaderManager();
		manager->paste_ref(data_req.my_ref());
	}
}

/**
 * Paste data from global clipboard
 *
 * The window to paste to must have a Loader with add loader to
 * receive the data. To receive data when this application owns
 * the clipboard the loader must support ram transfer.
 *
 * @param file_type file type to paste. If this type is not on the clipboard
 * it will try to load the native format stored on the clipboard. This means the gadget
 * loader routine may receive a different format if it is set to load all types.
 * @param window window to paste to
 * @param x x-coordinate for paste (defaults to 0)
 * @param y y-coordinate for paste (defaults to 0)
 */
void Clipboard::paste(int file_type, Window &window, int x /*= 0*/, int y /*= 0*/)
{
	paste(&file_type,1,window,x,y);
}

/**
 * Paste data from global clipboard
 *
 * The window to paste to must have a Loader with add loader to
 * receive the data. To receive data when this application owns
 * the clipboard the loader must support ram transfer.
 *
 * @param file_type file type to paste. If this type is not on the clipboard
 * it will try to load the native format stored on the clipboard. This means the gadget
 * loader routine may receive a different format if it is set to load all types.
 * @param num_types number of file types in the list.
 * @param window window to paste to
 * @param x x-coordinate for paste (defaults to 0)
 * @param y y-coordinate for paste (defaults to 0)
 */
void Clipboard::paste(int *file_types, int num_types, Window &window, int x /*= 0*/, int y /*= 0*/)
{
	paste(file_types, num_types, window, Gadget(), x, y);
}

/**
 * Check the global clipboard for a given file type.
 * The result of the check is returned via the ClipboardFormatListener.
 *
 * @param file_type the file type to check for
 * @param window a window to use for checking.
 */
void Clipboard::check(int file_type, Window &window)
{
	check(&file_type, 1, window);
}

/**
 * Check the global clipboard for once of thegiven file type.
 * The result of the check is returned via the ClipboardFormatListener.
 *
 * @param file_types an array of file types to check for.
 * @param num_types the number of types in the array.
 * @param window a window to use for checking.
 */
void Clipboard::check(int *file_types, int num_types, Window &window)
{
	if (_owns_clipboard)
	{
		int file_type;
		ClipboardData*found_data = 0;
		for (int j = 0; j < num_types && found_data == 0; ++j)
		{
			file_type = file_types[j];
			found_data = clipboard()->data(file_type);
		}

		// According to the spec, return native format if requested
		// format is not available.
		if (!found_data)			
		{
			found_data = clipboard()->native_data();
			if (found_data) file_type = clipboard()->native_file_type();
		}
		if (found_data)
		{
			fire_format_found(file_type);			
		} else
		{
			fire_clipboard_empty();
		}
	} else
	{
		// Pasting from foreign application
		WimpMessage data_req(Message_DataRequest, 11 + num_types);
		data_req[5] = window.window_handle();
		data_req[6] = -1;
		data_req[7] = 0;
		data_req[8] = 0;
		data_req[9] = 4; // bit 2, send data from clipboard
		for (int j = 0; j < num_types;j++)
		{
			data_req[10+j] = *file_types++;
		}
		data_req[10+num_types] = -1;
		
		data_req.send(WimpMessage::Recorded, WimpMessage::Broadcast);
		_message_handler.paste_ref(data_req.my_ref());
		LoaderManager *manager = LoaderManager::instance();
		if (manager == 0) manager = new LoaderManager();
		manager->paste_ref(data_req.my_ref());
		manager->message_intercept(&_message_handler);
	}
}



/**
 * Clear all the local clipboard data
 */
void Clipboard::clear()
{
	for(std::map<int,ClipboardData*>::iterator i = _data.begin();
		i != _data.end(); ++i)
	{
		delete i->second;
	}
		
	_data.clear();
	_message_handler.clear();
}

/**
 * Claim the caret/selection for this application.
 *
 * An application should call this when it gains the focus
 * to allow it to paste data and inform other applications
 * they no longer have the input focus
 *
 * This can be called safely multiple times, the caret is only
 * claimed from the system if this application doesn't already
 * have it.
 */
void Clipboard::claim_caret()
{
	if (!_owns_caret)
	{
		WimpMessage claim(Message_ClaimEntity, 6);
		claim[5] = 3; // caret or selection being claimed
		claim.send(WimpMessage::User, WimpMessage::Broadcast);
		_owns_caret = true;
	}
}

/**
 * Get data stored locally of the given file type.
 *
 *@param file_type file type of data to get
 *@returns pointer to the clipboard data (or 0 if no data of the given type)
 */
ClipboardData *Clipboard::data(int file_type) const
{
	std::map<int, ClipboardData *>::const_iterator found = _data.find(file_type);
	return (found == _data.end()) ? 0 : found->second;
}

/**
 * Get the native data from the data stored locally.
 *
 * The native data is the first that was copied to the clipboard
 * with copy.
 *@returns clipboard data for the native file type (or 0 if none)
 */
ClipboardData *Clipboard::native_data() const
{
	return data(_native_file_type);
}

/**
 * Return the native file type.
 *
 * This is set when the first data is copied to the clipboard
 * @returns native file type (or -1) if nothing has been put on the clipboard
 */
int Clipboard::native_file_type() const
{
	return _native_file_type;
}

// Private method to claim the clipboard
// called by the put methods.
void Clipboard::claim_clipboard()
{
	WimpMessage claim(Message_ClaimEntity, 6);
	claim[5] = 4; // clipboard being claimed
	claim.send(WimpMessage::User, WimpMessage::Broadcast);
	_owns_clipboard = true;
}

/**
 * Add a listener for when the clipboard has been claimed by another application.
 * @param listener listener to add
 */
void Clipboard::add_claimed_listener(ClipboardClaimedListener *listener)
{
	_claimed_listeners.push_back(listener);
}

/**
 * Remove a listener for when the clipboard has been claimed by another application.
 * @param listener listener to remove
 */
void Clipboard::remove_claimed_listener(ClipboardClaimedListener *listener)
{
	for(std::vector<ClipboardClaimedListener *>::iterator i = _claimed_listeners.begin();
	    i != _claimed_listeners.end();
		++i)
	{
		if (*i == listener)
		{
			_claimed_listeners.erase(i);
			return;
		}
	}
}

/**
 * Add a listener for the result of a Clipboard::check call.
 * @param listener listener to add
 */
void Clipboard::add_format_listener(ClipboardFormatListener *listener)
{
	if (!_format_listeners) _format_listeners = new std::vector<ClipboardFormatListener*>();
	_format_listeners->push_back(listener);
}

/**
 * Remove a listener for the result of a Clipboard::check call.
 * @param listener listener to remove
 */
void Clipboard::remove_format_listener(ClipboardFormatListener *listener)
{
	if (!_format_listeners) return;
	for(std::vector<ClipboardFormatListener *>::iterator i = _format_listeners->begin();
	    i != _format_listeners->end();
		++i)
	{
		if (*i == listener)
		{
			_format_listeners->erase(i);
			return;
		}
	}
}

// Private methods to update owner flags and fire listeners
//! @cond INTERNAL
void Clipboard::fire_claimed(bool clipboard_claimed, bool caret_claimed)
{
	if (clipboard_claimed) _owns_clipboard = false;
	if (caret_claimed) _owns_caret = false;
	
	for(std::vector<ClipboardClaimedListener *>::iterator i = _claimed_listeners.begin();
		i != _claimed_listeners.end(); ++i)
	{
		if (clipboard_claimed) (*i)->clipboard_claimed();
		if (caret_claimed) (*i)->clipboard_claimed_caret();
	}
}

void Clipboard::fire_format_found(int file_type)
{
	if (!_format_listeners) return;
	std::vector<ClipboardFormatListener*> listeners(*_format_listeners);
	for(std::vector<ClipboardFormatListener *>::iterator i = listeners.begin();
		i != listeners.end(); ++i)
	{
		(*i)->clipboard_format_available(file_type);
	}
}

void Clipboard::fire_clipboard_empty()
{
	if (!_format_listeners) return;
	std::vector<ClipboardFormatListener*> listeners(*_format_listeners);
	for(std::vector<ClipboardFormatListener *>::iterator i = listeners.begin();
		i != listeners.end(); ++i)
	{
		(*i)->clipboard_empty();
	}
}


// Set up the message handling class
Clipboard::MessageHandler::MessageHandler()
	: _saver(0),
	_save_data(0),
	_save_buffer(0)
{
	app()->add_user_message_listener(Message_ClaimEntity, this);
	app()->add_recorded_message_listener(Message_DataRequest, this);
	app()->add_acknowledge_message_listener(Message_DataRequest, this);
}

Clipboard::MessageHandler::~MessageHandler()
{
	clear();
	delete _saver; 
}

// Clear internal data
void Clipboard::MessageHandler::clear()
{
	delete _save_buffer;
	_save_buffer = 0;
	_save_data = 0;
}

// Handle clipboard specific messages
void Clipboard::MessageHandler::user_message(WimpMessageEvent &event)
{
	if (event.message().message_id() == Message_ClaimEntity)
	{
		// Only care if from another task
		if (event.message().sender_task_handle() != app()->task_handle())
		{
			int flags = event.message()[5];
			bool caret_claimed = ((flags & 3) != 0);
			bool clipboard_claimed = ((flags &4)!=0);
			clipboard()->fire_claimed(clipboard_claimed, caret_claimed);		
		}
	}
}

void Clipboard::MessageHandler::recorded_message(WimpMessageEvent &event, int reply_to)
{
    if (event.message().message_id() == Message_DataRequest)
	{
		const WimpMessage &data_req = event.message();
		
	    // Must ignore message if bit 2 is not set
		if (data_req[9] & 4)
		{			
			int file_type = -1;
			ClipboardData*found_data = 0;
			int ft_index = 10;
			while (!found_data && data_req[ft_index] != -1)
			{
				file_type = data_req[ft_index];
				found_data = clipboard()->data(file_type);
				if (!found_data) ++ft_index;
			}

            // According to the spec, return native format if requested
			// format is not available.
            if (!found_data)			
			{
				found_data = clipboard()->native_data();
				if (found_data) file_type = clipboard()->native_file_type();
			}
			
			if (found_data)
			{
			    if (!_saver)
				{
				   _saver = new Saver();
				   _saver->set_finished_handler(this);
					_saver->set_save_completed_handler(this);
					_saver->set_save_to_file_handler(this);
					_saver->set_fill_buffer_handler(this);
				}
				_save_data = found_data;
				_save_data->copy_started();

				_saver->save_for_data_request(data_req, "selection", file_type, found_data->size());
			}
		}
	}
}

void Clipboard::MessageHandler::acknowledge_message(WimpMessageEvent &event)
{
    if (event.message().message_id() == Message_DataRequest)
	{
		if (event.message().your_ref() == 0)
		{
			// Clipboard check response
			if (LoaderManager::instance()
			    && LoaderManager::instance()->message_intercept() == this)
			{
				LoaderManager::instance()->message_intercept(0);
				clipboard()->fire_clipboard_empty();
			}			
		}
	}
}

void Clipboard::MessageHandler::saver_finished(const tbx::SaverFinishedEvent &finished)
{
	if (_save_data)
	{
		_save_data->copy_finished();
		clear();
	}
}

/*
 * Save has completed
 */
void Clipboard::MessageHandler::saver_save_completed(tbx::SaverSaveCompletedEvent &)
{
	if (_save_data)
	{
		_save_data->copy_finished();
		clear();
	}
}

/**
 * Request to copied data to a file
 */
void Clipboard::MessageHandler::saver_save_to_file(tbx::Saver saver, std::string file_name)
{
	bool ok = false;
	if (_save_data)
	{
		std::ofstream file(file_name.c_str());
		file.write(_save_data->data(), _save_data->size());
		ok = file.good();
		file.close();
	}

	saver.file_save_completed(ok, file_name);
}

/**
 * Request to fill a buffer for transfer
 */
void Clipboard::MessageHandler::saver_fill_buffer(tbx::Saver saver, int size, void *buffer, int already_transmitted)
{
	if (!_save_data)
	{
		saver.buffer_filled(buffer,0);
		return;
	}
		
	unsigned char *bytes = (unsigned char *)_save_data->data();
	int left = _save_data->size() - already_transmitted;

	if (buffer == 0)
	{
		// Only maximum of our size and their size is needed
		if (size > left) size = left;
		_save_buffer = new char[size];
		buffer = _save_buffer;
	}
	if (left > size) left = size;
	std::memcpy(_save_buffer, bytes + already_transmitted, left);
	saver.buffer_filled(_save_buffer, left);
}

// Temporary loader intercept used when check for clipboard formats
bool Clipboard::MessageHandler::loader_message_intercept(WimpMessage::SendType type, WimpMessageEvent &event, int reply_to)
{
	if (type == WimpMessage::User || type == WimpMessage::Recorded)
	{
		if (event.message().message_id() == Message_DataSave)
		{
			if (event.message().your_ref()) // Message sent from us if no 0
			{
				// Message processed so dump intercept - do it before call in case it's readded
				LoaderManager::instance()->message_intercept(0);
				clipboard()->fire_format_found(event.message()[10]);
				return true;				
			} else
			{
				// Something didn't get cleaned - dump the intercept to be safe
				LoaderManager::instance()->message_intercept(0);
			}
		}
	}
	return false;
}

//! @endcond INTERNAL

/**
 * Construct a clipboard data object to hold an array of bytes
 *
 * @param bytes the bytes to store on the clipboard (see copy flag for ownership)
 * @param size number of bytes to store on the clipboard
 * @param copy true (the default) to make a copy of the given bytes.
 *              false take ownership of the bytes. This class will delete them when
 *              no longer required (they must have been allocated with new[].
 */
ClipboardBytes::ClipboardBytes(char *bytes, int size, bool copy /*= true*/)
   : _data(0), _size(0)
{
	if (copy)
	{
		_data = new char[size];
		std::memcpy(_data, bytes, size);
	} else
	{
		_data = bytes;
	}
	_size = size;
}



}
