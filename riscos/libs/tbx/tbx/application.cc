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

#include "application.h"
#include "eventrouter.h"
#include "autocreatelistener.h"
#include "prequitlistener.h"
#include "quitlistener.h"
#include "modechangedlistener.h"
#include "palettechangedlistener.h"
#include "command.h"
#include "commandrouter.h"
#include "reporterror.h"
#include "swixcheck.h"
#include "sprite.h"
#include "openermanager.h"
#include "res/resobject.h"
#include <swis.h>
#include <cstdlib>

// Unix file name processing
#include "unixlib/local.h"
int __riscosify_control = __RISCOSIFY_NO_PROCESS;


using namespace tbx;

Application *Application::_instance = 0;

static int all[1] = {0};

const int Application::MIN_WIMP_VERSION = 310;
const int *Application::NO_MESSAGES = (int *)0;
const int *Application::ALL_MESSAGES = all;
const int *Application::NO_EVENTS = (int *)0;;
const int *Application::ALL_EVENTS = all;

/**
 * Initialise the application.
 *
 * If the application can not be initialised this will
 * report the error and then exit the program.
 *
 * @param task_directory The path to application directory
 * @param wimp_version The minimum WIMP version supported by the application.
 *        Defaults to MIN_WIMP_VERSION (310).
 * @param deliver_messages Integer array of message IDs that will be delivered
 *        to the application. Defaults to deliver all messages. Use:
 *        - ALL_MESSAGES - to deliver all messages.
 *        - NO_MESSAGES - to deliver no messages
 *
 * @param deliver_events  Integer array of Toolbox event IDs that will be
 *        delivered to the application. Defaults to deliver all messages.
 *        Use:
 *        - ALL_EVENTS - to deliver all toolbox events.
 *        - NO_EVENTS - to deliver no toolbox events
 */
Application::Application(const char *task_directory,
	    int wimp_version /* = MIN_WIMP_VERSION*/,
	    const int *deliver_messages /*= ALL_MESSAGES*/,
	    const int *deliver_events /*= ALL_EVENTS*/)
{
	_running = false;
	_instance = this;

	// By default TBX applications do not do unix file name processing
	__riscosify_control = __RISCOSIFY_NO_PROCESS;

	_kernel_swi_regs regs;
    int messagesFD[4];

	regs.r[0] = 0; // flags
	regs.r[1] = wimp_version; // Minimum wimp version
	regs.r[2] = reinterpret_cast<int> (deliver_messages); // Message to allow return
	regs.r[3] = reinterpret_cast<int> (deliver_events); // toolbox events to allow
	regs.r[4] = reinterpret_cast<int> (task_directory);
	regs.r[5] = reinterpret_cast<int> (&messagesFD); // Message trans descriptor
	regs.r[6] = reinterpret_cast<int> (&(event_router()->_id_block));

	// Initialise the toolbox
	_kernel_oserror *err = _kernel_swi(0x44ECF, &regs, &regs);
	if (err) {
		report_error(err);
		exit(-1);
	}
	_wimp_version = regs.r[0];
	_task_handle = regs.r[1];
	OsSpriteAreaPtr os_sprite_area = reinterpret_cast<OsSpriteAreaPtr>(regs.r[2]);

	if (os_sprite_area) _sprite_area = new SpriteArea(os_sprite_area, false);
	else _sprite_area = 0;

	_messages.attach(messagesFD);
}

/**
 * Run the main polling loop for the application.
 *
 * Once the application has been initialised the program should
 * call this routine to process the events.
 *
 * The routine will not return until the quit method is called
 * from an event listener or command or the event id 0x82a91
 * (This is the id of the Quit event from the quit dialog) is
 * received.
 *
 * @see quit
 */
void Application::run()
{
	EventRouter *router = event_router();
	_running = true;

	while (_running)
	{
		router->poll();
	}
}

/**
 * Allow other applications and events in this application to be run.
 *
 * Idle commands and timers should be used in preference to this method.
 *
 * This command will repeatedly poll the WIMP and dispatch messages and
 * events for this application until there are no messages waiting.
 *
 * @returns true if the application is still running or false if it should quit
 * @throws Any exception generated by any of the events it calls.
 */
bool Application::yield()
{
	if (_running) event_router()->yield();
	return _running;
}

/**
 * Set the AutoCreateListener to be called when an object with the specified
 * template is created.
 *
 * This function is used to capture the object handle of an auto created
 * object so it can be used to add event handlers or stored for later
 * use.
 *
 * Objects are auto created when the toolbox resources are loaded if the
 * auto create flag was set in the toolbox resource file.
 *
 * @param template_name The name of the object template.
 * @param listener The auto create listener to call when an object for the
 *        specified template is called.
 */
void Application::set_autocreate_listener(std::string template_name, AutoCreateListener *listener)
{
	event_router()->set_autocreate_listener(template_name, listener);
}

/**
 * Clear the autocreate listener for the specified template.
 *
 * @param template_name The name of the template.
 */
void Application::clear_autocreate_listener(std::string template_name)
{
	event_router()->clear_autocreate_listener(template_name);
}

/**
 * Add a command to be run for the specified event ID.
 *
 * Multiple commands can be added for the same event ID.
 *
 * For more details on command IDs, events and commands see
 * the topic
 * @subpage events
 *
 * @param command_id The ID for the command
 * @param command The command to run.
  */
void Application::add_command(int command_id, Command *command)
{
	event_router()->add_object_listener(NULL_ObjectId,NULL_ComponentId, command_id, command, command_router);
}

/**
 * Remove a command.
 *
 * The event is only removed if the command ID and the command
 * match a previously added command.
 *
 * @param command_id The command ID used for this command
 * @param command The command that was originally added
 */
void Application::remove_command(int command_id, Command *command)
{
	event_router()->remove_object_listener(NULL_ObjectId,NULL_ComponentId, command_id, command);
}

/**
 * Add a command to be run when no events are being received from the desktop.
 *
 * @param command The command to add
 */
void Application::add_idle_command(Command *command)
{
	event_router()->add_null_event_command(command);
}

/**
 * Remove a command to be run when no events are being received from the desktop.
 *
 * @param command the command to remove
 */

void Application::remove_idle_command(Command *command)
{
	event_router()->remove_null_event_command(command);
}

/**
 * Add a listener for the given user message (event code 17)
 *
 * @param message_id code to listen for.
 * @param listener to execute.
 */
void Application::add_user_message_listener(int message_id, WimpUserMessageListener *listener)
{
	event_router()->add_message_listener(17, message_id, listener);
}

/**
 * Remove a listener for the given user message.
 *
 * @param message_id code to remove
 * @param listener to remove.
 */
void Application::remove_user_message_listener(int message_id, WimpUserMessageListener *listener)
{
	event_router()->remove_message_listener(17, message_id, listener);
}

/**
 * Add a listener for the given recorded message (event code 18)
 *
 * @param message_id code to listen for.
 * @param listener to execute.
 */
void Application::add_recorded_message_listener(int message_id, WimpRecordedMessageListener *listener)
{
	event_router()->add_message_listener(18, message_id, listener);
}

/**
 * Remove a listener for the given recorded message.
 *
 * @param message_id code to remove
 * @param listener to remove.
 */
void Application::remove_recorded_message_listener(int message_id, WimpRecordedMessageListener *listener)
{
	event_router()->remove_message_listener(18, message_id, listener);
}

/**
 * Add a listener for the given acknowledge message (event code 18)
 *
 * @param message_id code to listen for.
 * @param listener to execute.
 */
void Application::add_acknowledge_message_listener(int message_id, WimpAcknowledgeMessageListener *listener)
{
	event_router()->add_message_listener(19, message_id, listener);
}

/**
 * Remove a listener for the given acknowledge message.
 *
 * @param message_id code to remove
 * @param listener to remove.
 */
void Application::remove_acknowledge_message_listener(int message_id, WimpAcknowledgeMessageListener *listener)
{
	event_router()->remove_message_listener(19, message_id, listener);
}


/**
 * Add a listener for the pre quit message sent (usually) by the desktop.
 *
 * This message gives the application a chance to refuse to quit if
 * it has unsaved documents.
 *
 * @param listener the prequit listener to add
 */
void Application::add_prequit_listener(PreQuitListener *listener)
{
	if (PreQuitManager::instance() == 0)
	{
		event_router()->add_message_listener(18, 8, new PreQuitManager());
	}
	PreQuitManager::instance()->add_listener(listener);
}

/**
 * Remove a prequit listener
 *
 * @param listener the prequit listener to remove
 */
void Application::remove_prequit_listener(PreQuitListener *listener)
{
	if (PreQuitManager::instance() != 0)
		PreQuitManager::instance()->remove_listener(listener);
}

/**
 * Add a listener to process the Quit message.
 *
 * The quit message is sent when the desktop wants the application to close.
 * At this point the application does not have a choice and must close.
 *
 * The application class will drop out of its run method after these messages
 * have been processed and the program should exit.
 *
 * Use a prequit listener to object to the desktop closing down the application.
 *
 * @param listener quit listener to add
 */
void Application::add_quit_listener(QuitListener *listener)
{
	event_router()->add_message_listener(18, 0, &(listener->_message_listener));
}

/**
 * Remove a quit listener
 *
 * @param listener to remove
 */
void Application::remove_quit_listener(QuitListener *listener)
{
	event_router()->remove_message_listener(18, 0, &(listener->_message_listener));
}

/**
 * Listen for changes to the Desktop mode.
 *
 * This message is used if you need to know anything about changes to the
 * screen.
 *
 * When the mode changes the WIMP redraws all the windows so this event
 * normally only used when sizes depend on pixel rather than OS sizes or
 * eigen factors or colours are cached.
 *
 * @param listener listener for mode change events
 */
void Application::add_mode_changed_listener(ModeChangedListener *listener)
{
	event_router()->add_message_listener(17, 0x400C1, &(listener->_message_listener));
}

/**
 * Remove mode changed listener

 * @param listener previously added listener for mode change events
 */
void Application::remove_mode_changed_listener(ModeChangedListener *listener)
{
	event_router()->remove_message_listener(17, 0x400C1, &(listener->_message_listener));
}

/**
 * Add listener for changes to desktop palette.
 *
 * This message is sent if the desktop palette utility changes
 * the colour palette.
 *
 * It it normally only needed if an application has to change
 * internal colour tables.
 *
 * Note though that the palette utility automatically forces a redraw of
 * the whole screen if any of the WIMP's standard colours change their
 * logical mapping, so applications don't have to take further action
 * if they are just using WIMP colours.
 *
 * It is not generated on a mode change so you need to listen to that
 * event as well.
 *
 * @param listener listener for palette change events
 */
void Application::add_palette_changed_listener(PaletteChangedListener *listener)
{
	event_router()->add_message_listener(17, 9, &(listener->_message_listener));
}

/**
 * Remove listener for changes to the desktop palette
 *
 * @param listener previously added listener for palette change events
 */
void Application::remove_palette_changed_listener(PaletteChangedListener *listener)
{
	event_router()->remove_message_listener(17, 9, &(listener->_message_listener));
}


/**
 * Add a timer to the application that will be called at a
 * regular interval.
 *
 * Timers are only processed when there are no other events
 * so they will not necessarily be delivered exactly on time.
 *
 * @param elapsed number of centiseconds between calls
 * @param timer - timer to add
 */
void Application::add_timer(int elapsed, Timer *timer)
{
	event_router()->add_timer(elapsed, timer);
}

/**
 * Remove the given timer
 *
 * @param timer Timer to remove
 */
void Application::remove_timer(Timer *timer)
{
	event_router()->remove_timer(timer);
}

/**
 * Add a file opener.
 *
 * A file opener is called when a file of a given type is
 * double clicked in the filer.
 *
 * The file opener uses the same Loader class as a file
 * loader so the same loader can be shared, but note it
 * only fills in the file type, file name and from filer
 * fields.
 *
 * If the type is set to -2 you must implement the accept_file
 * member of the Loader or this application will intercept
 * all files.
 *
 * @param loader the loader used to open the file
 * @param file_type the file type for the loader or -2 for
 *        any type.
 */
void Application::add_opener(Loader *loader, int file_type)
{
	OpenerManager *manager = OpenerManager::instance();
	if (manager == 0) manager = new OpenerManager();
	manager->add_opener(file_type, loader);
}

/**
 * Remove a file opener.
 *
 * @param loader the loader used to open the file to remove
 * @param file_type the file type for the loader or -2 for
 *        any type.
 */
void Application::remove_opener(Loader *loader, int file_type)
{
	OpenerManager *manager = OpenerManager::instance();
	if (manager != 0)
		manager->remove_opener(file_type, loader);
}

/**
 * Get a Resource object template from the main toolbox templates.
 *
 * The return value should not be saved as the resource it
 * points to is not guaranteed to exist the same for the
 * lifetime of the application.
 */
res::ResObject Application::resource(std::string template_name)
{
    res::ResObjectHeader *res = 0;
    swix_check(_swix(0x44EFB, _INR(0,1)|_OUT(0),
        0, reinterpret_cast<int>(template_name.c_str()),
        &res));

    return res::ResObject(res);
}

/**
 * Return the name of the directory that was used to initialised this
 * application.
 *
 * RISC OS 5 always returns this as a path, so may have appended a
 * '.' to the name. RISC OS 4.0.2 doesn't do this.
 *
 * For consistency this function returns it in the RISC OS 5 format
 * whatever the OS.
 */
std::string Application::directory() const
{
	int len;
	// Toolbox_GetSysInfo to get the directory size
	swix_check(_swix(0x44ECE, _INR(0,2)|_OUT(2), 2, 0, 0, &len));

    std::string dir;
    if (len)
    {
    	char buffer[len+1];
        swix_check(_swix(0x44ECE, _INR(0,2), 2,
    		   reinterpret_cast<int>(buffer),
    		   len));
        if (buffer[len-2] != '.' && buffer[len-2] != ':')
        {
        	buffer[len-1] = '.';
        	buffer[len] = 0;
        }
        dir = buffer;
    }

    return dir;
}

/**
 * Turn on or off UnixLib's automatic translation of unix
 * style file names.
 *
 * By default TBX applications have the unix file name translation
 * turned off.
 *
 * When turned on this uses the default processing.
 * @see unix_file_name_control to tune how the details of the translation
 *
 * @param on true to turn on the file name translation, false to turn it off
 *
 */
void Application::unix_file_name_translation(bool on)
{
	if (on) __riscosify_control = 0;
	else __riscosify_control = __RISCOSIFY_NO_PROCESS;
}

/**
 * Check if some unix file name translations are being used
 * @return true if any unix file name translation is in force.
 */
bool Application::unix_file_name_translation() const
{
	return ((__riscosify_control & __RISCOSIFY_NO_PROCESS) == 0);
}

/**
 * Set flags to control the unix file name translation process.
 *
 * @param flags See the unixlib/local.h UnixLib header file RISCOSIFY flags.
 */
void Application::unix_file_name_control(int flags)
{
	__riscosify_control = flags;
}

/**
 * Get the flags that control the unix file name translation process
 */
int Application::unix_file_name_control() const
{
	return __riscosify_control;
}

/**
 * Catch any uncaught exceptions thrown during polling and
 * show them in an error box.
 *
 * If this is off, uncaught exceptions will close the program.
 *
 * This defaults to true so an end user will get an indication
 * if something goes wrong.
 *
 * It is useful to turn it off during development so a full
 * stack trace of the error is shown in stdout.
 *
 * @param on - true to catch the unhandled exceptions, false
 *             to let them through.
 */
void Application::catch_poll_exceptions(bool on)
{
	event_router()->catch_exceptions(on);
}

/**
 * Report any uncaught exceptions to the given interface.
 *
 * catch_poll_exceptions must be on for the given routine
 * to be called.
 *
 * Setting the handler to 0 (the default), will just show
 * a simple message in an error box and leave the application
 * running.
 *
 * @param handler The class to handle the uncaught exception or
 * 0 to use the default uncaught exception handling.
 */
void Application::uncaught_handler(UncaughtHandler *handler)
{
	event_router()->uncaught_handler(handler);
}


/**
 * Set listener to run just before the internal call to Wimp_Poll.
 *
 * There can only be one of these listeners. It is called immediately before
 * Wimp_Poll.
 *
 * This is an advanced routine and should be used sparingly.
 *
 * @param listener Listener to call prior to wimp poll. Set to 0 to remove
 * the post poll listener.
 */
void Application::set_pre_poll_listener(PrePollListener *listener)
{
   event_router()->_pre_poll_listener = listener;
}

/**
 * Set listener to look at the results of the internal call to Wimp_Poll.
 *
 * There can only be one of these listeners. It is called immediately after
 * Wimp_Poll before any other processing.
 *
 * Care must be taken in the PostPollListener so as not to cause problems
 * for the processing that follows this.
 *
 * The call is mainly provided to help with debugging of the messages received
 * by the application by the Wimp.
 *
 * @param listener Listener to report the poll results to. Set to 0 to remove
 * the post poll listener.
 */
void Application::set_post_poll_listener(PostPollListener *listener)
{
   event_router()->_post_poll_listener = listener;
}


/**
 * Set listener to call after the event processing in TBX.
 *
 * There can only be one of these listeners. It is called after Wimp_Poll
 * and any event processing.
 *
 * The parameters past to the post event listener may have been changed
 * by the event processing. (Though this is usually not the case).
 *
 * This is an advanced routine and should be used sparingly.
 * @param listener Listener to report the poll results to. Set to 0 to remove
 * the post poll listener.
 */
void Application::set_post_event_listener(PostEventListener *listener)
{
   event_router()->_post_event_listener = listener;
}


/**
 * Check if this application owns WIMP window/icon bar icon
 *
 * @param window_handle WIMP window handle to test or -2 to check an icon bar
 * @param icon_handle to test (ignored if window handle is not -2)
 */
bool Application::owns_window(WindowHandle window_handle, IconHandle icon_handle /*= 0*/)
{
	WimpMessage msg(0,5);
	return (msg.send(WimpMessage::Acknowledge, window_handle, icon_handle) == _task_handle);
}


/**
 *  Start a child task from this application.
 *
 *  This call returns when the task called calls Wimp_Poll or after it finishes.
 *
 *  @param command command line to start the task
 *  @returns handle of task started if it's still alive or 0
 *  @throws OsError Start task failed
 */
int Application::start_wimp_task(std::string command)
{
	int handle;
	swix_check(_swix(Wimp_StartTask, _IN(0)|_OUT(0), command.c_str(), &handle));
	return handle;
}

/**
 * Run a command using the RISC OS Command Line interpreter
 *
 * Warning: This method should only be used for module commands, any
 * command that runs another program will replace the program that
 * calls it and never return.
 *
 * Use start_wimp_task to run another program, leaving this program
 * running.
 *
 * @param command command to run
 * @throws OsError command failed to run
 */
void Application::os_cli(std::string command)
{
	swix_check(_swix(OS_CLI, _IN(0), command.c_str()));
}

