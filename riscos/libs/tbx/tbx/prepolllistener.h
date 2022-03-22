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

 * prepolllistener.h
 *
 *  Created on: 6 Jun 2014
 *      Author: alanb
 */

#ifndef TBX_PREPOLLLISTENER_H_
#define TBX_PREPOLLLISTENER_H_

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
class PrePollListener : public Listener
{
   public:
      virtual ~PrePollListener() {};

      /**
       * Method called immediately before the call to Wimp_Poll
       *
       * @returns true to change the poll to being a null event poll, false otherwise.
       */
      virtual bool pre_poll() = 0;
};

}

#endif /* TBX_PREPOLLLISTENER_H_ */
