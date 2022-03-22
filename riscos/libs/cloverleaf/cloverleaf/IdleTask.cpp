//
// Created by lenz on 2/15/20.
//
#include "IdleTask.h"

void IdleTask::process_tasks() {
    if (_on_next_idle_run_list.empty()) {
        return;
    }
    int size = _on_next_idle_run_list.size();
    while(size--)  {
        auto task = _on_next_idle_run_list.front();
        task();
        _on_next_idle_run_list.pop_front();
    }
}

IdleTask g_idle_task;