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

#include "memoryloader.h"
#include "messagewindow.h"
#include <fstream>
#include <cstring>

namespace tbx {

/**
 * Construct the memory loader object
 *
 * @param show_error true to show a message box on error (default false)
 * @param error_title Title to show if an error is reported ("" or 0 for the default title)
 */
MemoryLoader::MemoryLoader(bool show_error /*=false*/, const char *error_title /*= 0*/) :
   _show_error(show_error),
   _buffer(0),
   _size(0),
   _offset(0)
{
	if (error_title) _error_title = error_title;
}

MemoryLoader::~MemoryLoader()
{
	delete []_buffer;
}

/**
 * Overriden from Loader to load the file and pass
 * the bytes loaded to the data_loaded function
 *
 * The file name for the load is in event.file_name().
 *
 * @return true if file is loaded.
 */
bool MemoryLoader::load_file(LoadEvent &event)
{
    std::ifstream file(event.file_name().c_str());
    if (file)
    {    
        file.seekg(0, std::ios::end);
    	int len = file.tellg();
        file.seekg(0, std::ios::beg);
		try
		{
			delete [] _buffer;
			_buffer = new char[len];
			_size = len;
			file.read(_buffer, _size);
			bool data_claimed = false;
			data_loaded(event, _buffer, _size, data_claimed);
			if (!data_claimed) delete [] _buffer;
			_buffer = 0;
			_size = 0;
			_offset = 0;
			return false;
		} catch(...)
		{
			return false;
		}
	}
	
	return false;
	
}


/**
 * Set up buffer for application to application data transfer.
 *
 * This method is overriden from the Loader class to create an
 * internal buffer which will be filled and then passed to the
 * data_loaded method.
 *
 * @param event LoadEvent that started the data transfer
 * @param buffer_size size of the buffer returned (defaults to event.estimated_size())
 * @return internal buffer created for the data transfer.
 */
void *MemoryLoader::data_buffer(const LoadEvent &event, int &buffer_size)
{
	delete [] _buffer;
	_size = buffer_size;
	_offset = 0;
	_buffer = new char[_size];
	return _buffer;
}

/**
 * Overriden from Loader to populate the internal data buffer from
 * the transfer.
 * 
 * Once all data is received the data_loaded method will be called.
 *
 * @param event Details of buffer received
 * @returns true if buffer could be processed, false on error.
 */
bool MemoryLoader::data_received(DataReceivedEvent &event)
{
	_offset += event.received();
	event.buffer_size(_size - _offset);
	if (event.more())
	{
		if (event.buffer_size() <= 0)
		{
			int new_size = _size + 256;
			try
			{
			char *new_buf = new char[new_size];
			std::memcpy(new_buf, _buffer, _offset);
			delete [] _buffer;
			_buffer = new_buf;
			_size = new_size;
			event.buffer_size(_size - _offset);
			} catch(std::exception &except)
			{
				// Most likely a memory exception so give up
				// Delete buffer as it may be large by now.
				delete [] _buffer;
				_buffer = 0;
				_size = _offset = 0;
				data_exception(event.load_event(), except);
				return false;
			}			
		}
		event.buffer(_buffer + _offset);
	} else
	{
		bool data_claimed = false;
		data_loaded(event.load_event(), _buffer, _offset, data_claimed);
		if (!data_claimed) delete [] _buffer;
		_buffer = 0;
		_size = 0;
		_offset = 0;		
	}

	return true;
}


/**
 * Informs loader that an error occurred during application to application
 * data transfer.
 *
 * overrided to show a message if show_error was set in the constructor
 * override for alternate processing
 *
 * @param event load event that started the transfer
 */
void MemoryLoader::data_error(const LoadEvent &event)
{
	if (_show_error)
	{
		show_message("An error occurred during loading", _error_title);
	}
}
	
/**
 * Informs loader an exception occurred during the loading process.
 *
 * If not overriden it will show an error box if show_error was
 * set in the constructor or call data_error if not.
 *
 * @param event load event that started the transfer
 * @param except exception that occurred.
 */
void MemoryLoader::data_exception(const LoadEvent &event, std::exception &except)
{
	if (_show_error)
	{
		std::string msg = "Error while loading: ";
		msg += except.what();
		show_message(msg, _error_title);
	} else
	{
		data_error(event);
	}
}
	
}
