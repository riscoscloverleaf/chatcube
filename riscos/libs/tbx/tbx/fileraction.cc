/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2012 Alan Buckley   All Rights Reserved.
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
 * fileraction.cc
 *
 *  Created on: 21 Feb 2012
 *      Author: alanb
 */

#include "fileraction.h"
#include "swixcheck.h"
#include "path.h"
#include "wimpmessagelistener.h"
#include "command.h"
#include "application.h"

#include <swis.h>
#include <stdexcept>

namespace tbx {

// Internal class to detect when the FilerAction task has finished
// Looks out for task finished message for this task
//! @cond INTERNAL
class FilerActionTaskFinished :
   public WimpUserMessageListener
{
    int _handle;
    FilerActionFinishedListener *_listener;

public:
    FilerActionTaskFinished(int handle, FilerActionFinishedListener *listener) :
       _handle(handle), _listener(listener)
    {
       app()->add_user_message_listener(0x400c3, this);
    };

 	virtual void user_message(WimpMessageEvent &event)
 	{
 	    if (event.message().sender_task_handle() == _handle)
 	    {
 	       _listener->fileraction_finished();
 	       app()->remove_user_message_listener(0x400c3, this);
 	       delete this;
 	    }
 	}
};

//! @endcond INTERNAL

/**
 * Constructor with no parameters
 *
 * You must call directory and add_objects or set_object to specify what
 * to work on before running an operation
 */
FilerAction::FilerAction() : _handle(0)
{

}

/**
 * Construct with the object
 *
 * @param object_name name of object to operate on
 * @throws OsError if failed to set up filer action
 */
FilerAction::FilerAction(const std::string &object_name) :
		_handle(0)
{
	set_object(object_name);
}

/**
 * Construct with directory and objects to operate on
 *
 * @param dir_name directory containing objects to operator on
 * @param objects space separated list of leaf names in the directory to operate on.
 * @throws OsError failed to setup filer action
 */
FilerAction::FilerAction(const std::string &dir_name, const std::string &objects) :
		_handle(0)
{
	directory(dir_name);
	add_objects(objects);
}


FilerAction::~FilerAction()
{
}

/**
 * Set the directory containing the object to operate on
 *
 * @param dir_name directory containing objects to operator on
 * @throws OsError failed to setup filer action
 */
void FilerAction::directory(const std::string &dir_name)
{
	start();
	swix_check(_swix(FilerAction_SendSelectedDirectory, _INR(0,1),
			_handle,
			reinterpret_cast<int>(dir_name.c_str())));
}

/**
 * Set the objects to operate upon.
 *
 * The directory should have been set first using the directory methods.
 *
 * @param objects space separated list of objects to operate upon
 * @throws std::runtime_error directory hasn't been set
 * @throws OsError failed to set files
 */
void FilerAction::add_objects(const std::string &objects)
{
	if (_handle == 0) throw std::runtime_error("FilerAction directory must be set before adding objects");
	swix_check(_swix(FilerAction_SendSelectedFile, _INR(0,1),
			_handle,
			reinterpret_cast<int>(objects.c_str())));
}

/**
 * Set the filer action to operate on a single file system object
 * e.g. a directory or file.
 *
 * @param object_name name of object to operate upon
 * @throws OsError failed to set up filer action
 */
void FilerAction::set_object(const std::string &object_name)
{
	tbx::Path path(object_name);
	directory(path.parent());
	add_objects(path.leaf_name());
}

/**
 * Copy object(s) to the target directory
 *
 * @param target_dir target directory for copy
 * @param options Options for the copy
 * @throws std::runtime_error if source is not set
 * @throws OsError if failed to start copy
 */
void FilerAction::copy(const std::string &target_dir, int options /*= NONE*/)
{
	if (_handle == 0) throw std::runtime_error("FilerAction directory not set");
	swix_check(_swix(FilerAction_SendStartOperation, _INR(0,4),
			_handle,
			0, // Copy
			options,
			reinterpret_cast<int>(target_dir.c_str()),
			target_dir.size() + 1
			));
}

/**
 * Move object(s) to the target directory by renaming
 *
 * @param target_dir target directory for move
 * @param options Options for the move
 * @throws std::runtime_error if source is not set
 * @throws OsError if failed to start move
 */
void FilerAction::rename(const std::string &target_dir, int options /*= NONE*/)
{
	if (_handle == 0) throw std::runtime_error("FilerAction directory not set");
	swix_check(_swix(FilerAction_SendStartOperation, _INR(0,4),
			_handle,
			1, // Move
			options,
			reinterpret_cast<int>(target_dir.c_str()),
			target_dir.size() + 1
			));
}

/**
 * Remove (delete) object(s)
 *
 * @param options Options for the remove
 * @throws std::runtime_error if source is not set
 * @throws OsError if failed to start remove
 */
void FilerAction::remove(int options /*= NONE*/)
{
	if (_handle == 0) throw std::runtime_error("FilerAction directory not set");
	swix_check(_swix(FilerAction_SendStartOperation, _INR(0,4),
			_handle,
			2, // Delete
			options,
			0, // Unused
			0
			));
}

/**
 * Set access for object(s)
 *
 * @param set access bits to set
 * @param leave access bits to be left alone
 * @param options Options for the set access
 * @throws std::runtime_error if source is not set
 * @throws OsError if failed to start set access
 */
void FilerAction::set_access(int set, int leave, int options /*= NONE*/)
{
	if (_handle == 0) throw std::runtime_error("FilerAction directory not set");
	swix_check(_swix(FilerAction_SendStartOperation, _INR(0,4),
			_handle,
			3, // set access
			options,
			(set & 0xFFFF) | ((leave &0xFFFF)<<16),
			4
			));
}

/**
 * Set file type for object(s)
 *
 * @param type new file type
 * @param options Options for the file type
 * @throws std::runtime_error if source is not set
 * @throws OsError if failed to start setting file type
 */
void FilerAction::set_file_type(int type, int options /*= NONE*/)
{
	if (_handle == 0) throw std::runtime_error("FilerAction directory not set");
	swix_check(_swix(FilerAction_SendStartOperation, _INR(0,4),
			_handle,
			4, // set file type
			options,
			type,
			4
			));
}

/**
 * Show a count of the selected object(s)
 *
 * @param options Options for the count
 * @throws std::runtime_error if source is not set
 * @throws OsError if failed to start count
 */
void FilerAction::count(int options /*= NONE*/)
{
	if (_handle == 0) throw std::runtime_error("FilerAction directory not set");
	swix_check(_swix(FilerAction_SendStartOperation, _INR(0,4),
			_handle,
			5, // count
			options,
			0, // Not used
			0
			));
}

/**
 * Move object(s) to the target directory by copying first then deleting
 *
 * @param target_dir target directory for move
 * @param options Options for the move
 * @throws std::runtime_error if source is not set
 * @throws OsError if failed to start move
 */
void FilerAction::move(const std::string &target_dir, int options /*= NONE*/)
{
	swix_check(_swix(FilerAction_SendStartOperation, _INR(0,4),
			_handle,
			6, // Move
			options,
			reinterpret_cast<int>(target_dir.c_str()),
			target_dir.size() + 1
			));
}

/**
 * Copy file within the source directory
 *
 * @param leaf_name target leaf name for copy
 * @param options Options for the copy
 * @throws std::runtime_error if source is not set
 * @throws OsError if failed to start copy
 */
void FilerAction::copy_local(const std::string &leaf_name, int options /*= NONE*/)
{
	if (_handle == 0) throw std::runtime_error("FilerAction directory not set");
	swix_check(_swix(FilerAction_SendStartOperation, _INR(0,4),
		_handle,
		7, // Copy
		options,
		reinterpret_cast<int>(leaf_name.c_str()),
		leaf_name.size() + 1
		));
}

/**
 * Stamp the object(s) with the current time and date
 *
 * @param options Options for the stamp
 * @throws std::runtime_error if source is not set
 * @throws OsError if failed to start stamp
 *
 */
void FilerAction::stamp(int options /*= NONE*/)
{
	if (_handle == 0) throw std::runtime_error("FilerAction directory not set");
	swix_check(_swix(FilerAction_SendStartOperation, _INR(0,4),
		_handle,
		8, // Stamp
		options,
		0, // Unused
		0
		));
}

/**
 * Find an object and display its location
 *
 * @param find_object object to find in source locations
 * @param options Options for the find
 * @throws std::runtime_error if source is not set
 * @throws OsError if failed to start find
 */
void FilerAction::find(const std::string &find_object, int options /*= NONE*/)
{
	if (_handle == 0) throw std::runtime_error("FilerAction directory not set");
	swix_check(_swix(FilerAction_SendStartOperation, _INR(0,4),
		_handle,
		7, // Copy
		options,
		reinterpret_cast<int>(find_object.c_str()),
		find_object.size() + 1
		));
}


/**
 * Private function to start a filer action task to use for the operation
 *
 * @throws OsError if failed to start filer task
 */
void FilerAction::start()
{
	swix_check(_swix(Wimp_StartTask, _IN(0)|_OUT(0),
			reinterpret_cast<int>("Filer_Action"),
			&_handle
			));
}


/**
 * Turn on or off verbose window while filer action is running
 *
 * Does nothing if filer action is not running
 *
 * @param on true to turn verbose mode on
 */
void FilerAction::verbose(bool on)
{
   if (_handle)
   {
       WimpMessage filer_control_action(0x406, 6);
       filer_control_action.word(5) = on ? 1 : 2;
       filer_control_action.send(WimpMessage::User, _handle);
   }
}

/**
 * Add a listener for when the filer action has finished.
 *
 * This listener must be added after the action has been run or it
 * will be ignored.
 *
 * @param listener listener to add
 */
void FilerAction::add_finished_listener(FilerActionFinishedListener *listener)
{
    if (_handle)
    {
         new FilerActionTaskFinished(_handle, listener);
    }
}


} /* namespace tbx */
