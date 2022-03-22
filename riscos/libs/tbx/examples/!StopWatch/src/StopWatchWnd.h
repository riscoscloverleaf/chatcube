#ifndef STOPWATCHWND_H_
#define STOPWATCHWND_H_

#include "tbx/timer.h"
#include "tbx/displayfield.h"
#include "tbx/actionbutton.h"
#include "tbx/command.h"

#include "tbx/autocreatelistener.h"

/**
 * Class to set up listeners an variables when the single
 * window is shown.
 */
class StopWatchWnd :
	public tbx::AutoCreateListener,
	public tbx::Timer,
	public tbx::Command
{
private:
	bool _started;
	unsigned int _start_time;
	tbx::DisplayField _display;
	tbx::ActionButton _button;

public:
	StopWatchWnd();
	virtual ~StopWatchWnd();

	virtual void execute();
	virtual void timer(unsigned int elapsed);

	void update_display(unsigned int new_time);

    virtual void auto_created(std::string template_name, tbx::Object object);
};

#endif
