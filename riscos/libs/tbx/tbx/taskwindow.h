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
#ifndef TBX_TASKWINDOW_H
#define TBX_TASKWINDOW_H

#include <string>
#include <vector>
#include "listener.h"
#include "wimpmessagelistener.h"

namespace tbx
{

class TaskWindowStartedListener;
class TaskWindowFinishedListener;
class TaskWindowOutputListener;

/**
 * Class to start a child task in a RISC OS task window
 */
class TaskWindow
{
	std::string _command;
	std::string _name;
	int _wimp_slot;
	unsigned int _options;
	unsigned int _child_task;
	unsigned int _txt_id;
	bool _running;
	static unsigned int _next_txt_id;
	class Router : 
		public tbx::WimpUserMessageListener,
		public tbx::WimpRecordedMessageListener
	{
		TaskWindow *_me;
	public:
		Router(TaskWindow *me);
		~Router();
		std::vector<TaskWindowStartedListener *> _started_listeners;
		std::vector<TaskWindowFinishedListener *> _finished_listeners;
		std::vector<TaskWindowOutputListener *> _output_listeners;

		virtual void user_message(tbx::WimpMessageEvent &event);
		virtual void recorded_message(tbx::WimpMessageEvent &event, int reply_to);
	} *_router;
	friend Router;
public:
	/**
	 * Task window run options
	*/
	enum Options
	{
		/**
		 * Allow control characters to be sent to the output
		 */
		ALLOW_CONTROL=1, 
		/**
		 * Show the taskwindow output window before any output comes from the task
		 */
		DISPLAY=2, 
		/**
		 * Quit the task windo wht the child task finishes
		 */
		QUIT=4
	};

	TaskWindow();
	TaskWindow(std::string command, std::string name, int wimp_slot = 0, unsigned int options = 0);
	~TaskWindow();

	/**
	 * Get the command run by this task window
	 *
	 * @returns task window command
	 */
	const std::string &command() const { return _command; }
	/**
	 * Set the command run by this task window.
	 * Changing this will have no effect on a currently running task window
	 * @param command command to run
	 */
	void command(const std::string &command) { _command = command; }
	/**
	 * Get the task name for the task window
	 * @returns the task window name
	 */
	const std::string &name() const { return _name; }
	/**
	 * Set the name of the task window
	 * Changing this will have no effect on a currently running task window
	 * @param name the name for the task window
	 */
	void name(const std::string &name) { _name = name; }
	/**
	 * Get the wimp slot size in KB for the task window
	 */
	int wimp_slot() const { return _wimp_slot; }
	/**
	 * Set the wimp slot for the task window
	 * Changing this will have no effect on a currently running task window
	 * @param slot wimp slot size in KB for the task window
	 */
	void wimp_slot(int slot) { _wimp_slot = slot; }

	/**
	 * The options applied when the task window is executed.
	 * zero or more of the Options enum,
	 */
	unsigned int options() const { return _options; }
	/**
	 * Set the options when the task window is executed
	 * Changing this will have no effect on a currently running task window
	 * @param new_opts one or more of the options enum.
	 */
	void options(unsigned int new_opts) { _options = new_opts; }

	/**
	 * Check if task window is running.
	 * A task window is running from the time the run function is called
	 * until it finishes.
	 * @returns true if the task window is running
	 */
	bool running() const { return _running; }
	/**
	 * task id of the child task.
	 * This is zero until the task has started and zero again once if finishes.
	 * @returns child task id or 0.
	 */
	unsigned int child_task() const { return _child_task; }

	void add_started_listener(TaskWindowStartedListener *listener);
	void remove_started_listener(TaskWindowStartedListener *listener);
	void add_finished_listener(TaskWindowFinishedListener *listener);
	void remove_finished_listener(TaskWindowFinishedListener *listener);
	void add_output_listener(TaskWindowOutputListener *listener);
	void remove_output_listener(TaskWindowOutputListener *listener);

	void run();

	void send_input(const char *text, int size = -1);
	void send_input(const std::string &text);
	void suspend();
	void resume();
	void kill();
};

/**
 * Listener for when a task window has started
 */
class TaskWindowStartedListener : public tbx::Listener
{
public:
	TaskWindowStartedListener() {}
	virtual ~TaskWindowStartedListener() {}

	/**
	 * Called when the task window has started
	 * @param task_window - the task window that has started
	 */
	virtual void taskwindow_started(TaskWindow &task_window) = 0;
};

/**
* Listener for when a task window has finished
*/
class TaskWindowFinishedListener : public tbx::Listener
{
public:
	TaskWindowFinishedListener() {}
	virtual ~TaskWindowFinishedListener() {}

	/**
	* Called when the task window has finished
	* @param task_window - the task window that has finished
	*/
	virtual void taskwindow_finished(TaskWindow &task_window) = 0;
};

/**
* Listener for output from a task window
*/
class TaskWindowOutputListener : public tbx::Listener
{
public:
	TaskWindowOutputListener() {}
	virtual ~TaskWindowOutputListener() {}

	/**
	* Called when there is output from the task window has
	* @param task_window the task window with output
	* @param size number of bytes of output
	* @param text output text
	*/
	virtual void taskwindow_output(TaskWindow &task_window, int size, const char *text) = 0;
};

}; /* end of namespace tbx */

#endif
