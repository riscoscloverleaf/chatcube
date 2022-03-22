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

/*
 * posteventlistener.h
 *
 *  Created on: 6 Jun 2014
 *      Author: alanb
 */

#ifndef TBX_POSTEVENTLISTENER_H_
#define TBX_POSTEVENTLISTENER_H_

#include "listener.h"
#include "pollinfo.h"

namespace tbx
{

/**
 * Class to provide a listener to allow processing before the call to wimp poll
 *
 * There can only be one of these listeners and it is provided to give a hook
 * for extra low-level processing required to implement some features.
 */
class PostEventListener : public Listener
{
   public:
      virtual ~PostEventListener() {};

   /**
    * Method called after the call to Wimp_Poll and any events have
    * been processed.
    *
    * Note: The parameters passed may have been modified by the even processing
    *
    * @param reason_code poll reason code
    * @param poll_block pointer to an array of up to 64 integers with the
    * poll block updated by Wimp Poll. The contents depend on the reason
    * code.
    * @param id_block Toolbox id's for this message (if set)
    * @param reply_to for reason_code = 18 this is the task to reply to.
    */
   virtual void post_event(int reason_code, const PollBlock &poll_block, const IdBlock &id_block, int reply_to) = 0;
};

}



#endif /* POSTEVENTLISTENER_H_ */
