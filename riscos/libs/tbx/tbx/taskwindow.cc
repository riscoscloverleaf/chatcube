/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2015 Alan Buckley   All Rights Reserved.
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
#include "taskwindow.h"
#include "swixcheck.h"
#include "application.h"

#include <swis.h>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <iomanip>
#include <algorithm>

#include <iostream>

namespace tbx
{

/**
 * Used to generate the next id for task window startup
 */
unsigned int TaskWindow::_next_txt_id = 1;


/**
 * Uninitialised task window.
 * At least the command must be set before calling run.
 */
TaskWindow::TaskWindow() :
	_wimp_slot(0),
	_options(0),
	_child_task(0),
	_txt_id(0),
	_running(false),
	_router(0)
{
}

/**
 * Construct a task window.
 * Call the run method to start the task window
 * @param command command to run
 * @param name name of the child task
 * @param wimp_slot size in KB of the wimp slot for the task or 0 to use the Next
 * slot from the RISC OS task manager.
 * @param options one or more of the Options enum or 0 for none.
 */
TaskWindow::TaskWindow(std::string command, std::string name, int wimp_slot /*= 0*/, unsigned int options /*= 0*/) :
	_command(command),
	_name(name),
	_wimp_slot(wimp_slot),
	_options(options),
	_child_task(0),
	_txt_id(0),
	_running(false),
	_router(0)
{
}

/**
 * Destructor, remove interest in the task window.
 * The destructor does not stop a running task, just
 * removes any listeners.
 */
TaskWindow::~TaskWindow()
{
	delete _router;
}

/**
 * Add listener to detect when the child task is started
 * @param listener listener to add
 */
void TaskWindow::add_started_listener(TaskWindowStartedListener *listener)
{
	if (!_router) _router = new Router(this);
	_router->_started_listeners.push_back(listener);
}

/**
 * Remove listener to detect when the child task is started
 * @param listener to remove
 */
void TaskWindow::remove_started_listener(TaskWindowStartedListener *listener)
{
	if (_router)
	{
		std::vector<TaskWindowStartedListener *>::iterator found = std::find(
				_router->_started_listeners.begin(),
				_router->_started_listeners.end(),
				listener);
		if (found != _router->_started_listeners.end())
		{
			_router->_started_listeners.erase(found);
		}
	}
}

/**
* Add listener to detect when the child task has finished
* @param listener listener to add
*/
void TaskWindow::add_finished_listener(TaskWindowFinishedListener *listener)
{
	if (!_router) _router = new Router(this);
	_router->_finished_listeners.push_back(listener);
}

/**
* Remove listener to detect when the child task has finished
* @param listener to remove
*/
void TaskWindow::remove_finished_listener(TaskWindowFinishedListener *listener)
{
	if (_router)
	{
		std::vector<TaskWindowFinishedListener *>::iterator found = std::find(
				_router->_finished_listeners.begin(),
				_router->_finished_listeners.end(),
				listener);
		if (found != _router->_finished_listeners.end())
		{
			_router->_finished_listeners.erase(found);
		}
	}
}

/**
* Add listener to capture output from the child task.
*
* One or more of these listeners must be added before the task window
* is run or the output will go to an external window.
* @param listener listener to add
*/
void TaskWindow::add_output_listener(TaskWindowOutputListener *listener)
{
	if (!_router) _router = new Router(this);
	if (_router->_output_listeners.empty())
	{
		tbx::app()->add_user_message_listener(0x808C1, _router); // TaskWindow_Output
		tbx::app()->add_recorded_message_listener(0x808C1, _router);
	}
	_router->_output_listeners.push_back(listener);
}

/**
* Remove listener to detect output from the child task
* @param listener to remove
*/
void TaskWindow::remove_output_listener(TaskWindowOutputListener *listener)
{
	if (_router)
	{
		std::vector<TaskWindowOutputListener *>::iterator found = std::find(
				_router->_output_listeners.begin(),
				_router->_output_listeners.end(),
				listener);
		if (found != _router->_output_listeners.end())
		{
			_router->_output_listeners.erase(found);
		}
		if (_router->_output_listeners.empty())
		{
			tbx::app()->remove_user_message_listener(0x808C1, _router); // TaskWindow_Output
			tbx::app()->remove_recorded_message_listener(0x808C1, _router);
		}
	}
}

/**
 * Run the child task.
 *
 * At least one listener (of any type) must be added before calling this
 * method for the running() and child_task() field to be updated.
 *
 * If any listeners are added then output from the task window will
 * be directed to this application. Use the output listener to process
 * this output.
 *
 * If no listeners are added the task will be run by an external
 * application. As mentioned above in this case this application
 * has no knowledge of the state of the child task.
 *
 * @throws std::logic_error if the command to run has not been set, or the
 *  command is already running.
 * @throws tbx::OsError call to start the task failed
 */
void TaskWindow::run()
{
	if (_command.empty()) throw std::logic_error("command not set before run");
	if (_running) throw std::logic_error("Cannot run command while this TaskWindow is running");

	std::ostringstream ex;
	ex << "TaskWindow \"" << _command << "\"";
	if (_wimp_slot) ex << " -wimpslot " << _wimp_slot << "K";
	if (!_name.empty()) ex << " -name \"" << _name << "\"";
 	if (_options & ALLOW_CONTROL) ex << " -ctrl";
	if (_options & DISPLAY) ex << " -display";
	if (_options & QUIT) ex << " -quit";

	ex << std::setw(8) << std::hex << std::setfill('0');

	if (_router)
	{
		ex << " -task &" << tbx::app()->task_handle();
		_txt_id = _next_txt_id++;
		ex << " -txt &" << _txt_id;
	}

    std::string to_run = ex.str();

	int started;
	tbx::swix_check(_swix(Wimp_StartTask,_IN(0) | _OUT(0) , to_run.c_str(), &started));
	// Can only check the lifetime if any listeners have been added.
	if (_router) _running = true;
}

/**
* Send input to the task window.
*
* You can only send up to 232 chars this way.
* Use the normal Wimp data transfer to send more characters.
* @param text to send.
* @param number of characters to send or -1 to count the length of a
* 0 terminated string.
* @throws std::logic_error if you try to send an empty string,
* a string with too many characters, or to a child task that hasn't started.
*/
void TaskWindow::send_input(const char *text, int size /*= -1*/)
{
	if (_child_task == 0) throw std::logic_error("Child task must be running before sending input");
	if (size == -1) size = std::strlen(text);
	if (size == 0) throw std::logic_error("Cannot send an empty string to a task window");
	if (size > 232) throw std::logic_error("Can only send a maximum of 232 characters with send_input");

	int msg_size = (size + 3) / 4;
	tbx::WimpMessage send(0x808c0, msg_size+6); // TaskWindow_SendInput
	send[5] = size;
	std::memcpy(send.str(6), text, size);
	send.send(tbx::WimpMessage::User, _child_task);
}

/**
 * Send input to the task window.
 *
 * You can only send up to 232 chars this way.
 * Use the normal Wimp data transfer to send more characters.
 * @param text to send.
 * @throws std::logic_error if you try to send and empty string,
 * a string with too many characters, or to a child task that hasn't started.
 */
void TaskWindow::send_input(const std::string &text)
{
	send_input(text.c_str(), (int)text.size());
}

/**
 * Suspend the current child task
 *
 * @throws std::logic_error if the task has not been started
 */
void TaskWindow::suspend()
{
	if (_child_task == 0) throw std::logic_error("Child task must be running before it can be suspended");
	tbx::WimpMessage msg(0x808C6, 5); // TaskWindow_Suspend
	msg.send(tbx::WimpMessage::User, _child_task);
}

/**
* Resume the current child task after it has been suspended with a
* call to suspend.
*
* @throws std::logic_error if the task has not been started
*/
void TaskWindow::resume()
{
	if (_child_task == 0) throw std::logic_error("Child task must be running before it can be resumed");
	tbx::WimpMessage msg(0x808C7, 5); // TaskWindow_Resume
	msg.send(tbx::WimpMessage::User, _child_task);
}
/**
* Kill the current child task
*
* @throws std::logic_error if the task has not been started
*/
void TaskWindow::kill()
{
	if (_child_task == 0) throw std::logic_error("Child task must be running before it can be killed");
	tbx::WimpMessage msg(0x808C4, 5); // TaskWindow_Morite
	msg.send(tbx::WimpMessage::User, _child_task);
}

/**
 * Construct router to convert Wimp messages to task window listeners
 */
TaskWindow::Router::Router(TaskWindow *me) :
		_me(me)
{
	// always add start and end so we can monitor lifetime
	tbx::app()->add_user_message_listener(0x808C2, this); // TaskWindow_Ego
	tbx::app()->add_recorded_message_listener(0x808C2, this);
	tbx::app()->add_user_message_listener(0x808C3, this); // TaskWindow_Morio
	tbx::app()->add_recorded_message_listener(0x808C3, this);
}

/**
 * Stop listeners
 */
TaskWindow::Router::~Router()
{
	tbx::app()->remove_user_message_listener(0x808C2, this);
	tbx::app()->remove_recorded_message_listener(0x808C2, this);
	tbx::app()->remove_user_message_listener(0x808C3, this);
	tbx::app()->remove_recorded_message_listener(0x808C3, this);
	if (!_output_listeners.empty())
	{
		tbx::app()->remove_user_message_listener(0x808C1, this);
		tbx::app()->remove_recorded_message_listener(0x808C1, this);
	}
}

/**
 * Set state of TaskWindow and forward any events
 */
void TaskWindow::Router::user_message(tbx::WimpMessageEvent &event)
{
	switch (event.message().message_id())
	{
	case 0x808C2: // TaskWindow_Ego - started listener
		if (event.message()[5] == (int)_me->_txt_id)
		{
			_me->_child_task = event.message().sender_task_handle();
			for (std::vector<TaskWindowStartedListener *>::iterator i = _started_listeners.begin();
			i != _started_listeners.end(); ++i)
			{
				(*i)->taskwindow_started(*_me);
			}
		}
		break;

	case 0x808C3: // TaskWindow_Morio - finished listener
		if ((int)_me->_child_task == event.message().sender_task_handle())
		{
			_me->_child_task = 0;
			_me->_running = false;
			for (std::vector<TaskWindowFinishedListener *>::iterator i = _finished_listeners.begin();
			i != _finished_listeners.end(); ++i)
			{
				(*i)->taskwindow_finished(*_me);
			}
		}
		break;

	case 0x808C1: // TaskWindow_Output - Output listener
		if ((int)_me->_child_task == event.message().sender_task_handle())
		{
			int size = event.message()[5];
			const char *text = event.message().str(6);
			for (std::vector<TaskWindowOutputListener *>::iterator i = _output_listeners.begin();
			i != _output_listeners.end(); ++i)
			{
				(*i)->taskwindow_output(*_me, size, text);
			}
		}
		break;
	}
}

/**
 * Taskwindow event sent recorded so process and acknowledge
 */
void TaskWindow::Router::recorded_message(tbx::WimpMessageEvent &event, int reply_to)
{
	bool old_running = _me->_running;
	// Base processing same as user_message
	user_message(event);
	// But send an acknowledgement if for this task window
	if (old_running != _me->_running || (int)_me->_child_task == event.message().sender_task_handle())
	{
		tbx::WimpMessage ack(event.message());
		ack.your_ref(ack.my_ref());
		ack.send(tbx::WimpMessage::Acknowledge, reply_to);
	}
}

} /* End of namespace tbx */
