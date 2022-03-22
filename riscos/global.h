/*
 * global.h
 *
 * global application specific defines and structures
 */

#ifndef __global_h
#define __global_h

#include "oslib/os.h"
#include <tbx/hourglass.h>

#ifndef _DEBUG_
#define _DEBUG_ 1
#endif


struct AppState {
  bool IKDEBUG;
  bool ISUPDATED;
  AppState() {
      IKDEBUG = false;
      ISUPDATED = false;
  };
  os_t startup_time = 0;
  int app_poll_period = 3;
  bool is_main_window_shown = false;
  int start_hidden = 0;
};

/* global variables */
extern AppState g_app_state;

void set_app_poll_period(int period);
void g_hourglass_off();
void g_hourglass_on();
void g_hourglass_percentage(int percent);
#endif
