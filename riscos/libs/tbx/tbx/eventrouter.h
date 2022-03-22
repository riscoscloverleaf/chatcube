/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2012 Alan Buckley   All Rights Reserved.
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

#ifndef EVENTROUTER_H
#define EVENTROUTER_H

#include <map>
#include <vector>
#include <string>
#include "listener.h"
#include "autocreatelistener.h"
#include "pollinfo.h"

namespace tbx
{

// This class is internal to the workings of TBX
//! @cond INTERNAL

class Application;
class Object;
class Component;
class Window;
class WimpMessageListener;
class RedrawListener;
class DragHandler;
class Command;
class Timer;
class PrePollListener;
class PostPollListener;
class PostEventListener;
class UncaughtHandler;

/* Dummy window events to include component key presses/mouse click with those on the window */
const int WINDOW_AND_COMPONENT_KEY_PRESSED = 12;
const int WINDOW_AND_COMPONENT_MOUSE_CLICK = 13;

class EventRouter
{
public:
	EventRouter();
	~EventRouter();

	static EventRouter *instance() {return _instance;}

	void poll();
	void yield();

	// Turn on catching of uncaught exceptions (default to on)
	void catch_exceptions(bool c) {_catch_exceptions = c;}
	void uncaught_handler(UncaughtHandler *handler) {_uncaught_handler = handler;}

private:
	// Allow base listener adding classes access to low level listener adding routines
	friend class Application;
	friend class Object;
	friend class Component;
	friend class Window;

	// Toolbox events
	void add_autocreate_listener(const char *template_name, AutoCreateListener *listener);
	void remove_autocreate_listener(const char *template_name);

	void add_object_listener(ObjectId handle, ComponentId component_id, int action, Listener *listener, RawToolboxEventHandler handler);
	void remove_object_listener(ObjectId handle, ComponentId component_id, int action, Listener *listener);
	void set_object_handler(ObjectId handle, int action, Listener *listener, RawToolboxEventHandler handler);

	void add_window_event_listener(ObjectId handle, int event_code, Listener *listener);
	void remove_window_event_listener(ObjectId handle, int event_code, Listener *listener);
	void add_window_event_listener(ObjectId handle, ComponentId component_id, int event_code, Listener *listener);
	void remove_window_event_listener(ObjectId handle, ComponentId component_id, int event_code, Listener *listener);
	void add_window_component_event_listener(ObjectId handle, int event_code, Listener *listener);
	void remove_window_component_event_listener(ObjectId handle, int event_code, Listener *listener);

	void remove_all_listeners(ObjectId handle);
	void remove_all_listeners(ObjectId handle, ComponentId component_id);

	void set_autocreate_listener(std::string template_name, AutoCreateListener *listener);
	void clear_autocreate_listener(std::string template_name);

	void add_message_listener(int type, int message_id, WimpMessageListener *listener);
	void remove_message_listener(int type, int message_id, WimpMessageListener *listener);

	void add_null_event_command(Command *command);
	void remove_null_event_command(Command *command);

	void set_drag_handler(DragHandler *handler, int drag_stop_swi = 0);
	void cancel_drag();

	void add_timer(int elapsed, Timer *timer);
	void remove_timer(Timer *timer);

private:
	void route_event(int event_code);

	void process_toolbox_event();
	bool process_toolbox_event(ObjectId object_id, ComponentId comp_id);

	void process_null_event();
	void process_redraw_request();
	void process_open_window_request();
	void process_close_window_request();
	void process_pointer_leaving_window();
    void process_pointer_entering_window();
	void process_mouse_click();
	void process_key_pressed();
	void process_scroll_request();
	void process_lose_caret();
	void process_gain_caret();

	void process_user_message();
	void process_recorded_message();
	void process_acknowledge_message();

private:
	IdBlock _id_block;
	int _poll_mask;
	PollBlock _poll_block;
	int _reply_to;
	bool _catch_exceptions;
	UncaughtHandler *_uncaught_handler;

	PrePollListener *_pre_poll_listener;
	PostPollListener *_post_poll_listener;
	PostEventListener *_post_event_listener;


	// List item for object/component toolbox events
    struct ObjectListenerItem
	{
		int action;
		ComponentId component_id;
		RawToolboxEventHandler handler;
		Listener *listener;
		ObjectListenerItem *next;
	};

    // List item for WIMP window events
    struct WindowEventListenerItem
    {
    	Listener *listener;
    	WindowEventListenerItem *next;
    	ComponentId component_id;
    };

    // List item for wimp messages
    struct WimpMessageListenerItem
    {
    	int message_id;
    	WimpMessageListener *listener;
    	WimpMessageListenerItem *next;
    };


    // Running event delete helpers
    bool _remove_running;
    ObjectListenerItem *_running_object_item;
    WindowEventListenerItem *_running_window_event_item;
    WimpMessageListenerItem *_running_message_item;

	std::map<ObjectId, ObjectListenerItem *> _object_listeners;
	std::map<std::string, AutoCreateListener*> *_autocreate_listeners;
	std::map<int, WimpMessageListenerItem *> **_message_listeners;
	std::map<ObjectId, WindowEventListenerItem **> *_window_event_listeners;

	// Internal class to look after null events
	class NullCommandQueue
	{
		Command **_commands;
		unsigned int _capacity;
		unsigned int _size;
		unsigned int _next;
		unsigned int _end;
	public:
		NullCommandQueue();
		~NullCommandQueue();

		void add(Command *command);
		void remove(Command *command);
		Command *next();

		unsigned int size() const {return _size;}
	} *_null_event_commands;

	DragHandler *_drag_handler;
	int _drag_stop_swi;

	struct TimerInfo
	{
		unsigned int due;
		unsigned int elapsed;
		Timer *timer;
		TimerInfo *next;
	} *_first_timer;

private:
	// Listener list helpers
	ObjectListenerItem *find_first_object_listener(ObjectId handle, int action);
    WindowEventListenerItem *find_window_event_listener(ObjectId object_id, int event_code);
    WindowEventListenerItem *find_window_event_component(WindowEventListenerItem *&item, ComponentId component_id);
    void remove_running_window_event_listener(ObjectId object_id, int event_code);

    void add_timer_info(TimerInfo *info);

private:
	static EventRouter *_instance;
};


inline EventRouter *event_router() {return EventRouter::instance();}

}

//! @endcond

#endif
