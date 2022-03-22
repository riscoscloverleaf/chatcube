
#include "StopWatchWnd.h"
#include "tbx/window.h"

using namespace tbx;

#include "tbx/application.h"
#include "tbx/monotonictime.h"
#include <sstream>
#include <iomanip>

StopWatchWnd::StopWatchWnd()
{
	_started = false;
}

StopWatchWnd::~StopWatchWnd()
{
	if (_started) tbx::app()->remove_timer(this);
}


/**
 * Set up listeners and commands for the window.
 */
void StopWatchWnd::auto_created(std::string template_name, Object object)
{
    Window window(object);
    _display = window.gadget(0);
    _button = window.gadget(1);
   	_button.add_selected_command(this);
}

/**
 * Execute is attached to the start/stop button
 * and starts and stops the stop watch
 */
void StopWatchWnd::execute()
{
	if (_started)
	{
		// Stop the stop watch
		tbx::app()->remove_timer(this);
		update_display(tbx::monotonic_elapsed(_start_time, tbx::monotonic_time()));
		_button.text("Start");
		_started = false;
	} else
	{
		_start_time = tbx::monotonic_time();
		update_display(0);
		_button.text("Stop");
		// Update display every 10 centiseconds
		tbx::app()->add_timer(10, this);
		_started = true;
	}
}

/**
 * Called back approximately every 10 seconds
 */
void StopWatchWnd::timer(unsigned int elapsed)
{
	update_display(tbx::monotonic_elapsed(_start_time, tbx::monotonic_time()));
}

/**
 * Update the stopwatch display
 */
void StopWatchWnd::update_display(unsigned int new_time)
{
	std::ostringstream os;
	os << (new_time / 6000)
	   << ":" << std::setw(2) << std::setfill('0') << ((new_time / 100) % 60)
	   << "." << std::setw(1) << ((new_time / 10) % 10);

	_display.text(os.str());
}
