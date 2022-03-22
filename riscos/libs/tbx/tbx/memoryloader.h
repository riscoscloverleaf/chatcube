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

#ifndef TBX_MEMORYLOADER_H_
#define TBX_MEMORYLOADER_H_

#include "loader.h"

namespace tbx {

/**
 * Helper class to implement the loader interface and
 * call a routine once all the bytes have been loaded
 * with the bytes loaded at the end of the processing.
 *
 * Add the loader to the object you wish to handle the
 * load and override the data_loaded method to process
 * the bytes loaded.
 */
class MemoryLoader : public Loader {
public:
	MemoryLoader(bool show_error = false, const char *error_title = 0);
	virtual ~MemoryLoader();

	virtual bool load_file(LoadEvent &event);

	virtual void *data_buffer(const LoadEvent &event, int &buffer_size);
	virtual bool data_received(DataReceivedEvent &event);
	virtual void data_error(const LoadEvent &event);
	virtual void data_exception(const LoadEvent &event, std::exception &except);
	
	/**
	 * Called when all the data has been loaded this method is called
	 * with a character array of the data recieved.
	 *
	 * @param data bytes loaded.
	 * @param data_size number of bytes in the buffer;
	 * @param claim_data set to true to claim the data buffer.
	 * If claim_data is true, delete the claimed buffer with delete [] when it's finished with.
	 */
	 virtual void data_loaded(const LoadEvent &event, char *data, int data_size, bool &claim_data) = 0;
	 
private:
	bool _show_error;
	std::string _error_title;
    char *_buffer;
	int _size;
	int _offset;	
};

}

#endif /* MEMORY_LOADER_H_ */
