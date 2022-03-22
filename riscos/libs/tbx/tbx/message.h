/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010 Alan Buckley   All Rights Reserved.
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
 * message.h
 *
 *  Created on: 14 Oct 2010
 *      Author: alanb
 */

#ifndef TBX_MESSAGE_H_
#define TBX_MESSAGE_H_

#include <string>

namespace tbx {


std::string message(const std::string &token);
std::string message(const std::string &token, const std::string &arg0, int max_size = 255);
std::string message(const std::string &token, const std::string &arg0, const std::string &arg1, int max_size = 255);
std::string message(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, int max_size = 255);
std::string message(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, const std::string &arg3, int max_size = 255);

std::string gsmessage(const std::string &token, int max_size = 255);
std::string gsmessage(const std::string &token, const std::string &arg0, int max_size = 255);
std::string gsmessage(const std::string &token, const std::string &arg0, const std::string &arg1, int max_size = 255);
std::string gsmessage(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, int max_size = 255);
std::string gsmessage(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, const std::string &arg3, int max_size = 255);

}

#endif /* TBX_MESSAGE_H_ */
