//
// Created by lenz on 3/18/20.
//

#ifndef ROCHAT_CALLBACKLISTENERS_H
#define ROCHAT_CALLBACKLISTENERS_H

#include <functional>
#include <tbx/mouseclicklistener.h>

class CallbackMouseClickListener : public tbx::MouseClickListener {
private:
    std::function<void(tbx::MouseClickEvent &event)> callback;
public:
    CallbackMouseClickListener(std::function<void(tbx::MouseClickEvent &event)> cb) :
        callback(cb), tbx::MouseClickListener() {}

    void mouse_click(tbx::MouseClickEvent &event) override {
        callback(event);
    };
};


#endif //ROCHAT_CALLBACKLISTENERS_H
