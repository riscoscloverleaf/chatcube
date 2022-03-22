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

#ifndef TBX_LOADERMANAGER_H_
#define TBX_LOADERMANAGER_H_

#include "wimpmessagelistener.h"
#include "loader.h"
#include <map>

namespace tbx {

//! @cond INTERNAL
/*
 * Internal class to managed file/data loaders
 */

class LoaderManager:
	public tbx::WimpUserMessageListener,
	public tbx::WimpRecordedMessageListener,
	public tbx::WimpAcknowledgeMessageListener
{
private:
	static LoaderManager *_instance;

public:
	LoaderManager();
	virtual ~LoaderManager();

	static LoaderManager* instance() {return _instance;}

	virtual void user_message(WimpMessageEvent &event);
	virtual void recorded_message(WimpMessageEvent &event, int reply_to);
	virtual void acknowledge_message(WimpMessageEvent &event);

	void add_loader(ObjectId handle, ComponentId id, int file_type, Loader *loader);
	void remove_loader(ObjectId handle, ComponentId id, int file_type, Loader *loader);

	void remove_all_loaders(ObjectId handle);
	void remove_all_loaders(ObjectId handle, ComponentId id);

	// Clipboard support
	void paste_ref(int ref) {_paste_ref = ref;}
	void send_local(int file_type, const char *data, int size, Object load_object, Gadget load_gadget, int x, int y);
	// Allow temporary interception of loader messages.
	class MessageIntercept
	{
		public:
		   virtual ~MessageIntercept() {};
		   virtual bool loader_message_intercept(WimpMessage::SendType type, WimpMessageEvent &event, int reply_to) = 0;
	};
	void message_intercept(MessageIntercept *intercept) {_message_intercept = intercept;}
    MessageIntercept *message_intercept() const {return _message_intercept;}

private:
	void start_loader(WimpMessageEvent &msg_event, int reply_to);
	void process_dataload(WimpMessageEvent &event);
	bool load_file(const WimpMessage &msg);
	void ram_transmit(const WimpMessage &msg);
	void find_loading(WimpMessageEvent &msg_event, int reply_to);


	struct LoaderItem
	{
		ObjectId handle;
		ComponentId id;
		int file_type;
		Loader *loader;
		LoaderItem *next;
	};

	std::map<ObjectId, LoaderItem *> _loaders;

	struct LoadingItem
	{
		Loader *_loader;
		WimpMessage *_data_save_reply;
		int _reply_to;
		int _my_ref;
		int _buffer_size;
		LoadEvent *_load_event;

		LoadingItem(Loader *loader) : _loader(loader), _data_save_reply(0), _load_event(0) {};
		~LoadingItem() {delete _data_save_reply; delete _load_event;}
	} *_loading;

	int _paste_ref;
	MessageIntercept *_message_intercept;
};

//! @endcond

}

#endif /* LOADERMANAGER_H_ */
