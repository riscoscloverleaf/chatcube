/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2014 Alan Buckley   All Rights Reserved.
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

#include "eventrouter.h"
#include "application.h"
#include "reporterror.h"
#include "usereventids.h"
#include "component.h"
#include "wimpmessagelistener.h"
#include "redrawlistener.h"
#include "openwindowlistener.h"
#include "closewindowlistener.h"
#include "pointerlistener.h"
#include "mouseclicklistener.h"
#include "keylistener.h"
#include "draghandler.h"
#include "caretlistener.h"
#include "scrollrequestlistener.h"
#include "command.h"
#include "loadermanager.h"
#include "swixcheck.h"
#include "tbxexcept.h"
#include "monotonictime.h"
#include "timer.h"
#include "prepolllistener.h"
#include "postpolllistener.h"
#include "posteventlistener.h"
#include "uncaughthandler.h"

#include <cstring>
#include <kernel.h>
#include <swis.h>
#include <algorithm>

using namespace tbx;

// This class is internal to the workings of TBX
//! @cond INTERNAL

EventRouter *EventRouter::_instance = 0;

// Maximum window event in window event map
// Events 12 onwards are dummy events
const int MAX_WINDOW_EVENTS = 14;


namespace tbx
{
	EventRouter the_event_router;


	PollBlock::PollBlock(const PollBlock &other)
	{
		std::memcpy(word, other.word, 256);
	}

	PollBlock &PollBlock::operator=(const PollBlock &other)
	{
		std::memcpy(word, other.word, 256);
		return *this;
	}
}

EventRouter::EventRouter()
{
	_instance = this;
	_running_object_item = 0;
	_remove_running = false;
    _poll_mask = 1; // No Null events

    _autocreate_listeners = 0;
    _message_listeners = 0;
    _running_message_item = 0;

    _null_event_commands = 0;

    _drag_handler = 0;

    _first_timer = 0;

    _catch_exceptions = true;
    _uncaught_handler = 0;
    _pre_poll_listener = 0;
    _post_poll_listener = 0;
    _post_poll_listener = 0;
}

EventRouter::~EventRouter()
{
}

void EventRouter::poll()
{
    _kernel_swi_regs regs;
	int poll = Wimp_Poll;
    regs.r[0] = _poll_mask;
    regs.r[1] = reinterpret_cast<int>(&_poll_block);

    if ((_poll_mask & 1) && _first_timer)
    {
    	// Use PollIdle if we have a timer in the future.
    	regs.r[0] &= ~1; // Ensure null events are processed
    	unsigned int now = monotonic_time();
    	if (monotonic_lt(now, _first_timer->due))
    	{
    		poll = Wimp_PollIdle;
			regs.r[2] = _first_timer->due;
    	}
    }

    if (_pre_poll_listener)
    {
    	if (_pre_poll_listener->pre_poll())
    	{
    		// Return true if it want's idle events enabled
    		poll = Wimp_Poll;
    		regs.r[0] &= ~1;
    	}
    }

    if (_kernel_swi(poll, &regs, &regs) == 0)
	{
        // Reply task for User Message Acknowledge (eventCode == 18)
    	_reply_to = regs.r[2];

        if (_post_poll_listener)
        {
           _post_poll_listener->post_poll(regs.r[0], _poll_block, _id_block, _reply_to);
        }

    	if (_catch_exceptions)
    	{
    	    int r0 = regs.r[0];
			try
			{
				route_event(regs.r[0]);
			} catch(std::exception &e)
			{
				if (_uncaught_handler) _uncaught_handler->uncaught_exception(&e, r0);
				else report_error(e.what(), "Uncaught Exception");
			} catch(...)
			{
				if (_uncaught_handler) _uncaught_handler->uncaught_exception(0, r0);
				else report_error("Uncaught exception");
			}
    	} else
    	{
    		route_event(regs.r[0]);
    	}
    	if (_post_event_listener)
    	{
    		_post_event_listener->post_event(regs.r[0], _poll_block, _id_block, _reply_to);
    	}
	}
}

// Save the contents of a poll block, restoring when this goes out of scope.
struct SavePollBlock
{
private:
	tbx::PollBlock _saved;
	tbx::PollBlock *_block;
public:
	SavePollBlock(tbx::PollBlock *block) : _saved(*block), _block(block) {}
	~SavePollBlock() {*_block = _saved;}
};

void EventRouter::yield()
{
    _kernel_swi_regs regs;
	int poll = Wimp_Poll;
    regs.r[1] = reinterpret_cast<int>(&_poll_block);

    SavePollBlock spb(&_poll_block);

	_poll_mask &= ~1; // Turn on null event processing
	bool more_events = true;
	while (app()->running() && more_events)
	{
	    regs.r[0] = (_poll_mask & ~1); // Always return for null events

		if (_kernel_swi(poll, &regs, &regs) == 0)
		{
			// Reply task for User Message Acknowledge (eventCode == 18)
			_reply_to = regs.r[2];

			if (_post_poll_listener)
			{
			   _post_poll_listener->post_poll(regs.r[0], _poll_block, _id_block, _reply_to);
			}

			more_events = (regs.r[0] != 0);
			route_event(regs.r[0]);
		}
	}
}


void EventRouter::route_event(int event_code)
{
	switch(event_code)
	{
		case 0x200: process_toolbox_event(); break;

		case 0: process_null_event(); break;

		case 1: process_redraw_request(); break;
		case 2: process_open_window_request(); break;
		case 3: process_close_window_request(); break;
		case 4: process_pointer_leaving_window(); break;
		case 5: process_pointer_entering_window(); break;
		case 6: process_mouse_click(); break;

		case 7: // Drag finished
			if (_drag_handler)
			{
				if (_drag_stop_swi) _swix(_drag_stop_swi, 0);
				BBox dest(_poll_block.word[0], _poll_block.word[1],
						_poll_block.word[2], _poll_block.word[3]);
				_drag_handler->drag_finished(dest);
				_drag_handler = 0;
			}
		break;

		case 8: // Key_Pressed
			process_key_pressed();
		break;

		case 9: // TODO: Menu_Selection
		// No window handle so we need to route it via the application
		break;

		case 10: process_scroll_request(); break;
		case 11: process_lose_caret(); break;
		case 12: process_gain_caret(); break;

		case 17: process_user_message(); break;
		case 18: process_recorded_message(); break;
		case 19: process_acknowledge_message(); break;

		default:
		// Others require no special processing
		//TODO:	RouteWimpEvent(eventCode);
		break;
	}
}

void EventRouter::process_toolbox_event()
{
	int action = _poll_block.word[2];
	bool handled = false;

	switch (action)
	{
	case 0x44EC0: // Toolbox error
		{
			//TODO: Allow error handler to be set
			_kernel_oserror err;
			err.errnum = _poll_block.word[4];
			std::strcpy(err.errmess, reinterpret_cast<char *>(&_poll_block.word[5]));
			report_error(&err,0);
		}
		break;

	case 0x44EC1: // Auto Create
		if (_autocreate_listeners != 0)
		{
			char *template_name = reinterpret_cast<char *>(&(_poll_block.word[4]));
			std::map<std::string, AutoCreateListener *>::iterator found = _autocreate_listeners->find(template_name);
			if (found != _autocreate_listeners->end())
			{
				found->second->auto_created(template_name, _id_block.self_object());
			}
		}
		break;

	case 0x44EC2: // Auto deleted event - automatically removes listeners
		process_toolbox_event(_id_block.self_object_id, _id_block.self_component_id);
		remove_all_listeners(_id_block.self_object_id);
		if (LoaderManager::instance())
		{
			LoaderManager::instance()->remove_all_loaders(_id_block.self_object_id);
		}
		break;

	case 0x82a91: // Quit_Quit - always quits application
		process_toolbox_event(_id_block.self_object_id, _id_block.self_component_id);
		app()->quit();
		break;

	default:
		handled = process_toolbox_event(_id_block.self_object_id, _id_block.self_component_id);

		if (action >= FIRST_USER_EID && action <= LAST_USER_EID)
		{
			// User events are also passed to parent and ancestor objects and application
			if (!handled
					&& _id_block.parent_object_id != NULL_ObjectId
					&& _id_block.parent_object_id != _id_block.self_object_id)
			{
				handled = process_toolbox_event(_id_block.parent_object_id, _id_block.parent_component_id);
			}

			if (!handled
			    && _id_block.ancestor_object_id != NULL_ObjectId
				&& _id_block.ancestor_object_id != _id_block.parent_object_id
				&& _id_block.ancestor_object_id != _id_block.self_object_id)
			{
				handled = process_toolbox_event(_id_block.ancestor_object_id, _id_block.ancestor_component_id);
			}

			if (!handled)
			{
				handled = process_toolbox_event(NULL_ObjectId, NULL_ComponentId);
			}
		}
	}
}

bool EventRouter::process_toolbox_event(ObjectId object_id, ComponentId comp_id)
{
	int action = _poll_block.word[2];
	bool handled = false;
	ObjectListenerItem *item = find_first_object_listener(object_id, action);

	while (item && item->action == action)
	{
		if (item->component_id == comp_id || item->component_id == NULL_ComponentId)
		{
			_running_object_item = item;
			handled = true;
			(*item->handler)(_id_block, _poll_block, item->listener);
			item = item->next;
			if (_remove_running)
			{
				ObjectListenerItem *remove_item = _running_object_item;
				_remove_running = false;
				_running_object_item = 0;
				remove_object_listener(object_id, remove_item->component_id, remove_item->action, remove_item->listener);
			} else
				_running_object_item = 0;
		} else
			item = item->next;
	}

	return handled;
}

EventRouter::ObjectListenerItem *EventRouter::find_first_object_listener(ObjectId handle, int action)
{
	std::map<ObjectId, ObjectListenerItem *>::iterator found = _object_listeners.find(handle);
	ObjectListenerItem *item = 0;

	if (found != _object_listeners.end())
	{
		item = found->second;
		while (item && action != item->action)
		{
			item = item->next;
		}
	}

	return item;
}

void EventRouter::add_object_listener(ObjectId handle, ComponentId component_id, int action, Listener *listener, RawToolboxEventHandler handler)
{
	ObjectListenerItem *new_item = new ObjectListenerItem;
	new_item->component_id = component_id;
	new_item->action = action;
	new_item->listener = listener;
	new_item->handler = handler;
	new_item->next = 0;

	std::map<ObjectId, ObjectListenerItem *>::iterator found = _object_listeners.find(handle);
	if (found == _object_listeners.end())
	{
		_object_listeners[handle] = new_item;
	} else
	{
		ObjectListenerItem *item = found->second;
		ObjectListenerItem *prev = 0;
		while (item && item->action <= action)
		{
			prev = item;
			item = item->next;
		}
		if (item == 0)
		{
			prev->next = new_item;
		} else if (prev == 0)
		{
			new_item->next = item;
			_object_listeners[handle] = new_item;
		} else
		{
			new_item->next = item;
			prev->next = new_item;
		}
	}
}

void EventRouter::remove_object_listener(ObjectId handle, ComponentId component_id, int action, Listener *listener)
{
	std::map<ObjectId, ObjectListenerItem *>::iterator found = _object_listeners.find(handle);
	if (found != _object_listeners.end())
	{
		ObjectListenerItem *item = found->second;
		ObjectListenerItem *prev = 0;

        while (item && item->action < action)
		{
			prev = item;
			item = item->next;
		}
		while (item && item->action == action
				&& (item->listener != listener || item->component_id != component_id))
		{
			prev = item;
			item = item->next;
		}
		if (item && item->action == action && item->listener == listener && item->component_id == component_id)
		{
			if (item == _running_object_item) _remove_running = true;
			else
			{
				if (prev == 0)
				{
					if (item->next == 0)
					{
						_object_listeners.erase(handle);
					} else
					{
						_object_listeners[handle] = item->next;
					}
				} else
				{
					prev->next = item->next;
					item->next = 0;
				}
				delete item;
			}
		}
	}
}

/**
 * Handler is for an event that logically should not have more than one listener
 */
void EventRouter::set_object_handler(ObjectId handle, int action, Listener *listener, RawToolboxEventHandler handler)
{
	ObjectListenerItem *item = find_first_object_listener(handle, action);
	if (item == 0)
	{
		if (listener != 0)
		{
			add_object_listener(handle, NULL_ComponentId, action, listener, handler);
		}
	} else if (listener == 0)
	{
		remove_object_listener(handle, NULL_ComponentId, action, item->listener);
	} else
	{
		item->listener = listener;
		item->handler = handler;
	}
}

void EventRouter::remove_all_listeners(ObjectId handle)
{
	std::map<ObjectId, ObjectListenerItem *>::iterator found = _object_listeners.find(handle);
	if (found != _object_listeners.end())
	{
		ObjectListenerItem *item = found->second;
		ObjectListenerItem *running = 0;
		while (item)
		{
			ObjectListenerItem *next = item->next;
			if (item == _running_object_item)
			{
				item->next = 0;
				running = item;
				_remove_running = true;
			} else
				delete item;
			item = next;
		}

		if (running)
		{
			_object_listeners[handle] = running;
		} else
			_object_listeners.erase(handle);
	}

	if (_window_event_listeners)
	{
		std::map<ObjectId, WindowEventListenerItem **>::iterator found = _window_event_listeners->find(handle);
		if (found != _window_event_listeners->end())
		{
			bool delete_all = true;
			for (int j = 0; j < MAX_WINDOW_EVENTS; j++)
			{
				WindowEventListenerItem *item = found->second[j];
				WindowEventListenerItem *running = 0;
				while (item)
				{
					WindowEventListenerItem *next = item->next;
					if (item == _running_window_event_item)
					{
						item->next = 0;
						running = item;
						_remove_running = true;
						delete_all = false;
					} else
						delete item;
					item = next;
				}
				found->second[j] = running;
			}
			if (delete_all)
			{
				delete [] found->second;
				_window_event_listeners->erase(found);
			}
		}
	}
}

void EventRouter::remove_all_listeners(ObjectId handle, ComponentId component_id)
{
	std::map<ObjectId, ObjectListenerItem *>::iterator found = _object_listeners.find(handle);
	if (found != _object_listeners.end())
	{
		ObjectListenerItem *old_first = found->second;
		ObjectListenerItem *item = old_first;
		ObjectListenerItem *prev = 0;
		ObjectListenerItem *first = 0;

		while (item)
		{
			ObjectListenerItem *next = item->next;
			if (item->component_id == component_id)
			{
				if (item == _running_object_item)
				{
					if (first == 0) first = item;
					prev = item;
					_remove_running = true;
				} else
				{
					if (prev) prev->next = next;
					delete item;
				}
			} else
			{
				if (first == 0) first = item;
				prev = item;
			}

			item = next;
		}

		if (first)
		{
			if (first != old_first)
			{
				_object_listeners[handle] = first;
			}
		} else
			_object_listeners.erase(handle);
	}

	if (_window_event_listeners)
	{
		std::map<ObjectId, WindowEventListenerItem **>::iterator found = _window_event_listeners->find(handle);
		if (found != _window_event_listeners->end())
		{
			bool delete_all = true;
			// Just need to remove mouse_click 5 (6-1) and key press 8 (7-1)
			for (int j = 5; j < 8; j+=2)
			{
				WindowEventListenerItem *item = found->second[j];
				WindowEventListenerItem *prev = find_window_event_component(item, component_id);

				while (item && item->component_id == component_id)
				{
					WindowEventListenerItem *next = item->next;
					if (item == _running_window_event_item)
					{
						_remove_running = true;
						prev = item;
						delete_all = false;
					} else
					{
						if (prev) prev->next = next;
						else found->second[j] = next;
						delete item;
					}
					item = next;
				}
			}
			for (int j = 0; j < MAX_WINDOW_EVENTS && delete_all; j++)
			{
				if (found->second[j] != 0) delete_all = false;
			}
			if (delete_all)
			{
				delete [] found->second;
				_window_event_listeners->erase(found);
			}
		}
	}
}


void EventRouter::set_autocreate_listener(std::string template_name, AutoCreateListener *listener)
{
	if (_autocreate_listeners == 0) _autocreate_listeners = new std::map<std::string, AutoCreateListener*>();
	(*_autocreate_listeners)[template_name] = listener;
}

void EventRouter::clear_autocreate_listener(std::string template_name)
{
	if (_autocreate_listeners != 0)
	{
		std::map<std::string, AutoCreateListener *>::iterator found = _autocreate_listeners->find(template_name);
		if (found != _autocreate_listeners->end()) _autocreate_listeners->erase(found);
	}
}


/**
 * Run all the null event commands
 */
void EventRouter::process_null_event()
{
	if (_null_event_commands)
	{
		Command *to_run;
		while (_null_event_commands
				&& ((to_run = _null_event_commands->next()) != 0))
		{
			to_run->execute();
		}
	}

	// Process timers
	if (_first_timer)
	{
		unsigned int now = monotonic_time();
		unsigned int actual = now; // take into account long timer routines
		while (_first_timer && monotonic_ge(now, _first_timer->due))
		{
			TimerInfo *running = _first_timer;
			unsigned int diff = monotonic_elapsed(_first_timer->due, actual);
			_first_timer->timer->timer(diff);
			if (running == _first_timer)
			{
				// Update due time for this timer
				while (diff > _first_timer->elapsed)
				{
					diff -= _first_timer->elapsed;
					_first_timer->due += _first_timer->elapsed;
				}
				_first_timer->due += _first_timer->elapsed;
				if (_first_timer->next != 0)
				{
					_first_timer = _first_timer->next;
					// Re-add in correct order
					add_timer_info(running);
					actual = monotonic_time();
				}
			}
		}
	}
}

/*
 * Process window redraw requests
 * for each rectangle calls all the registered listeners
 */
void EventRouter::process_redraw_request()
{
	WindowEventListenerItem *item = find_window_event_listener(_id_block.self_object_id, 1);

	int more;
	if (_swix(Wimp_RedrawWindow, _IN(1)|_OUT(0), &_poll_block, &more) != 0) more = 0;

	if (item)
	{
		WindowEventListenerItem *redraw;
		while (more)
		{
			RedrawEvent e(_id_block, _poll_block);
			redraw = item;
			while(redraw)
			{
				_running_window_event_item = redraw;
				static_cast<RedrawListener *>(redraw->listener)->redraw(e);
				redraw = redraw->next;
				if (_remove_running)
				{
					if (item == _running_window_event_item) item = redraw;
					remove_running_window_event_listener(_id_block.self_object_id, 2);
				}
			}
			if (_swix(Wimp_GetRectangle, _IN(1)|_OUT(0), &_poll_block, &more) != 0) more = 0;
		}
		_running_window_event_item = 0;

	} else
	{
		// No registered redrawers so just run through process
		while (more)
		{
			if (_swix(Wimp_GetRectangle, _IN(1)|_OUT(0), &_poll_block, &more) != 0) more = 0;
		}
	}
}

/*
 * Process OpenWindowRequest from the Wimp.
 * calls Wimp_OpenWindow after calling all listeners.
 */
void EventRouter::process_open_window_request()
{
	WindowEventListenerItem *item = find_window_event_listener(_id_block.self_object_id, 2);

	if (item)
	{
		OpenWindowEvent ev(_id_block, _poll_block);
		while (item)
		{
			_running_window_event_item = item;
			static_cast<OpenWindowListener *>(item->listener)->open_window(ev);
			item = item->next;
			if (_remove_running) remove_running_window_event_listener(_id_block.self_object_id, 2);
		}
		_running_window_event_item = 0;
	}

	_swix(Wimp_OpenWindow, _IN(1), &_poll_block);
}

/**
 * Process close window request from WIMP
 */
void EventRouter::process_close_window_request()
{
	WindowEventListenerItem *item = find_window_event_listener(_id_block.self_object_id, 3);

	if (item)
	{
		EventInfo ev(_id_block, _poll_block);
		while (item)
		{
			_running_window_event_item = item;
			static_cast<CloseWindowListener *>(item->listener)->close_window(ev);
			item = item->next;
			if (_remove_running) remove_running_window_event_listener(_id_block.self_object_id, 3);
		}
		_running_window_event_item = 0;
	}
}

/**
 * Process pointer leaving window request from WIMP
 */
void EventRouter::process_pointer_leaving_window()
{
	WindowEventListenerItem *item = find_window_event_listener(_id_block.self_object_id, 4);

	if (item)
	{
		EventInfo ev(_id_block, _poll_block);
		while (item)
		{
			_running_window_event_item = item;
			static_cast<PointerLeavingListener *>(item->listener)->pointer_leaving(ev);
			item = item->next;
			if (_remove_running) remove_running_window_event_listener(_id_block.self_object_id, 4);
		}
		_running_window_event_item = 0;
	}
}

/**
 * Process pointer entering window request from WIMP
 */
void EventRouter::process_pointer_entering_window()
{
	WindowEventListenerItem *item = find_window_event_listener(_id_block.self_object_id, 5);

	if (item)
	{
		EventInfo ev(_id_block, _poll_block);
		while (item)
		{
			_running_window_event_item = item;
			static_cast<PointerEnteringListener *>(item->listener)->pointer_entering(ev);
			item = item->next;
			if (_remove_running) remove_running_window_event_listener(_id_block.self_object_id, 5);
		}
		_running_window_event_item = 0;
	}
}

/*
 * Process Mouse Click from the WIMP
 */
void EventRouter::process_mouse_click()
{
	WindowEventListenerItem *item = find_window_event_listener(_id_block.self_object_id, 6);
	WindowEventListenerItem *all_item = find_window_event_listener(_id_block.self_object_id, WINDOW_AND_COMPONENT_MOUSE_CLICK);
	if (item) find_window_event_component(item, _id_block.self_component_id);
	if (item || all_item)
	{
		Window win(_id_block.self_object_id);
		Point pt(_poll_block.word[0], _poll_block.word[1]);
		// Get flag to see if mouse click can include double clicks
		ButtonType type = BUTTONTYPE_CLICK_ONCE;
		if (_poll_block.word[2])
		{
			if (_poll_block.word[4] != -1)
			{
				// Click on an icon
				int icon_info[10];
				icon_info[0] = _poll_block.word[3];
				icon_info[1] = _poll_block.word[4];
				if (_swix(Wimp_GetIconState, _IN(1), icon_info) == 0)
				{
				    type = ButtonType((icon_info[6]>>12)&15);
				}
			} else
			{
				WindowInfo info;
				win.get_info(info);
				type = info.button_type();
			}
		}

		MouseClickEvent ev(_id_block, _poll_block,
				(type == BUTTONTYPE_DOUBLE
				|| type == BUTTONTYPE_DOUBLE_DRAG
				|| type == BUTTONTYPE_DOUBLE_CLICK_DRAG)
		);
		while (item && item->component_id == _id_block.self_component_id)
		{
			_running_window_event_item = item;
			static_cast<MouseClickListener *>(item->listener)->mouse_click(ev);
			item = item->next;
			if (_remove_running) remove_running_window_event_listener(_id_block.self_object_id, 6);
		}
		while (all_item)
		{
			_running_window_event_item = all_item;
			static_cast<MouseClickListener *>(all_item->listener)->mouse_click(ev);
			all_item = all_item->next;
			if (_remove_running) remove_running_window_event_listener(_id_block.self_object_id, WINDOW_AND_COMPONENT_MOUSE_CLICK);
		}
		_running_window_event_item = 0;
	}
}

/*
 * Process Key pressed from the WIMP
 */
void EventRouter::process_key_pressed()
{
	WindowEventListenerItem *item = find_window_event_listener(_id_block.self_object_id, 8);
	bool used = false;

	if (item) find_window_event_component(item, _id_block.self_component_id);
	if (item)
	{
		KeyEvent ev(_id_block, _poll_block);

		while (!used && item && item->component_id == _id_block.self_component_id)
		{
			_running_window_event_item = item;
			static_cast<KeyListener *>(item->listener)->key(ev);
			item = item->next;
			if (_remove_running) remove_running_window_event_listener(_id_block.self_object_id, 6);
			used = ev.is_key_used();
		}
		_running_window_event_item = 0;
	}
	if (!used)
	{
		item = find_window_event_listener(_id_block.self_object_id, WINDOW_AND_COMPONENT_KEY_PRESSED);
		if (item)
		{
			KeyEvent ev(_id_block, _poll_block);
			while (!used && item)
			{
				_running_window_event_item = item;
				static_cast<KeyListener *>(item->listener)->key(ev);
				item = item->next;
				if (_remove_running) remove_running_window_event_listener(_id_block.self_object_id, 6);
				used = ev.is_key_used();
			}
			_running_window_event_item = 0;
		}
	}

	// Pass on if not used
	if (!used)
	{
		_swi(Wimp_ProcessKey, _IN(0), _poll_block.word[6]);
	}
}

/**
 * Proecss scroll request event
 */
void EventRouter::process_scroll_request()
{
	WindowEventListenerItem *item = find_window_event_listener(_id_block.self_object_id, 10);
	if (item)
	{
		ScrollRequestEvent event(_id_block, _poll_block);
		while (item)
		{
			_running_window_event_item = item;
			static_cast<ScrollRequestListener *>(item->listener)->scroll_request(event);
			item = item->next;
			if (_remove_running) remove_running_window_event_listener(_id_block.self_object_id, 11);
		}
		_running_window_event_item = 0;
	}

}

/**
 * Process lose caret event
 */
void EventRouter::process_lose_caret()
{
	WindowEventListenerItem *item = find_window_event_listener(_id_block.self_object_id, 11);

	if (item)
	{
		CaretEvent ev(_id_block, _poll_block);
		while (item)
		{
			_running_window_event_item = item;
			static_cast<LoseCaretListener *>(item->listener)->lose_caret(ev);
			item = item->next;
			if (_remove_running) remove_running_window_event_listener(_id_block.self_object_id, 11);
		}
		_running_window_event_item = 0;
	}
}

/**
 * Process gain caret event
 */
void EventRouter::process_gain_caret()
{
	WindowEventListenerItem *item = find_window_event_listener(_id_block.self_object_id, 12);

	if (item)
	{
		CaretEvent ev(_id_block, _poll_block);

		while (item)
		{
			_running_window_event_item = item;
			static_cast<GainCaretListener *>(item->listener)->gain_caret(ev);
			item = item->next;
			if (_remove_running) remove_running_window_event_listener(_id_block.self_object_id, 12);
		}
		_running_window_event_item = 0;
	}
}


void EventRouter::remove_running_window_event_listener(ObjectId object_id, int event_code)
{
	WindowEventListenerItem *remove_item = _running_window_event_item;
	_remove_running = false;
	_running_window_event_item = 0;
	remove_window_event_listener(object_id, remove_item->component_id, event_code, remove_item->listener);
}

/*
 * Finds first event listener item for an object and event
 */
EventRouter::WindowEventListenerItem *EventRouter::find_window_event_listener(ObjectId object_id, int event_code)
{
    WindowEventListenerItem *item = 0;
    if (_window_event_listeners != 0)
    {
       std::map<ObjectId, WindowEventListenerItem **>::iterator found = _window_event_listeners->find(object_id);
       if (found != _window_event_listeners->end())
       {
            item = found->second[event_code-1];
       }
    }

    return item;
}

// Add main window WIMP event listener
void EventRouter::add_window_event_listener(ObjectId object_id, int event_code, Listener *listener)
{
    WindowEventListenerItem *new_item = new WindowEventListenerItem();
    new_item->listener = listener;
    new_item->next = 0;
    new_item->component_id = NULL_ComponentId;

    if (_window_event_listeners == 0) _window_event_listeners = new std::map<ObjectId, WindowEventListenerItem **>();

    std::map<ObjectId, WindowEventListenerItem **>::iterator found = _window_event_listeners->find(object_id);
    if (found == _window_event_listeners->end())
    {
    	WindowEventListenerItem **items = new WindowEventListenerItem*[MAX_WINDOW_EVENTS];
    	for (int j = 0; j < MAX_WINDOW_EVENTS; j++)
    		items[j] = 0;

    	items[event_code - 1] = new_item;
    	(*_window_event_listeners)[object_id] = items;
    } else
    {
    	WindowEventListenerItem *item = found->second[event_code - 1];
    	WindowEventListenerItem *prev = 0;
    	while (item)
    	{
    		prev = item;
    		item = item->next;
    	}
    	if (prev)
    	{
    		prev->next = new_item;
    	} else
    	{
    		found->second[event_code - 1] = new_item;
    	}
    }
}

/*
 * Remove main window event listener.
 * If the event is currently the event being run it will mark it to be deleted
 * in the event process instead.
 */
void EventRouter::remove_window_event_listener(ObjectId object_id, int event_code, Listener *listener)
{
    if (_window_event_listeners != 0)
    {
       std::map<ObjectId, WindowEventListenerItem **>::iterator found = _window_event_listeners->find(object_id);
       if (found != _window_event_listeners->end())
       {
    	   WindowEventListenerItem *item = found->second[event_code-1];
    	   WindowEventListenerItem *prev = 0;
    	   while (item && (item->listener != listener || item->component_id != NULL_ComponentId))
    	   {
    		   prev = item;
    		   item = item->next;
    	   }
		   if (item)
		   {
				if (item == _running_window_event_item) _remove_running = true;
				else
				{
					if (prev == 0) found->second[event_code-1] = item->next;
					else prev->next = item->next;

					if (prev == 0 && item->next == 0)
					{
						// All of this type of listeners have been deleted
						// so check if there are any other events
						bool delete_all = true;
						for (int j = 0; j < MAX_WINDOW_EVENTS && delete_all; j++)
							if (found->second[j]) delete_all = false;
						if (delete_all)
						{
							delete [] found->second;
							_window_event_listeners->erase(found);
						}
					}

					delete item;
	    	   }
		   }
	   }
    }
}

// Add Component WIMP event listener
void EventRouter::add_window_event_listener(ObjectId object_id, ComponentId component_id, int event_code, Listener *listener)
{
    WindowEventListenerItem *new_item = new WindowEventListenerItem();
    new_item->listener = listener;
    new_item->next = 0;
    new_item->component_id = component_id;

    if (_window_event_listeners == 0) _window_event_listeners = new std::map<ObjectId, WindowEventListenerItem **>();

    std::map<ObjectId, WindowEventListenerItem **>::iterator found = _window_event_listeners->find(object_id);
    if (found == _window_event_listeners->end())
    {
    	WindowEventListenerItem **items = new WindowEventListenerItem*[MAX_WINDOW_EVENTS];
    	for (int j = 0; j < MAX_WINDOW_EVENTS; j++)
    		items[j] = 0;

    	items[event_code - 1] = new_item;
    	(*_window_event_listeners)[object_id] = items;
    } else
    {
    	WindowEventListenerItem *item = found->second[event_code - 1];
    	WindowEventListenerItem *prev = 0;
    	while (item && item->component_id != -1
    			&& item->component_id <= component_id)
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
    		found->second[event_code - 1] = new_item;
    	}
    }
}

/**
 * Find the Window event listener item for a component
 *
 * @param item first item in list to search - updated to found item or null
 * @param component_id id to find
 * @returns previous item to found item
 */
EventRouter::WindowEventListenerItem *EventRouter::find_window_event_component(WindowEventListenerItem *&item, ComponentId component_id)
{
	WindowEventListenerItem *prev = 0;
    if (component_id == -1)
	{
		while (item && item->component_id != component_id)
		{
			prev = item;
			item = item->next;
		}
	} else
	{
		while (item
				&& item->component_id != -1
				&& item->component_id < component_id)
		{
			prev = item;
			item = item->next;
		}
		if (item && item->component_id != component_id) item = 0;
	}

	return prev;
}

/*
 * Remove component WIMP event listener.
 * If the event is currently the event being run it will mark it to be deleted
 * in the event process instead.
 */
void EventRouter::remove_window_event_listener(ObjectId object_id, ComponentId component_id, int event_code, Listener *listener)
{
    if (_window_event_listeners != 0)
    {
       std::map<ObjectId, WindowEventListenerItem **>::iterator found = _window_event_listeners->find(object_id);
       if (found != _window_event_listeners->end())
       {
    	   WindowEventListenerItem *item = found->second[event_code-1];
    	   WindowEventListenerItem *prev = find_window_event_component(item, component_id);

    	   while (item && item->component_id == component_id
    			   && item->listener != listener)
    	   {
    		   prev = item;
    		   item = item->next;
    	   }

    	   if (item && item->component_id == component_id && item->listener == listener)
		   {
				if (item == _running_window_event_item) _remove_running = true;
				else if (item != 0)
				{
					if (prev == 0) found->second[event_code-1] = item->next;
					else prev->next = item->next;

					if (prev == 0 && item->next == 0)
					{
						// All of this type of listeners have been deleted
						// so check if there are any other events
						bool delete_all = true;
						for (int j = 0; j < MAX_WINDOW_EVENTS && delete_all; j++)
							if (found->second[j]) delete_all = false;
						if (delete_all)
						{
							delete [] found->second;
							_window_event_listeners->erase(found);
						}
					}

					delete item;
				}
		   }
       }
    }
}

void EventRouter::add_null_event_command(Command *command)
{
	if (_null_event_commands == 0)
	{
		_null_event_commands = new NullCommandQueue();
		_poll_mask &= ~1; // Turn on null event processing
	}
	_null_event_commands->add(command);
}

void EventRouter::remove_null_event_command(Command *command)
{
	if (_null_event_commands)
	{
		_null_event_commands->remove(command);
		if (_null_event_commands->size() == 0)
		{
			delete _null_event_commands;
			_null_event_commands = 0;
			_poll_mask |= 1; // Stop null events being reported
		}
	}
}

void EventRouter::process_user_message()
{
	int message_id = _poll_block.word[4];

	if (_message_listeners != 0 && _message_listeners[0] != 0)
	{
		std::map<int, WimpMessageListenerItem *>::iterator found = _message_listeners[0]->find(message_id);
		if (found != _message_listeners[0]->end())
		{
			WimpMessageListenerItem *item = found->second;
			WimpMessageEvent event(_poll_block);

			while (item && !event.claimed())
			{
				_running_message_item = item;
				static_cast<WimpUserMessageListener *>(item->listener)->user_message(event);
				item = item->next;

				if (_remove_running)
				{
					WimpMessageListenerItem *remove_item = _running_message_item;
					_remove_running = false;
					_running_message_item = 0;
					remove_message_listener(17, message_id, remove_item->listener);
				} else
				{
					_running_message_item = 0;
				}
			}
		}
	}

	// Must always quit on message code 0
	if (message_id == 0) app()->quit();
}

void EventRouter::process_recorded_message()
{
	int message_id = _poll_block.word[4];

	if (_message_listeners != 0 && _message_listeners[1] != 0)
	{
		std::map<int, WimpMessageListenerItem *>::iterator found = _message_listeners[1]->find(message_id);
		if (found != _message_listeners[1]->end())
		{
			WimpMessageListenerItem *item = found->second;
			WimpMessageEvent event(_poll_block);

			while (item && !event.claimed())
			{
				_running_message_item = item;
				static_cast<WimpRecordedMessageListener *>(item->listener)->recorded_message(event, _reply_to);
				item = item->next;

				if (_remove_running)
				{
					WimpMessageListenerItem *remove_item = _running_message_item;
					_remove_running = false;
					_running_message_item = 0;
					remove_message_listener(18, message_id, remove_item->listener);
				} else
				{
					_running_message_item = 0;
				}
			}
		}
	}

	// Must always quit on message code 0
	if (message_id == 0) app()->quit();
}

void EventRouter::process_acknowledge_message()
{
	int message_id = _poll_block.word[4];

	if (_message_listeners != 0 && _message_listeners[2] != 0)
	{
		std::map<int, WimpMessageListenerItem *>::iterator found = _message_listeners[2]->find(message_id);
		if (found != _message_listeners[2]->end())
		{
			WimpMessageListenerItem *item = found->second;
			WimpMessageEvent event(_poll_block);

			while (item && !event.claimed())
			{
				_running_message_item = item;
				static_cast<WimpAcknowledgeMessageListener *>(item->listener)->acknowledge_message(event);

				item = item->next;

				if (_remove_running)
				{
					WimpMessageListenerItem *remove_item = _running_message_item;
					_remove_running = false;
					_running_message_item = 0;
					remove_message_listener(19, message_id, remove_item->listener);
				} else
				{
					_running_message_item = 0;
				}
			}
		}
	}
}


void EventRouter::add_message_listener(int type, int message_id, WimpMessageListener *listener)
{
	if (_message_listeners == 0)
	{
		_message_listeners = new std::map<int, WimpMessageListenerItem *>*[3];
		_message_listeners[0] = _message_listeners[1] = _message_listeners[2] = 0;
	}
	type -= 17;
	if (_message_listeners[type] == 0) _message_listeners[type] = new std::map<int, WimpMessageListenerItem *>();

	WimpMessageListenerItem *new_item = new WimpMessageListenerItem;
	new_item->listener = listener;
	new_item->next = 0;

	std::map<int, WimpMessageListenerItem *>::iterator found = _message_listeners[type]->find(message_id);
	if (found == _message_listeners[type]->end())
	{
		(*_message_listeners[type])[message_id] = new_item;
	} else
	{
		WimpMessageListenerItem *item = found->second;
		WimpMessageListenerItem *prev = 0;
		while (item && item->message_id <= message_id)
		{
			prev = item;
			item = item->next;
		}
		if (item == 0)
		{
			prev->next = new_item;
		} else if (prev == 0)
		{
			new_item->next = item;
			(*_message_listeners[type])[message_id] = new_item;
		} else
		{
			new_item->next = item;
			prev->next = new_item;
		}
	}

}

void EventRouter::remove_message_listener(int type, int message_id, WimpMessageListener *listener)
{
	if (_message_listeners == 0) return;
	type -= 17;
	if (_message_listeners[type] == 0) return;

	std::map<int, WimpMessageListenerItem *>::iterator found = _message_listeners[type]->find(message_id);
	if (found != _message_listeners[type]->end())
	{
		WimpMessageListenerItem *item = found->second;
		WimpMessageListenerItem *prev = 0;

		while (item && item->listener != listener)
		{
			prev = item;
			item = item->next;
		}
		if (item && item->listener == listener)
		{
			if (item == _running_message_item) _remove_running = true;
			else
			{
				if (prev == 0)
				{
					if (item->next == 0)
					{
						_message_listeners[type]->erase(message_id);
					} else
					{
						(*_message_listeners[type])[message_id] = item->next;
					}
				} else
				{
					prev->next = item->next;
					item->next = 0;
				}
				delete item;
			}
		}
	}
}

/**
 * Set the handler for the next/current drag
 *
 * @param handler to call when drag finishes
 * @param drag_drop_swi - switch to call (with no params) when drag ends or 0 for none.
 * e.g. DragASprite_Stop, DragAnObject_Stop
 */
void EventRouter::set_drag_handler(DragHandler *handler, int drag_stop_swi /*= 0*/)
{
	_drag_handler = handler;
	_drag_stop_swi = drag_stop_swi;
}

/**
 * Current drag has been cancelled so clear drag handler
 */
void EventRouter::cancel_drag()
{
	if (_drag_handler)
	{
		_drag_handler->drag_cancelled();
		_drag_handler = 0;
		if (_drag_stop_swi) _swix(_drag_stop_swi, 0);
	}
}

/**
 * Add a timer that is called repeatedly after a given time
 * has elapsed.
 *
 * The timer runs after other messages so may occur at a
 * time later than the elapsed time given.
 *
 * @param elapsed Minimum time between each call to the timer in centiseconds
 */
void EventRouter::add_timer(int elapsed, Timer *timer)
{
	TimerInfo *info = new TimerInfo;
	info->due = monotonic_time() + elapsed;
	info->elapsed = elapsed;
	info->timer = timer;
	add_timer_info(info);
}

/**
 * Remove timer
 */
void EventRouter::remove_timer(Timer *timer)
{
	TimerInfo *last = 0, *check = _first_timer;
	while (check && check->timer != timer)
	{
		last = check;
		check = check->next;
	}
	if (check)
	{
		if (last) last->next = check->next;
		else _first_timer = check->next;
		delete check;
	}
}

/**
 * Add internal timer info structure in correct location
 */
void EventRouter::add_timer_info(TimerInfo *info)
{
	if (_first_timer == 0)
	{
		_first_timer = info;
		info->next = 0;
	} else
	{
		TimerInfo *last = 0, *check = _first_timer;
		while (check && monotonic_ge(info->due, check->due))
		{
			last = check;
			check = check->next;
		}
		if (last == 0)
		{
			info->next = _first_timer;
			_first_timer = info;
		} else
		{
			info->next = last->next;
			last->next = info;
		}
	}
}

EventRouter::NullCommandQueue::NullCommandQueue() :
		_commands(0), _capacity(0), _size(0),
		_next(0), _end(0)
{
	_commands = new Command*[8];
	_capacity = 8;
}

EventRouter::NullCommandQueue::~NullCommandQueue()
{
	delete [] _commands;
}

void EventRouter::NullCommandQueue::add(Command *command)
{
	if (_size == _capacity)
	{
		unsigned int new_capacity = _capacity + 8;
		Command **new_commands = new Command*[new_capacity];
		std::memcpy(new_commands, _commands, sizeof(Command *) * _size );
		delete [] _commands;
		_commands = new_commands;
		_capacity = new_capacity;
	}
	_commands[_size++] = command;
}

void EventRouter::NullCommandQueue::remove(Command *command)
{
	if (_size == 0) return;
	unsigned int found;
	for(found = 0; found < _size; found++)
	{
		if (_commands[found] == command) break;
	}
	if (found == _size) return;

	Command **commands = _commands;
	if (_capacity > 12 && _size < _capacity - 12)
	{
		commands = new Command*[_capacity - 8];
	}

	if (found == _size - 1)
	{
		if (commands != _commands && found > 0)
		{
			std::memcpy(commands, _commands, sizeof(Command *) * found);
		}
	} else
	{
		if (found) std::memmove(commands, _commands, sizeof(Command *) * found);
		std::memmove(commands + found, _commands + found + 1, sizeof(Command *) * (_size - found - 1));
	}

	_size--;
	if (_end >= found) _end--;
	if (_next >= found) _next--;

	if (commands != _commands)
	{
		delete [] _commands;
		_commands = commands;
		_capacity -= 8;
	}
}

// Get next command until it hits the end then returns 0 and resets to beginning
Command *EventRouter::NullCommandQueue::next()
{
	Command *command;
	if (_next < _end)
	{
		command = _commands[_next++];
	} else
	{
		command = 0;
		_next = 0;
		_end = _size;
	}

	return command;
}


//! @endcond

/**
 * Construct IdBlock with details from the given object
 */
IdBlock::IdBlock(Object obj)
{
	if (obj.handle() == NULL_ObjectId) throw ObjectNullError();
	self_object_id = obj.handle();
	self_component_id = NULL_ComponentId;

	swix_check(_swix(0x44ECA, _INR(0,1) | _OUTR(0,1), 0, self_object_id,
			&parent_object_id, &parent_component_id));
	swix_check(_swix(0x44ECB, _INR(0,1) | _OUTR(0,1), 0, self_object_id,
			&ancestor_object_id, &ancestor_component_id));
}

/**
 * Get the object the event occured on.
 */
Object IdBlock::self_object() const
{
    return Object(self_object_id);
}

/**
 * Get the component the event occured on
 *
 * The returned component will be null if the event did not occur on a
 * component.
 */
Component IdBlock::self_component() const
{
   return Component(self_object_id, self_component_id);
}

/**
 * Get the parent object of the object the event occured upon.
 *
 * The object will be null if there was no parent object.
 */
Object IdBlock::parent_object() const
{
   return Object(parent_object_id);
}

/**
 * Get the parent component of the object the event occured on
 *
 * The returned component will be null if the event did not occur on a
 * component.
 */

Component IdBlock::parent_component() const
{
   return Component(parent_object_id, parent_component_id);
}

/**
 * Get the ancestor object of the object the event occured upon.
 *
 * The object will be null if there was no parent object.
 */
Object IdBlock::ancestor_object() const
{
   return Object(ancestor_object_id);
}

/**
 * Get the ancestor component of the object the event occured on
 *
 * The returned component will be null if the event did not occur on a
 * component.
 */
Component IdBlock::ancestor_component() const
{
   return Component(ancestor_object_id, ancestor_component_id);
}

