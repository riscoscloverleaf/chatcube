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

#include "saver.h"
#include "application.h"
#include "path.h"
#include "reporterror.h"
#include "swis.h"

#include <iostream>
#include <cstring>

namespace tbx {

/**
 * Constructor for an uninitialised save operation
 */
Saver::Saver()
{
	_impl = new SaverImpl();
}

/**
 * Constructor from another Saver
 *
 * This constructer shares a reference counted internal representation
 * of the save operations
 *
 * @param other Saver to share save implementation with.
 */
Saver::Saver(const Saver &other)
{
	_impl = other._impl;
	_impl->add_ref();
}

// Private constructor for save handlers
Saver::Saver(SaverImpl *impl)
{
	_impl = impl;
	_impl->add_ref();
}


/**
 * Assign this saver from another
 *
 * The assignment shares a reference counted internal representation
 * of the save operations
 *
 * @param other Saver to share save implementation with.
 */
Saver &Saver::operator=(const Saver &other)
{
	SaverImpl *imp = _impl;
	_impl = other._impl;
	_impl->add_ref();
	imp->release();
	return *this;
}

/**
 * Start the save
 *
 * @param where Location to save to
 * @param leaf_name leaf_name of the file to save
 * @param file_type file type for the saved file
 * @param file_size estimated file size for the saved file
 * @param your_ref reference from other application when saving from a data request or equivalent (0 for no ref)
 */
void Saver::save(const PointerInfo &where, const std::string &leaf_name, int file_type, int file_size)
{
	_impl->_where = where;
	_impl->_leaf_name = leaf_name;
	_impl->_file_type = file_type;
	_impl->_file_size = file_size;
	_impl->_your_ref = 0;

	_impl->start();
}

/**
 * Start save in response to a DataRequest message.
 *
 * @param leaf_name leaf_name of the file to save
 * @param file_type file type for the saved file
 * @param file_size estimated file size for the saved file
 */
void Saver::save_for_data_request(const WimpMessage &data_req, const std::string &leaf_name, int file_type, int file_size)
{
    _impl->_where = tbx::PointerInfo(
					(WindowHandle)data_req[5],
					(IconHandle)data_req[6],
					data_req[7], // x-coordinate
					data_req[8], // y-coordinate
					0 /* Buttons */
					);
	_impl->_leaf_name = leaf_name;
	_impl->_file_type = file_type;
	_impl->_file_size = file_size;
	_impl->_your_ref = data_req.my_ref();

	_impl->start();
}

/**
 * Call in a RAM transfer when another buffer has been made
 * available.
 *
 * @param buffer new buffer to transfer
 * @param size size of data in the buffer
 */
void Saver::buffer_filled(void *buffer, int size)
{
	_impl->transfer_data(buffer, size);
}

/**
 * Call this after saving a file in the SaveAsSaveToFileListener
 *
 * @param successful set to true if save was successful
 * @param file_name actual file name client saved to.
 */
void Saver::file_save_completed(bool successful, std::string file_name)
{
	if (successful)
	{
		_impl->send_data_load(file_name);
	}
}


Saver::SaverImpl::SaverImpl() :
		_ref_count(1),
		_save_to_file_handler(0),
		_fill_buffer_handler(0),
		_completed_handler(0),
		_finished_handler(0)
{
}

/**
 * Start the save operation
 */
void Saver::SaverImpl::start()
{
	app()->add_user_message_listener(2, this); // DataSaveAck
	app()->add_user_message_listener(4, this); // DataLoadAck
	app()->add_recorded_message_listener(6, this); // RAMFetch
	app()->add_acknowledge_message_listener(1, this); // DataSave
	app()->add_acknowledge_message_listener(3, this); // DataLoad
	app()->add_acknowledge_message_listener(7, this); // RAMTransmit

	_safe = 0;
	_source_buffer = 0;
	_transmitted = 0;

	int msg_size = 11 + ((_leaf_name.size() + 4) / 4);
	WimpMessage data_save(1, msg_size);
	data_save[5] = _where.window_handle();
	data_save[6] = _where.icon_handle();
	data_save[7] = _where.mouse_x();
	data_save[8] = _where.mouse_y();
	data_save[9] = _file_size;
	data_save[10] = _file_type;
	_leaf_name.copy(data_save.str(11), _leaf_name.size());
	*(data_save.str(11) + _leaf_name.size()) = 0;

	data_save.your_ref(_your_ref);

	data_save.send(WimpMessage::Recorded, _where.window_handle(), _where.icon_handle());
	_my_ref = data_save.my_ref();
	add_ref();
}

/**
 * Messages from another application
 */
void Saver::SaverImpl::user_message(WimpMessageEvent &event)
{
	if (event.message().your_ref() != _my_ref) return;

	switch(event.message().message_id())
	{
	case 2: // DataSaveAck - want file via file transfer
		_safe = (event.message().word(9) != -1);
		if (_save_to_file_handler)
		{
			Saver saver(this);
			_reply_to = event.message().sender_task_handle();
			_msg_ref = event.message().my_ref();
			_save_to_file_handler->saver_save_to_file(saver, event.message().str(11));
			event.claim();
		}
		break;

	case 4: // DataLoadAck - target application has successfully loaded the app
		if (_completed_handler)
		{
			SaverSaveCompletedEvent cevent(Saver(this), _safe, event.message().str(11));
			_completed_handler->saver_save_completed(cevent);
		}
		finished(true);
		break;
	}
}

/**
 * Recorded messages
 */
void Saver::SaverImpl::recorded_message(WimpMessageEvent &event, int reply_to)
{
	if (event.message().your_ref() != _my_ref) return;
	switch(event.message().message_id())
	{
	case 6: // RAM fetch
		_msg_ref = event.message().my_ref();
		_reply_to = reply_to;
		_dest_buffer = (char *)event.message().word(5);
		_dest_size = event.message().word(6);
		// If we don't have a buffer fill routine then this message will be
		// ignored so it will fall back to a file save
		if (_fill_buffer_handler)
		{
			Saver saver(this);
			_fill_buffer_handler->saver_fill_buffer(saver, _dest_size, _source_buffer, _transmitted);
		}
		break;
	}
}

/**
 * Acknowledgements from other application
 */
void Saver::SaverImpl::acknowledge_message(WimpMessageEvent &event)
{
	if (event.message().your_ref() != _my_ref) return;

    switch(event.message().message_id())
    {
    case 1: // DataSave came back so target didn't accept file
    	event.claim();
    	finished(false);
    	break;

	case 3: // Message DataLoad - we didn't receive a reply from our dataload
		    // so delete WIMP scrap.
		if (strcasecmp(event.message().str(11), "<Wimp$Scrap>") == 0)
		{
			Path path("<Wimp$Scrap>");
			path.remove();
		}
		event.claim();
		finished(false);
		break;

	case 7: // RAM Transmit came back - error
		tbx::report_error("Failed to save data");
		event.claim();
		finished(false);
		break;
    }
}

void Saver::SaverImpl::finished(bool saved)
{
	app()->remove_user_message_listener(2, this); // DataSaveAck
	app()->remove_user_message_listener(4, this); // DataLoadAck
	app()->remove_recorded_message_listener(6, this); // RAMFetch
	app()->remove_acknowledge_message_listener(1, this); // DataSave
	app()->remove_acknowledge_message_listener(3, this); // DataLoad
	app()->remove_acknowledge_message_listener(7, this); // RAMTransmit
	if (_finished_handler)
	{
		Saver saver(this);
		SaverFinishedEvent event(saver, saved);
		_finished_handler->saver_finished(event);
	}
	release();
}

/**
 * Send data load message to other after successful save.
 */
void Saver::SaverImpl::send_data_load(const std::string &file_name)
{
	WimpMessage data_load(3, 11 + ((file_name.size()+4)/4));
	data_load.your_ref(_msg_ref);
	data_load[5] = _where.window_handle();
	data_load[6] = _where.icon_handle();
	data_load[7] = _where.mouse_x();
	data_load[8] = _where.mouse_y();
	data_load[9] = _file_size;
	data_load[10] = _file_type;
	file_name.copy(data_load.str(11), file_name.size());
	*(data_load.str(11) + file_name.size()) = 0;
	data_load.send(WimpMessage::Recorded, _reply_to);
	_my_ref = data_load.my_ref();
}

/**
 * Transmit data to other app
 */
void Saver::SaverImpl::transfer_data(void *buffer, int size)
{
	_source_buffer = (char *)buffer;
	_transmitted += size;
	// WIMP transfer block to copy bytes across
	_swix(0x400F1, _INR(0,4), app()->task_handle(), buffer, _reply_to, _dest_buffer, size);
	WimpMessage ram_transmit(7, 7); // Message RAMTransmit
	ram_transmit.your_ref(_msg_ref);
	ram_transmit[5] = (int)_dest_buffer;
	ram_transmit[6] = size;

	bool more = (size == _dest_size);

	WimpMessage::SendType send_type = (more) ? WimpMessage::Recorded : WimpMessage::User;
	ram_transmit.send(send_type, _reply_to);
	_my_ref = ram_transmit.my_ref();

	if (!more) finished(true);
}


}
