//
// Created by lenz on 3/16/20.
//

#ifndef ROCHAT_APPMENU_H
#define ROCHAT_APPMENU_H

#include <tbx/menu.h>
#include <tbx/eventinfo.h>
#include <tbx/menuselectionlistener.h>
#include <tbx/abouttobeshownlistener.h>
#include <tbx/autocreatelistener.h>
#include "../model/AppDataModel.h"

class MainMenu : public tbx::AboutToBeShownListener {
private:
    tbx::Menu menu;
public:
    MainMenu(tbx::Object object);
    void about_to_be_shown(tbx::AboutToBeShownEvent &event) override;
};


class MessengersMenu : public tbx::AboutToBeShownListener {
private:
    const int CID_TELEGRAM = 0;
    tbx::Menu menu;
public:
    MessengersMenu(tbx::Object object);
    void about_to_be_shown(tbx::AboutToBeShownEvent &event) override;
};

class CreateChatMenu : public tbx::AboutToBeShownListener {
private:
    tbx::Menu menu;
    const int CID_TELEGRAM = 2;
public:
    CreateChatMenu(tbx::Object object);
    void about_to_be_shown(tbx::AboutToBeShownEvent &event) override;
};

#endif //ROCHAT_APPMENU_H
