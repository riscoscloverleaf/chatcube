//
// Created by lenz on 3/19/20.
//

#ifndef ROCHAT_CLRADIOBUTTON_H
#define ROCHAT_CLRADIOBUTTON_H

#include "tbx/button.h"

class CLRadioButton : public tbx::Button {
public:
    CLRadioButton(CLRadioButton &other) : tbx::Button(other) {};
    CLRadioButton(const tbx::Component &other) : tbx::Button(other) {};
    CLRadioButton(const Gadget &other) : tbx::Button(other) {};

    bool on() {
        return (flags() & (1 << 21)) != 0;
    }
    void on(bool value) {
        int clear = (1 << 21);
        int eor;
        if (value) {
            eor = (1 << 21);
        } else {
            eor = 0;
        }
        flags(clear, eor);
    }
};


#endif //ROCHAT_CLRADIOBUTTON_H
