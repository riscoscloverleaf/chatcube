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
 * fileraction.h
 *
 *  Created on: 21 Feb 2012
 *      Author: alanb
 */

#ifndef TBX_FILERACTION_H_
#define TBX_FILERACTION_H_

#include <string>
#include "listener.h"

namespace tbx {

/**
 * Listener for filer action finished
 */
class FilerActionFinishedListener : public tbx::Listener
{
    public:
       virtual ~FilerActionFinishedListener() {}

       /**
        * Called when the filer action has finished.
        */
       virtual void fileraction_finished() = 0;
};

/**
 * Class to use the RISC OS desktop filer to perform multi-tasking
 * file operations.
 *
 * The actions are set to execute and then returns immediately. The results
 * of the operations are not returned.
 *
 * To use set the directory and objects to work upon then call one of the
 * action methods (e.g. copy, move etc).
 */
class FilerAction
{
	int _handle;
public:
	FilerAction();
	FilerAction(const std::string &object_name);
	FilerAction(const std::string &dir_name, const std::string &objects);
	virtual ~FilerAction();

    /**
     * Return the handle of the filer action task.
     *
     * This will be 0 until one of the action methods is called
     *
     * @return WIMP task handle for filer action task
     */
    int task_handle() const {return _handle;}

	void directory(const std::string &dir_name);
	void add_objects(const std::string &objects);
	void set_object(const std::string &object_name);

	/**
	 * Options for file operations. Combine one or more in the options
	 * parameter for the operation using "|".
	 */
	enum Options
	{
		NONE = 0,    //<! No options
		VERBOSE = 1, //<! Show verbose information
		CONFIRM = 2, //<! Confirm operation
		FORCE = 4,   //<! Force overwrite
		NEWER = 8,   //<! Copy if newer only
		RECURSE = 16 //<! Recurse (applies to access only)
	};

	void copy(const std::string &target_dir, int options = NONE);
	void rename(const std::string &target_dir, int options = NONE);
	void remove(int options = NONE);
	void set_access(int set, int leave, int options = NONE);
	void set_file_type(int type, int options = NONE);
	void count(int options = NONE);
	void move(const std::string &target_dir, int options = NONE);
	void copy_local(const std::string &leaf_name, int options = NONE);
	void stamp(int options = NONE);
	void find(const std::string &find_object, int options = NONE);

    void verbose(bool on);

    void add_finished_listener(FilerActionFinishedListener *listener);

private:
	void start();
};

} /* namespace tbx */

#endif /* TBX_FILERACTION_H_ */
