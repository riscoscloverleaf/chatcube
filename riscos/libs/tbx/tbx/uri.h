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

#ifndef TBX_URI_H_
#define TBX_URI_H_

#include <string>
#include "wimpmessagelistener.h"

namespace tbx
{
class URIResultHandler;

/**
 * Class to take a URI and dispatch it to the Wimp for processing
 *
 * To use successfully the AcornURI module must be loaded and started.
 * Either add the correct field to the !Run file or use the method
 * ensure_uri_handler in this class.
 */
class URI :
   private tbx::WimpRecordedMessageListener
{
   public:
      URI(const std::string &uri);
      virtual ~URI();

      void uri(const std::string &new_uri);

      static int version();
      bool dispatch();
      bool check();
      void set_result_handler(URIResultHandler *handler);

      static bool dispatch(const std::string &uri);

      enum State {NotDispatched, Dispatched, DispatchError, WaitingForResult, Claimed, NotClaimed};
      State state() const {return _state;}

      static bool ensure_uri_handler();
      static bool uri_handler_started();

   private:
       virtual void recorded_message(tbx::WimpMessageEvent &event, int reply_to);

   private:
      std::string _uri;
      int _uri_handle;
      URIResultHandler *_handler;
      State _state;
 };

/**
 * Handler to check if a URI was successfully launched
 */
class URIResultHandler
{
   public:
      URIResultHandler() {}
      virtual ~URIResultHandler() {}

      /**
       * Called once the Wimp has attempted to launch
       * the URI dispatcher
       * @param uri_handle handle from the launch
       * @param claimed true if the dispatched message has been claimed
       */
      virtual void uri_result(URI &uri, bool claimed) = 0;
};

}

#endif
