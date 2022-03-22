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

#include <string>

#include "application.h"

namespace tbx {

/**
 * Shortcut to the tbx::app()->messages().message(...)
 */
std::string message(const std::string &token)
{
	return app()->messages().message(token);
}

/**
 * Shortcut to the tbx::app()->messages().message(...)
 */
std::string message(const std::string &token, const std::string &arg0, int max_size /* = 255*/)
{
	return app()->messages().message(token, arg0, max_size);
}

/**
 * Shortcut to the tbx::app()->messages().message(...)
 */
std::string message(const std::string &token, const std::string &arg0, const std::string &arg1, int max_size /*= 255*/)
{
	return app()->messages().message(token, arg0, arg1, max_size);
}
/**
 * Shortcut to the tbx::app()->messages().message(...)
 */
std::string message(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, int max_size /*= 255*/)
{
	return app()->messages().message(token, arg0, arg1, arg2, max_size);
}
/**
 * Shortcut to the tbx::app()->messages().message(...)
 */
std::string message(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, const std::string &arg3, int max_size /*= 255*/)
{
	return app()->messages().message(token, arg0, arg1, arg2, arg3, max_size);
}

/**
 * Shortcut to the tbx::app()->messages().gsmessage(...)
 */
std::string gsmessage(const std::string &token, int max_size /*= 255*/)
{
	return app()->messages().gsmessage(token, max_size);
}

/**
 * Shortcut to the tbx::app()->messages().gsmessage(...)
 */
std::string gsmessage(const std::string &token, const std::string &arg0, int max_size /* = 255*/)
{
	return app()->messages().gsmessage(token, arg0, max_size);
}

/**
 * Shortcut to the tbx::app()->messages().gsmessage(...)
 */
std::string gsmessage(const std::string &token, const std::string &arg0, const std::string &arg1, int max_size /*= 255*/)
{
	return app()->messages().gsmessage(token, arg0, arg1, max_size);
}
/**
 * Shortcut to the tbx::app()->messages().gsmessage(...)
 */
std::string gsmessage(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, int max_size /*= 255*/)
{
	return app()->messages().gsmessage(token, arg0, arg1, arg2, max_size);
}
/**
 * Shortcut to the tbx::app()->messages().gsmessage(...)
 */
std::string gsmessage(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, const std::string &arg3, int max_size /*= 255*/)
{
	return app()->messages().gsmessage(token, arg0, arg1, arg2, arg3, max_size);
}

}

