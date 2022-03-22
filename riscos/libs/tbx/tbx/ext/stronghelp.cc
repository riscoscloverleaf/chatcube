/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2014 Alan Buckley   All Rights Reserved.
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

#include "stronghelp.h"
#include "../application.h"
#include "../wimpmessagelistener.h"
#include "../reporterror.h"
#include "../command.h"

#include <stdlib.h>

namespace tbx {
namespace ext {

std::string StrongHelp::_default_path;

class StrongHelpMessageHelper;


/**
 * Construct class to control StrongHelp application
 *
 * By default the path is set to the default path
 */
StrongHelp::StrongHelp()
{
   _path = _default_path;
}

/**
 * Construct class with path to help file
 *
 * @param path can be "" to scan all loaded manuals
 *        A comma separated list of help manuals to scan. If the list
 *        ends with a comma the rest of the manuals will be scannced
 *        after the list.
 *        The complete path to the image file (used for program help)
 */
StrongHelp::StrongHelp(const std::string &path) : _path(path)
{
}

/**
 * Set the static default path for use with the no argument
 * constructor.
 * This allows you to set the help file once for that whole application
 *
 * @param path the default path to use
 */
void StrongHelp::default_path(const std::string &path)
{
  _default_path = path;
}

/**
 * Set the static default path for the no argument constructor
 * to be the standard !Help file for the application
 */
void StrongHelp::standard_default_path()
{
   _default_path = tbx::app()->directory() + ".!Help";
}

/**
 * Construct as a copy of another StrongHelp object
 *
 * @param other object to copy
 */
StrongHelp::StrongHelp(const StrongHelp &other) :
     _path(other._path)
{
}

/**
* Attempt to start the strong help program
*
* This will try to run the !StrongHlp application if it has been
* seen by the filer.
*
* @returns true if the launch location was known so it could be launched.
*/
bool StrongHelp::run_stronghelp()
{
  if (getenv("StrongHelp$Dir") != 0)
  {
    try
    {
      tbx::app()->os_cli("Filer_Run <StrongHelp$Dir>.!Run");
      return true;
    } catch(...)
    {
      return false;
    }
  }
  return false;
}

/**
* Attempt to  run the file name in path
*
* This will run the path set in the constructor and rely on the filer
* to use it's file type to display it.
*
* The path in the constructor must have been set to a the full filename
* of the file to launch.
*
* It is a simple way to launch the root page of a strong help file.
*
* To provide more information and to allow fallbacks use the lookup
* call with the name of the root page (usually !root).
*
* @returns true if strong help has been seen by the filer, so there is
* a good chance it will run.
*/
bool StrongHelp::run_path()
{
  if (getenv("StrongHelp$Dir") != 0)
  {
    try
    {
       tbx::app()->os_cli("Filer_Run " + _path);
       return true;
    } catch(...)
    {
       return false;
    }
  }
  return false;
}

/**
* Handler for if the page or text could not be found by StrongHelp.
*
* The default handler just returns false so strong help shows a message
*
* @param help StrongHelp class with details of path used.
* @param page_or_text page or text that was searched for
* @returns return true if you have informed the user of the error or false
* to use the default StrongHelp not found message.
*/
bool StrongHelpNotFoundHandler::stronghelp_not_found(StrongHelp &help, const std::string &page_or_text)
{
   return false;
}

/**
* Handler for if the StrongHelp program could not be run.
*
* Default show a simple error box
*
* @param help StrongHelp class with details of path used
* @param page_or_text page or text that was searched for
*/
void StrongHelpNotFoundHandler::stronghelp_not_running(StrongHelp &help, const std::string &page_or_text)
{
   tbx::report_error("Unable to launch StrongHelp to show the help");
}

//! @cond INTERNAL
/*
 * Private class to handle processing of strong help messages
 */
class StrongHelpMessageHelper :
   public tbx::WimpAcknowledgeMessageListener,
   public tbx::WimpRecordedMessageListener,
   public tbx::Command
{
   StrongHelp _help;
   bool _lookup;
   std::string _page_or_text;
   StrongHelpNotFoundHandler *_handler;
   enum State {FIRST_GO, TRY_AGAIN, SECOND_GO} _state;
   int _myref;

 public:
   StrongHelpMessageHelper(StrongHelp &help, bool lookup, const std::string &page_or_text, StrongHelpNotFoundHandler *handler) :
      _help(help),
      _lookup(lookup),
      _page_or_text(page_or_text),
      _handler(handler)
   {
      tbx::app()->add_acknowledge_message_listener(0x43b00, this);
      tbx::app()->add_recorded_message_listener(0x43b01, this);
      _state = FIRST_GO;
      send_help_message();
   }

   virtual ~StrongHelpMessageHelper()
   {
      tbx::app()->remove_acknowledge_message_listener(0x43b00, this);
      tbx::app()->remove_recorded_message_listener(0x43b01, this);
   }

   void send_help_message()
   {
      std::string text(_lookup ? "Help_Word" : "Help_Search");
      if (!_help.path().empty()) text += " " + _help.path();
      text += " " + _page_or_text;
      int size = ((text.size()+3)>>2)+5;
      tbx::WimpMessage help_mess(0x43b00, size);
      text.copy(help_mess.str(5), text.size());
      help_mess.str(5)[text.size()] = 0;

      help_mess.send(tbx::WimpMessage::Recorded, tbx::WimpMessage::Broadcast);
      _myref = help_mess.my_ref();
      tbx::app()->add_idle_command(this);
   }

   /**
   * StrongHelp responded to 0x43b00 with 0x43b01 as not found
   */
   void recorded_message(tbx::WimpMessageEvent &event, int reply_to)
   {
      if (event.message().your_ref() == _myref && _handler != 0)
      {
         tbx::app()->remove_idle_command(this);
         if (_handler->stronghelp_not_found(_help, _page_or_text))
         {
            // User showed something so acknowledge message so strong help
            // doesn't show the not found box
            tbx::WimpMessage reply(event.message());
            reply.your_ref(reply.my_ref());
            reply.send(tbx::WimpMessage::Acknowledge, reply_to);
         }
         delete this;
      }
   }

   /**
    * Wimp replied to 0x43b00 as StrongHelp didn't respond
    */
   void acknowledge_message(tbx::WimpMessageEvent &event)
   {
      if (event.message().my_ref() == _myref)
      {
         tbx::app()->remove_idle_command(this);
         bool failed = false;
         if (_state == FIRST_GO)
	     {
	        _state = TRY_AGAIN;
	        if (StrongHelp::run_stronghelp())
	        {
	           tbx::app()->add_idle_command(this);
	        } else
	        {
	           failed = true;
	        }
	     } else if (_state == SECOND_GO)
	     {
	        failed = true;
	     }

         if (failed)
         {
            if (_handler)
            {
               _handler->stronghelp_not_running(_help, _page_or_text);
            } else
            {
 	           StrongHelpNotFoundHandler default_handler;
	           default_handler.stronghelp_not_running(_help, _page_or_text);
	        }
	        delete this;
	     }
	  }
   }

   virtual void execute()
   {
     tbx::app()->remove_idle_command(this);
     switch(_state)
     {
     case TRY_AGAIN:
       {
         _state = SECOND_GO;
         send_help_message();
       }
       break;

     case FIRST_GO:
       // No other message so self destruct
       delete this;
       break;

     case SECOND_GO:
       // Second attempt succeeded so delete
       delete this;
       break;
     }

   }
};

//! @endcond

/**
* Lookup a page in the strong help path
*
* @param page page to look up
* @param handler class to handle not found/not running errors or 0 for
* the default processing.
*/
void StrongHelp::lookup(const std::string &page, StrongHelpNotFoundHandler *handler/*= 0*/)
{
  new StrongHelpMessageHelper(*this, true, page, handler);
}

/**
* Open the free text search window
*
* @param text text to search forpage
* @param handler class to handle not found/not running errors or 0 for
* the default processing.
*/
void StrongHelp::search(const std::string &text, StrongHelpNotFoundHandler *handler /*= 0*/)
{
  new StrongHelpMessageHelper(*this, false, text, handler);
}

  } // ext namespace
} // tbx namespace
