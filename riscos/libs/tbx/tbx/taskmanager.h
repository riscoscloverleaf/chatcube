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

#ifndef TBX_TASKMANAGER
#define TBX_TASKMANAGER

#include <string>
#include <vector>

#include "handles.h"

namespace tbx
{

/**
 * Structure containing information on a task
 */
struct TaskInfo
{
  std::string name;    ///< Name of the task
  TaskHandle handle;   ///< Handle of the task
  unsigned int memory; ///< Memory used by the task
  unsigned int flags;  ///< A combination of one or more of the TaskFlags
  /**
  * Values for the flags parameter can be combined
  */
  enum TaskFlags
  {
    None, ///< No flags set
    ModuleTask = 1, // Set for module task, unset application task
    ResizeSlot = 2  // Set if slot bar can be dragged
  };
};

/**
 * Class to find and enumerate running tasks
 */
class TaskManager
{
  /**
  * Private class to enumerate tasks
  */
  class Enumerator
  {
    int _buffer[128];
    int _call_value;
    int *_current;
    int *_end;

    public:
    Enumerator();
    bool more() {return _current != _end || _call_value >= 0;}
    bool next();

    TaskHandle handle() {return _current[0];}
    std::string name() const;
    unsigned int memory() {return _current[2];}
    unsigned int flags() {return _current[3];}
  };

  public:
  bool running(const std::string &task_name) const;
  std::string name(TaskHandle handle) const;
  TaskHandle find_first(const std::string &task_name) const;
  bool find_first(TaskInfo &info, const std::string &task_name) const;
  bool find_all(std::vector<TaskInfo> &infos, const std::string &task_name) const;
  void list(std::vector<TaskInfo> &infos) const;
};

};

#endif
