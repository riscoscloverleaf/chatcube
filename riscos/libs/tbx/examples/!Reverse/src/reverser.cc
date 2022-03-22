// Reverser class
// Sets up and processes the events for the reverse toolbox window
//
// A.R.Buckley
//
// 15 Sept 2008
//
// Copyright is the same as for TBX (see COPYING in TBX directory for details)
//

#include "res.h"
#include "reverser.h"
#include <sstream>

#include <iostream>

using namespace tbx;

/**
 * Set up listeners and commands for the window.
 */
void Reverser::auto_created(std::string template_name, Object object)
{
    Window window(object);
    WritableField input(window.gadget(CID_Text));
    DisplayField count(window.gadget(CID_Display));

  _counter.display_to(count);
    input.add_text_changed_listener(&_counter);

    _reverser.reverse_field(input);
    window.add_command(EID_ReverseText, &_reverser);
}

/**
 * Count the length of the text field and display it
 */
void Counter::text_changed(TextChangedEvent &event)
{
    std::ostringstream ss;
    ss << event.text().length();
   _count.text(ss.str());
}

/**
 * Reverse the text field
 */
void ReverseCommand::execute()
{
    std::string value = _reverse.text();
    std::string reverse;
    reverse.append(value.rbegin(), value.rend());
    _reverse.text(reverse);
}

