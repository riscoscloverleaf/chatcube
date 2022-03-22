// Reverse main
//
// A.R.Buckley
//
// 15 Sept 2008
//
// Copyright is the same as for TBX (see COPYING in TBX directory for details)
//

#ifndef _REVERSER_H_
#define _REVERSER_H_

#include "tbx/autocreatelistener.h"
#include "tbx/displayfield.h"
#include "tbx/writablefield.h"
#include "tbx/textchangedlistener.h"
#include "tbx/command.h"

/**
 * Class to count writeable field as it changes and update
 * a display field.
 */
class Counter : public tbx::TextChangedListener
{
    tbx::DisplayField _count;

public:
    void display_to(tbx::DisplayField &count) {_count = count;}

    void text_changed(tbx::TextChangedEvent &event);
};

/**
 * Command to reverse the given writeable field
 */
class ReverseCommand : public tbx::Command
{
    tbx::WritableField _reverse;
public:
    void reverse_field(tbx::WritableField &reverse) {_reverse = reverse;}

    virtual void execute();
};

/**
 * Class to tie the counter and reverse commands to the
 * window and gadgets involved.
 */
class Reverser : public tbx::AutoCreateListener
{
    Counter _counter;
    ReverseCommand _reverser;
public:
    virtual void auto_created(std::string template_name, tbx::Object object);
};

#endif
