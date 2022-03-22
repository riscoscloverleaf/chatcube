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
 * saver.h
 *
 *  Created on: 08-Apr-2009
 *      Author: alanb
 */

#ifndef TBX_SAVER_H_
#define TBX_SAVER_H_

#include "pointerinfo.h"
#include "wimpmessagelistener.h"
#include <string>

namespace tbx {

// Handler classes used by Saver
class SaverSaveToFileHandler;
class SaverFillBufferHandler;
class SaverSaveCompletedHandler;
class SaverFinishedHandler;

/**
 * Class to give the same interface to saving
 * to an external application as the SaveAs dialogue.
 *
 * This allows a convenient interface to save to another
 * application or the filer.
 *
 * To use, set the handlers and then call the save method.
 *
 * The Saver class is just a reference to the actual save so does not have to
 * be kept after the save has started.
 *
 */
class Saver
{
	// Reference counted structure so Saver does not need to be saved
	class SaverImpl :
	   public WimpUserMessageListener,
	   public WimpRecordedMessageListener,
	   public WimpAcknowledgeMessageListener
	{
		int _ref_count;
		int _reply_to;
		int _my_ref;
		int _msg_ref;
		char *_source_buffer;
		char *_dest_buffer;
		int _dest_size;
		int _transmitted;

		virtual ~SaverImpl() {}

		virtual void user_message(WimpMessageEvent &event);
		virtual void recorded_message(WimpMessageEvent &event, int reply_to);
		virtual void acknowledge_message(WimpMessageEvent &event);

	public:
		PointerInfo _where;
		std::string _leaf_name;
		int _file_type;
		int _file_size;
		int _your_ref;
		bool _safe;
		SaverSaveToFileHandler *_save_to_file_handler;
		SaverFillBufferHandler *_fill_buffer_handler;
		SaverSaveCompletedHandler *_completed_handler;
		SaverFinishedHandler *_finished_handler;

		SaverImpl();
		void add_ref() {_ref_count++;}
		void release() {if (--_ref_count == 0) delete this;}
		void start();
		void send_data_load(const std::string &file_name);
		void transfer_data(void *buffer, int size);
		void finished(bool saved);

	} *_impl;

	Saver(SaverImpl *impl);
	friend class SaverImpl;
public:
	Saver();
	Saver(const Saver &other);

	Saver &operator=(const Saver &other);

	/**
	 * Check if two savers are the same underlying save operation
	 *
	 * @param other Saver to compare with
	 * @return true if they are the same save operation
	 */
	bool operator==(const Saver &other) const {return _impl == other._impl;}
	/**
	 * Check if two savers are not the same underlying save operation
	 *
	 * @param other Saver to compare with
	 * @return true if they are not the same save operation
	 */
	bool operator!=(const Saver &other) const {return _impl != other._impl;}

	void save(const PointerInfo &where, const std::string &leaf_name, int file_type, int file_size);
	void save_for_data_request(const WimpMessage &data_req, const std::string &leaf_name, int file_type, int file_size);

	/**
	 * Get the proposed leaf name for the file to be saved
	 */
	std::string leaf_name() const {return _impl->_leaf_name;}

	/**
	 * Get the file type from the save
	 */
	int file_type() const {return _impl->_file_type;}

	/**
	 * Get the file size from the SaveAs object
	 */
	int file_size() const {return _impl->_file_size;}

	/**
	 * Check if the save is to a safe locations.
	 *
	 * This is set before the save to file handler is called.
	 */
	bool safe() const {return _impl->_safe;}


//TODO: See if this is needed: void set_data_address(void *data, int size);
	void buffer_filled(void *buffer, int size);
	void file_save_completed(bool successful, std::string file_name);

	/**
	 * Handler called when the save operation has completed.
	 *
	 * @param handler object to call when save has finished
	 */
	void set_finished_handler(SaverFinishedHandler *handler) {_impl->_finished_handler = handler;}
	/**
	 * Handler called if the save completed successfully
	 *
	 * @param handler object to call if save completed successfully
	 */
	void set_save_completed_handler(SaverSaveCompletedHandler *handler) {_impl->_completed_handler = handler;}

	/**
	 * Handler called to save to a file
	 *
	 * @param handler object to call to save the file
	 */
	void set_save_to_file_handler(SaverSaveToFileHandler *handler) {_impl->_save_to_file_handler = handler;}

	/**
	 * Handler called to fill the buffers for a RAM transfer
	 *
	 * @param handler object to call to fill a buffer
	 */
	void set_fill_buffer_handler(SaverFillBufferHandler *handler) {_impl->_fill_buffer_handler = handler;}

};

/**
 * Event information for when saver dialogue has been completed.
 */
class SaverFinishedEvent
{
	Saver _saver;
	bool _save_done;
public:
	/**
	 * Construct event from a Saver
	 *
	 * @param saver for this save
	 * @param saved true if save completed successully
	 */
	SaverFinishedEvent(Saver &saver, bool saved) : _saver(saver), _save_done(saved) {}

	/**
	 * Return Saver for save that has completed
	 */
	const Saver &saver() const {return _saver;}

	/**
	 * save_done true if the save was successful
	 */
	bool save_done() const	{return _save_done;}
};

/**
 * Listener for when the save as dialogue box has been closed
 */
class SaverFinishedHandler
{
public:
	virtual ~SaverFinishedHandler() {};

	/**
	 * Called when the saver has finished
	 *
	 * @param finished information on how this save finished
	 */
	virtual void saver_finished(const SaverFinishedEvent &finished) = 0;
};

/**
 * Event for SaverSaveCompletedHandler
 */

class SaverSaveCompletedEvent
{
	Saver _saver;
	bool _safe;
	std::string _file_name;
public:
	/**
	 * Construct save completed event
	 *
	 * @param saver Saver for this save
	 * @param safe true if save was to a safe location
	 * @param file_name full name where file was saved
	 */
	SaverSaveCompletedEvent(const Saver &saver, bool safe, const std::string &file_name) :
		_saver(saver), _safe(safe), _file_name(file_name)
	{}

	/**
	 * Check if the save was to a safe location (e.g. a filing system)
	 */
	bool safe() const {return _safe;}

	/**
	 * Full file name of file location if save was safe
	 */
	const std::string file_name() const {return _file_name;}
};

/**
 * Listener for when a save has been completed.
 */
class SaverSaveCompletedHandler
{
public:
	virtual ~SaverSaveCompletedHandler() {}
	/**
	 * Called when a save has been successful
	 *
	 * @param event details of successful save
	 */
	virtual void saver_save_completed(SaverSaveCompletedEvent &event) = 0;
};

/**
 * Handler to save data to a file
 *
 */
class SaverSaveToFileHandler
{
public:
	virtual ~SaverSaveToFileHandler() {}

	/**
	 * Request to save the file to the specified location.
	 *
	 * Once the file is saved or the attempt to save failed.
	 * saver.file_save_completed should be called.
	 *
	 * @param saver Saver object for save
	 * @param file_name file name to save to
	 */
	virtual void saver_save_to_file(Saver saver, std::string file_name) = 0;
};

/**
 * Handler for SaverFillBuffer handler
 *
 */
class SaverFillBufferHandler
{
public:
	virtual ~SaverFillBufferHandler() {}

	/**
	 * Called when the buffer need to be refilled for a RAM transfer (type 3) save.
	 *
	 * If buffer is 0 the buffer for the RAM transfer should be allocated or
	 * maintain the address in memory of the data to be transferred.
	 *
	 * Either way saveas.buffer_filled should be called once the buffer has
	 * the correct data.
	 *
	 * The transfer will stop when the size of data transferred is less than
	 * a complete buffer.
	 *
	 * @param saver Saver the transfer is occurring on.
	 * @param size of buffer for transfer in bytes
	 * @param buffer for transfer
	 * @param already_transmitted number of bytes already transmitted
	 */
	virtual void saver_fill_buffer(Saver saver, int size, void *buffer, int already_transmitted) = 0;
};

}

#endif /* TBX_SAVER_H_ */
