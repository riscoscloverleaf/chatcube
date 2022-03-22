/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2014 Alan Buckley   All Rights Reserved.
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

#include "taskmanager.h"
#include "swixcheck.h"
#include "swis.h"

namespace tbx
{

  /**
  * Check if a task is running
  *
  * @param task_name name of task to search for
  * @returns true if the task is running
  */
  bool TaskManager::running(const std::string &task_name) const
  {
     return (find_first(task_name) != 0);
  }

  /**
  * Return name of task from a handle
  *
  *@param handle TaskHandle to get name for
  *@returns name of task
  *@throws tbx::OsError invalid task handle
  */
  std::string TaskManager::name(TaskHandle handle) const
  {
    char *name;
    swix_check(_swix(TaskManager_TaskNameFromHandle, _IN(0)|_OUT(0), handle,&name));
    return std::string(name);
  }

  /**
  * Find the first running task with the given name
  *
  *@param task_name name of task to find
  *@returns Handle to task or 0 if not found
  */
TaskHandle TaskManager::find_first(const std::string &task_name) const
{
  Enumerator e;
  while (e.next() && task_name != e.name());
  return (e.more() ? e.handle() : 0);
}

/**
* Find first running task with given name and return full info
*
* @param info task information filled in if task name found
* @param task_name name of task to find
* @returns true if task is found in which case info is valid.
*/
bool TaskManager::find_first(TaskInfo &info, const std::string &task_name) const
{
  Enumerator e;
  while (e.next() && task_name != e.name());
  if (e.more())
  {
    info.name = e.name();
    info.handle = e.handle();
    info.memory = e.memory();
    info.flags = e.flags();
    return true;
  }
  return false;
}

/**
* Find all running tasks with given name and return full info
*
* @param infos vector of task information for found tasks
* @param task_name name of task to find
* @returns true if on or more tasks were.
*/
bool TaskManager::find_all(std::vector<TaskInfo> &infos, const std::string &task_name) const
{
  Enumerator e;
  TaskInfo info;
  while (e.next())
  {
    if (task_name == e.name())
    {
       info.name = e.name();
       info.handle = e.handle();
       info.memory = e.memory();
       info.flags = e.flags();
       infos.push_back(info);
    }
  }
  return !infos.empty();
}

/**
* Get list of all running tasks
*
*@param info task information for all tasks
*/
void TaskManager::list(std::vector<TaskInfo> &infos) const
{
  Enumerator e;
  TaskInfo info;
  while (e.next())
  {
     info.name = e.name();
     info.handle = e.handle();
     info.memory = e.memory();
     info.flags = e.flags();
     infos.push_back(info);
  }
}

  /**
  * Initialise enumerator ready for first call
  */
TaskManager::Enumerator::Enumerator() :
     _call_value(0),
     _current(_buffer-4),
     _end(_buffer)
{
}

/**
* Move enumerator to next item in task list
*@returns true if more to come
*/
bool TaskManager::Enumerator::next()
{
  _current += 4;
  if (_current == _end && _call_value >= 0)
  {
     swix_check(_swix(TaskManager_EnumerateTasks, _INR(0,2) | _OUTR(0,1),
        _call_value, _buffer, sizeof(_buffer),
        &_call_value, &_end));
     _current = _buffer;
  }

  return more();
}


/**
 * Get task name from enumerator
 *
 * @param returns the name
 */
std::string TaskManager::Enumerator::name() const
{
	const unsigned char *p = reinterpret_cast<const unsigned char *>(_current[1]);
	const unsigned char *end = p;
	// If appears name may be delimited by any control character instead of char(0)
	while (*end >= 32) end++;
	return std::string((const char *)p, end-p);
}

}



