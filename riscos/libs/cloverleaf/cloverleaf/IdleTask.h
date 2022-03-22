//
// Created by lenz on 2/15/20.
//

#ifndef CL_IDLETASK_H
#define CL_IDLETASK_H
#include <list>
#include <functional>

class IdleTask {
private:
    std::list<std::function<void()>> _on_next_idle_run_list;
public:
    void run_at_next_idle(std::function<void()> fun) {
        _on_next_idle_run_list.push_back(fun);
    }
    void process_tasks();
};

extern IdleTask g_idle_task;

#endif //CL_IDLETASK_H
