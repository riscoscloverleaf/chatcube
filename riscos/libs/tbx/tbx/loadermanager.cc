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

#include "loadermanager.h"
#include "application.h"
#include "point.h"
#include "path.h"
#include "iconbar.h"
#include "string.h"
#include <cstring>

namespace tbx {

// Internal class not included in documentation
//! @cond INTERNAL

LoaderManager *LoaderManager::_instance = 0;

LoaderManager::LoaderManager()
{
	_instance = this;
	_loading = 0;
	_paste_ref = 0;
	_message_intercept = 0;

	// Register for messages we handle
	// User messages
	app()->add_user_message_listener(1, this); // DataSave - from paste
	app()->add_user_message_listener(3, this); // DataLoad
	app()->add_user_message_listener(7, this); // RAMTransmit
	// Recorded messages
	app()->add_recorded_message_listener(1, this); // DataSave
	app()->add_recorded_message_listener(3, this); // DataLoad
	app()->add_recorded_message_listener(7, this); // RAMTransmit
	// Acknowledge messages
	app()->add_acknowledge_message_listener(6, this); // RAMFetch
}

LoaderManager::~LoaderManager()
{
}

void LoaderManager::add_loader(ObjectId handle, ComponentId id, int file_type, Loader *loader)
{
	LoaderItem *new_item = new LoaderItem();
	new_item->handle = handle;
	new_item->id = id;
	new_item->file_type = file_type;
	new_item->loader = loader;
	new_item->next = 0;

	std::map<ObjectId, LoaderItem *>::iterator found = _loaders.find(handle);
	if (found != _loaders.end())
	{
		LoaderItem *prev = 0;
		LoaderItem *item = found->second;

		while (item
				&& (item->id > id
					|| (item->id == id && item->file_type >= file_type)
				    )
				)
		{
			prev = item;
			item = item->next;
		}
        new_item->next = item;
        if (prev)
        {
        	prev->next = new_item;
        } else
        {
        	_loaders[handle] = new_item;
        }
	} else
	{
		_loaders[handle] = new_item;
	}
}

void LoaderManager::remove_loader(ObjectId handle, ComponentId id, int file_type, Loader *loader)
{
	std::map<ObjectId, LoaderItem *>::iterator found = _loaders.find(handle);
	if (found != _loaders.end())
	{
		LoaderItem *prev = 0;
		LoaderItem *item = found->second;
		while (item
				&& (item->id != id || item->file_type != file_type
					|| item->loader != loader)
				)
		{
			prev = item;
			item = item->next;
		}
		if (item)
		{
			if (prev == 0)
			{
				if (item->next == 0)
				{
					_loaders.erase(found);
				} else
				{
					_loaders[handle] = item->next;
				}
			} else
			{
				prev->next = item->next;
			}
			delete item;
		}
	}
}

void LoaderManager::remove_all_loaders(ObjectId handle)
{
	std::map<ObjectId, LoaderItem *>::iterator found = _loaders.find(handle);
	if (found != _loaders.end())
	{
		LoaderItem *item = found->second;
		LoaderItem *next;
		while (item)
		{
			next = item->next;
			delete item;
			item = next;
		}
		_loaders.erase(found);
	}
}

void LoaderManager::remove_all_loaders(ObjectId handle, ComponentId id)
{
	std::map<ObjectId, LoaderItem *>::iterator found = _loaders.find(handle);
	if (found != _loaders.end())
	{
		LoaderItem *prev = 0;
		LoaderItem *item = found->second;
		while (item  && item->id != id)
		{
			prev = item;
			item = item->next;
		}
		if (item && item->id == id)
		{
			LoaderItem *next;

			while (item && item->id == id)
			{
				next = item->next;
				delete item;
				item = next;
			}

			if (prev == 0)
			{
				if (item == 0)
				{
					_loaders.erase(found);
				} else
				{
					_loaders[handle] = item;
				}
			} else
			{
				prev->next = item;
			}
		}
	}
}


void LoaderManager::user_message(WimpMessageEvent &event)
{
	if (_message_intercept && _message_intercept->loader_message_intercept(WimpMessage::User, event,-1))
	{
		event.claim();
		return; // Loader temporarily redirected (e.g. to clipboard)
	}

    switch(event.message().message_id())
    {
    case 1: // Message DataSave
		{
			int reply_to = event.message().sender_task_handle();
			int my_ref = event.message().your_ref();

			// This can come as user message (instead of the recorded message
			// as expected) as a response to a clipboard DataRequest
			if (my_ref == 0 || my_ref == _paste_ref)
			{
				if (my_ref == 0) _paste_ref = 0;
				start_loader(event, reply_to);
				if (_loading) event.claim();
			}
		}
    	break;
    case 3: // Message DataLoad - should be sent recorded (see below, but
    	    // PRM says must allow for both types.
    	process_dataload(event);
    	break;

    case 7: // Message RAM_Transmit - last message of RAM transfer
    	if (_loading && _loading->_my_ref == event.message().your_ref())
    	{
    		ram_transmit(event.message());
    		event.claim();
    	}
    	break;
    }
}

void LoaderManager::recorded_message(WimpMessageEvent &event, int reply_to)
{
	if (_message_intercept && _message_intercept->loader_message_intercept(WimpMessage::Recorded, event, reply_to))
	{
		event.claim();
		return; // Loader temporarily redirected (e.g. to clipboard)
	}

	int my_ref = event.message().your_ref();

    switch(event.message().message_id())
    {
    case 1: // Message DataSave - file coming from another application
    	if (my_ref == 0 || my_ref == _paste_ref)
    	{
			if (my_ref == 0) _paste_ref = 0;
			start_loader(event, reply_to);
			if (_loading) event.claim();

    	}
    	break;

    case 3: // Message DataLoad
    	process_dataload(event);
    	break;

    case 7: // Message RAM_Transmit
    	if (_loading && _loading->_my_ref == my_ref)
    	{
			ram_transmit(event.message());
			event.claim();
    	}
    	break;
    }
}

/**
 * Process user message acknowledge
 */
void LoaderManager::acknowledge_message(WimpMessageEvent &event)
{
	if (_message_intercept && _message_intercept->loader_message_intercept(WimpMessage::Acknowledge, event,-1))
	{
		event.claim();
		return; // Loader temporarily redirected (e.g. to clipboard)
	}
	if (event.message().message_id() == 6)
	{
		// RAMFetched was not acknowledged by the other application.
		// - first time this means use file transfer instead.
		// - after that is is an error
		if (_loading)
		{
			if (_loading->_data_save_reply)
			{
				_loading->_data_save_reply->send(WimpMessage::User, _loading->_reply_to);
				_loading->_my_ref = _loading->_data_save_reply->my_ref();
	    		delete _loading->_data_save_reply;
	    		_loading->_data_save_reply = 0;

			} else
			{
				_loading->_loader->data_error(*(_loading->_load_event));
			}
			event.claim();
		}
	}
}

/**
 * Process DataLoad message.
 *
 * It's either received directly from the filer or as part
 * of the data being loaded from another application.
 */
void LoaderManager::process_dataload(WimpMessageEvent &event)
{
	int reply_to = event.message().sender_task_handle();
	int my_ref = event.message().your_ref();
	bool load = false;

	if (my_ref == 0)
	{
		// First call - probably from filer
		find_loading(event, reply_to);
		load = (_loading != 0);
	} else if (_loading && _loading->_my_ref == my_ref)
	{
		load = true;
	}
	if (load && load_file(event.message()))
	{
		if (strcasecmp(event.message().str(11), "<Wimp$Scrap>") == 0)
		{
			Path path("<Wimp$Scrap>");
			path.remove();
		}

		WimpMessage reply(event.message());
		reply.message_id(4); // DataLoadAck
		reply.your_ref(event.message().my_ref());

		reply.send(WimpMessage::User, reply_to);
		event.claim();
	}
}

/**
 * Process DataSave message by starting the file/data
 * loader if there is any that are appropriate for the
 * message contents.
 *
 * Sends the acknowledgement message back to the application
 * that is doing the data save if a loader to load the file/data
 * is found.
 *
 * @param msg the wimp message for the transfer.
 */
void LoaderManager::start_loader(WimpMessageEvent &msg_event, int reply_to)
{
	const WimpMessage &msg = msg_event.message();

	if (_loading) delete _loading;
	_loading = 0;
	find_loading(msg_event, reply_to);
	if (_loading)
	{
		_loading->_data_save_reply = new WimpMessage(msg, 14);
		_loading->_data_save_reply->message_id(2); // DataSaveAck
		if (_paste_ref) _loading->_data_save_reply->my_ref(_paste_ref);
		_loading->_data_save_reply->your_ref(msg.my_ref());
		_loading->_data_save_reply->word(9) = -1; // Save is unsafe i.e. not to a filer
		strcpy(_loading->_data_save_reply->str(11), "<Wimp$Scrap>");
		void *buffer = _loading->_loader->data_buffer(*(_loading->_load_event), _loading->_buffer_size);
		if (buffer)
		{
			if (_loading->_buffer_size <= 0) _loading->_buffer_size = 256;
			WimpMessage reply(msg, true);
			reply.message_id(6); // RAMFetch
			if (_paste_ref) reply.my_ref(_paste_ref);
			reply.your_ref(msg.my_ref());
			reply.word(5) = (int)buffer;
			reply.word(6) = _loading->_buffer_size;
			reply.send(WimpMessage::Recorded, reply_to);
			_loading->_my_ref = reply.my_ref();
		}
		else
		{
			_loading->_data_save_reply->send(WimpMessage::User, reply_to);
			_loading->_my_ref = _loading->_data_save_reply->my_ref();
		}
	}
	_paste_ref = 0;
}

/**
 * Find the loader for this event and setup and return loading object
 */
void LoaderManager::find_loading(WimpMessageEvent &msg_event, int reply_to)
{
	const WimpMessage &msg = msg_event.message();
	int dest_window = msg[5];
	int dest_icon   = msg[6];

	Object load_object;
	Gadget load_gadget;

	if (dest_window == -2)
	{
		std::map<ObjectId, LoaderItem *>::iterator i;
		for (i = _loaders.begin(); i != _loaders.end(); ++i)
		{
			Object obj(i->first);
			if (obj.toolbox_class() == Iconbar::TOOLBOX_CLASS
					&& Iconbar(obj).icon_handle() == dest_icon)
			{
				load_object = obj;
				break;
			}
		}
	} else
	{
		Window window;
		load_gadget = Window::from_handle(dest_window, dest_icon, &window);
		load_object = window;
	}

	if (!load_object.null())
	{
		std::map<ObjectId, LoaderItem *>::iterator found = _loaders.find(load_object.handle());
		if (found != _loaders.end())
		{
			LoaderItem *item = found->second;
			ComponentId id = load_gadget.id();
			LoadEvent *event = new LoadEvent(load_object, load_gadget, msg[7], msg[8], msg[9], msg[10], msg.str(11), (msg.your_ref() == 0));
			bool done = false;
			while (item && !done)
			{
				if (item->id == id || item->id == NULL_ComponentId)
				{
					if (item->file_type == event->file_type() || item->file_type == -2)
					{
						done = item->loader->accept_file(*event);
						if (done)
						{
							msg_event.claim();
							_loading = new LoadingItem(item->loader);
							_loading->_reply_to = reply_to;
							_loading->_data_save_reply = 0;
							_loading->_buffer_size = event->estimated_size();
							_loading->_load_event = event;
							event = 0; // Loading now owns event
						}
					}
				}
				item = item->next;
			}

			delete event;
		}
	}
}

/**
 * Load file from disc from data load message
 *
 * @param msg the wimp message for the transfer.
 * @return true if file is loaded
 */
bool LoaderManager::load_file(const WimpMessage &msg)
{
	if (!_loading) return false;

	_loading->_load_event->update_file_details(msg.str(11), msg.word(9));

	bool done = _loading->_loader->load_file(*(_loading->_load_event));
	delete _loading;
	_loading = 0;

	return done;
}

/**
 * RAM Transmit message received so process bytes received
 */
void LoaderManager::ram_transmit(const WimpMessage &msg)
{
	WimpMessage rep(msg, true);
	rep.message_id(6); // RAMFetch
	rep.your_ref(msg.my_ref());
	bool reply = false;

	if (_loading)
	{
		DataReceivedEvent event(_loading->_load_event, (void*)msg[5], _loading->_buffer_size, msg[6]);
		if (_loading->_loader->data_received(event))
		{
			// Update data transfer sizes
			rep.word(5) = (int)event.buffer();
			rep.word(6) = _loading->_buffer_size = event.buffer_size();
			reply = event.more();
		}
	}
	if (reply)
	{
		rep.send(WimpMessage::Recorded, _loading->_reply_to);
		_loading->_my_ref = rep.my_ref();
	} else
	{
		delete _loading;
		_loading = 0;
	}
}

// Helper for clipboard support to send data directly to
// a loader by simulating a ram data transmit from another
// application.
void LoaderManager::send_local(int file_type, const char *data, int size, Object load_object, Gadget load_gadget, int x, int y)
{
	std::map<ObjectId, LoaderItem *>::iterator found = _loaders.find(load_object.handle());
	if (found != _loaders.end())
	{
		LoaderItem *item = found->second;
		LoaderItem *item_to_load = 0;
		ComponentId id = load_gadget.id();
		LoadEvent load_event(load_object, load_gadget, x, y, size, file_type, "local", false);
		while (item && !item_to_load)
		{
			if (item->id == id || item->id == NULL_ComponentId)
			{
				if (item->file_type == file_type || item->file_type == -2)
				{
					if (item->loader->accept_file(load_event))
					{
						item_to_load = item;
					}
				}
			}
			item = item->next;
		}

		if (item_to_load)
		{
			int recv_size = size;
			void *recv_buffer = item_to_load->loader->data_buffer(load_event, recv_size);
			if (recv_buffer)
			{
				int sent = 0;
				bool more = true;
				while (more)
				{
					int size_to_send = size-sent;
					if (size_to_send > recv_size) size_to_send = recv_size;
					else if (size_to_send < recv_size) more = false;
					if (size_to_send > 0) std::memcpy(recv_buffer, data, size_to_send);
					DataReceivedEvent recv_event(&load_event, recv_buffer, recv_size, size_to_send);
					if (item_to_load->loader->data_received(recv_event))
					{
						recv_buffer = recv_event.buffer();
						recv_size = recv_event.buffer_size();
						data += size_to_send;
						sent += size_to_send;
					} else
					{
						break; // Drop out of loop on error
					}
				}
			}
		}
	}
}

//! @endcond
}
