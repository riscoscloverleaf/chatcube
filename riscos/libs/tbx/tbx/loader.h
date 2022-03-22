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

#ifndef TBX_LOADER_H_
#define TBX_LOADER_H_

#include "object.h"
#include "gadget.h"
#include "point.h"

namespace tbx {

/**
 * Class with details of a file load operation.
 *
 * This event is also used with the file open
 * operation, but in this case only the file type,
 * file name and from filer fields have a value.
 */
class LoadEvent
{
private:
	Object _object;
	Gadget _gadget;
	Point _point;
	int _est_size;
	int _file_type;
	std::string _file_name;
	bool _from_filer;

public:
	LoadEvent(Object obj, Gadget gadget, int x, int y,
			  int est, int type, const char *file_name, bool from_filer);

	/**
	 * Object the load is going to occur on
	 */
	Object destination_object() const {return _object;}

	/**
	 * Gadget for load (can be a null gadget if no gadget is involved).
	 */
	Gadget destination_gadget() const {return _gadget;}

	/**
	 * Location for load (screen coordinates)
	 */
	const Point &destination_point() const  {return _point;}

	/**
	 * Estimated size of file.
	 */
	int estimated_size() const  {return _est_size;}

	/**
	 * File type of file
	 */
	int file_type() const {return _file_type;}

	/**
	 * File name of file.
	 *
	 * This will be either a proposed leaf name for the file when
	 * an application is checking if you can load the file or the
	 * full pathname when you are about to load the file.
	 */
	const std::string &file_name() const {return _file_name;}

	/**
	 * Check if the load is from the filer.
	 */
	bool from_filer() const {return _from_filer;}

	/**
	 * Returns true if this event is from an opener added
	 * to the main application.
	 */
	bool opener() const {return _object.null();}

	/**
	 * Update file details for load.
	 *
	 * This is called automatically by the LoaderManager before a file load
	 */
	void update_file_details(const char *file_name, int size) {_file_name = file_name; _est_size = size;}
};

/**
 * Class with details of buffer transferred from another application
 */
class DataReceivedEvent
{
private:
	LoadEvent *_load_event;
	void *_buffer;
	int _buffer_size;
	int _received;
	bool _more;

public:
	/**
	 * Construct with details of received buffer
	 */
	DataReceivedEvent(LoadEvent *event, void *buffer, int buffer_size, int received) :
		_load_event(event),
		_buffer(buffer),
		_buffer_size(buffer_size),
		_received(received)
	{
		_more = (_buffer_size == _received);
	}

	/**
	 * Return o LoadEvent that started the transfer
	 */
	const LoadEvent &load_event() const {return *_load_event;}

	/**
	 * Return the buffer the transmitted data has been copied too
	 */
	void *buffer() const {return _buffer;}

	/**
	 * Set the buffer address for the next bytes received
	 */
	void buffer(void *buf) {_buffer = buf;}

	/**
	 * Return the buffer size of the current buffer
	 */
	int buffer_size() const {return _buffer_size;}

	/**
	 * Set the buffer size for the next transmission from the
	 * other application
	 */
	void buffer_size(int size) {_buffer_size = size;}

	/**
	 * Number of bytes received
	 */
	int received() const {return _received;}

	/**
	 * Return true if more data is to come.
	 */
	bool more() const {return _more;}
};

/**
 * Class to handle file/data loading from the filer or
 * an external application.
 *
 * Add the loader to the object you wish to handle the
 * load and override the load_file method to load the
 * file.
 */
class Loader {
public:
	virtual ~Loader() {};

	/**
	 * Override to do the actual file load.
	 *
	 * The file name for the load is in event.file_name().
	 *
	 * @return true if file is loaded.
	 */
	virtual bool load_file(LoadEvent &event) = 0;

	/**
	 * Override to see if the loader can accept a file.
	 *
	 * Defaults to return true as in normal circumstances
	 * the file type is all that is important and that
	 * will be specified when adding the loader.
	 */
	virtual bool accept_file(LoadEvent &event) {return true;}

	/**
	 * Set up buffer for application to application data transfer
	 *
	 * Called after a file has been accepted so the loader can create a buffer
	 * for the transfer or point to the location to load to.
	 *
	 * Return 0 (as the default does) if application to application data
	 * transfer is not supported.
	 *
	 * @param event LoadEvent that started the data transfer
	 * @param buffer_size size of the buffer returned (defaults to event.estimated_size())
	 * @return buffer for data transfer or 0 if data transfer is not to be used.
	 */
	virtual void *data_buffer(const LoadEvent &event, int &buffer_size) {return 0;}

	/**
	 * Override to receive the file by in memory transfer from another
	 * application.
	 *
	 * This routine may be called multiple times until all the data
	 * is transferred.
	 *
	 * It is possible for the last packet transferred to contain zero
	 * bytes if the penultimate call filled the buffer exactly.
	 *
	 * The DataReceivedEvent contains details of where the data was
	 * copied and the number of bytes received. The more method returns
	 * false when there is no more data to receive.
	 *
	 * By default this returns false as a loader can support transfer
	 * by file only.
	 *
	 * @param event Details of buffer received
	 * @returns true if buffer could be processed, false on error.
	 */
	virtual bool data_received(DataReceivedEvent &event) {return false;}

	/**
	 * Informs loader that an error occurred during application to application
	 * data transfer.
	 *
	 * @param event load event that started the transfer
	 */
	virtual void data_error(const LoadEvent &event) {}
};

}

#endif /* LOADER_H_ */
