/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2019 Alan Buckley   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "uri.h"
#include "application.h"
#include "swis.h"
#include "kernel.h"

namespace tbx
{

const int URI_MReturnResult = 0x4E383;

// Cache of URI handler started result
// 0 - not checked
// 1 - started OK
// 2 - failed to start
static int s_uri_handler_started = 0;

/**
 * Create a URI object to dispatch a URI
 *
 * @param: uri to dispatch
 */
URI::URI(const std::string &uri) :
   _uri(uri),
   _uri_handle(0),
   _handler(0),
   _state(URI::NotDispatched)
{
}

/**
 * Reset the URI
 *
 * If the previous URI had been dispatched, this will stop the
 * result handler being called for the old uri.
 *
 * @param new_uri the new URI to use
 */
void URI::uri(const std::string &new_uri)
{
   if (_state == WaitingForResult)
   {
      tbx::app()->remove_recorded_message_listener(URI_MReturnResult, this);
   }
   _uri = new_uri;
   _state = NotDispatched;
    _uri_handle = 0;
}

/**
 * When the object is destroyed the result handler is cancelled.
 */
URI::~URI()
{
   if (_state == WaitingForResult)
   {
      tbx::app()->remove_recorded_message_listener(URI_MReturnResult, this);
   }
}

/**
 * Get the URI version
 *
 * @return URI module version * 100 or 0 on error.
 */
int URI::version()
{
    int ver = 0;
    if (_swix(0x4E380, _IN(0)|_OUT(0), 0, &ver))
    {
       ver = 0;
    }
    return ver;
}

/**
 * Dispatch the URI
 *
 * The result of this call only fails on the initial dispatch
 * use set_result_handler to check if it was claimed.
 *
 * @return true if dispatched, false otherwise
 */
bool URI::dispatch()
{
   int flags = (_handler) ? 1 : 0;
   int result = 0;
   if (_swix(0x4E381, _INR(0,2) | _OUT(0) | _OUT(3), flags,
       reinterpret_cast<int>(_uri.c_str()),
       tbx::app()->task_handle(),
       &result, &_uri_handle))
   {
       // Error
       result = 1;
       _uri_handle = 0;
       _state = DispatchError;
   }
   if (result == 0)
   {
      if (_handler)
      {
         _state = WaitingForResult;
         tbx::app()->add_recorded_message_listener(URI_MReturnResult, this);
      } else
      {
         _state = Dispatched;
      }
   }

   return (result == 0);
}

/**
 * Check if an application is open that can process the URI
 *
 * The result of this call only fails on the initial dispatch
 * use set_result_handler to check if it would be claimed.
 *
 * returns true if check can start, false otherwise
 */
bool URI::check()
{
   int flags = 2 | 4 | ((_handler) ? 1 : 0);
   int result = 0;
   if (_swix(0x4E381, _INR(0,2) | _OUT(0) | _OUT(3), flags,
       reinterpret_cast<int>(_uri.c_str()),
       tbx::app()->task_handle(),
       &result, &_uri_handle))
   {
       // Error
       result = 1;
       _uri_handle = 0;
       _state = DispatchError;
   }
   if (result == 0)
   {
      if (_handler)
      {
         _state = WaitingForResult;
         tbx::app()->add_recorded_message_listener(URI_MReturnResult, this);
      } else
      {
         _state = Dispatched;
      }
   }

   return (result == 0);
}

/**
 * Set the object to handle the result status of a dispatch
 * or check call.
 *
 * The handler must be set before the dispatch or check call
 * to be actioned.
 * @param handler interface to handle the result or nullptr to clear the handler.
 */
void URI::set_result_handler(URIResultHandler *handler)
{
   _handler = handler;
   if (_state == WaitingForResult && !_handler)
   {
       tbx::app()->remove_recorded_message_listener(URI_MReturnResult, this);
       _state = Dispatched;
   }
}

/**
 * static method to give a simple method of launching a uri
 *
 * @return true if uri is dispatched (this doesn't mean it has been claimed by anything)
 */
bool URI::dispatch(const std::string &uri)
{
    URI u(uri);
    return u.dispatch();
}

/**
 * Ensure the URI handler is started
 *
 * Checks if the URI handler (AcornURI) has been started and
 * attempts to start it if it has not.
 *
 * This can only be called from a Wimp task after the application has
 * been initialised.
 *
 * Caches the result so does nothing after the first successful call.
 *
 * @return true if successful
 */
bool URI::ensure_uri_handler()
{
   if (s_uri_handler_started) return (s_uri_handler_started == 1);
   if (_kernel_oscli("RMEnsure AcornURI 0.01"))
   {
      // Module not loaded
      if (_swix(OS_Module, _INR(0,1), 1, "System:Modules.Network.URI"))
      {
         // Still can't load it
         s_uri_handler_started = 2;
         return false;
      }
      s_uri_handler_started = 1;
      // Force URI module task to start
      _swix(Wimp_StartTask, _IN(0), reinterpret_cast<int>("Desktop"));
   }
  return (s_uri_handler_started == 1);
}

/**
 * Check if the uri_handler has been started
 *
 * If not started from this task, will use version() to check if it exists
 * and caches the result.
 *
 * @returns true if it has been started
 */
bool URI::uri_handler_started()
{
   if (s_uri_handler_started) return (s_uri_handler_started == 1);

   if (version() != 0) s_uri_handler_started = true;

   return (s_uri_handler_started == 1);
}


//! @cond INTERNAL

/*
 * private message handler to deal look for URI_MReturnResult
 */
void URI::recorded_message(tbx::WimpMessageEvent &event, int reply_to)
{
    int flags = event.message()[5];
    int handle = event.message()[6];
    if (handle == _uri_handle)
    {
       _state = (flags & 1) ? NotClaimed : Claimed;
       tbx::app()->remove_recorded_message_listener(URI_MReturnResult, this);
       if (_handler) _handler->uri_result(*this, ((flags&1)==0));
       // Invalidate handle
       _swix(0x4E383, _IN(0) | _IN(3), 0, _uri_handle);
    }
}

//! @endcond INTERNAL

}
