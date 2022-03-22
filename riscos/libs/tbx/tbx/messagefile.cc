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
 * messagefile.cc
 *
 *  Created on: 14 Oct 2010
 *      Author: alanb
 */

#include "messagefile.h"
#include <swis.h>
#include "swixcheck.h"

namespace tbx {

/**
 * Unused message file constructor.
 *
 * call open to open a message file before use
 */
MessageFile::MessageFile()
{
	_status = CLOSED;
	_buffer = 0;
}

/**
 * Construct and load given file
 *
 * use is_open to check if the file opened OK
 *
 * @param file_name name of file to open
 */
MessageFile::MessageFile(const std::string &file_name)
{
	_status = CLOSED;
	_buffer = 0;
	open(file_name);
}

/**
 * Construct given an already created message file descriptor block.
 *
 * This class does not own this block so will not close the file.
 */
MessageFile::MessageFile(int *messageFD)
{
	for (int j = 0; j < 4; j++) _messageFD[j] = *messageFD++;
	_status = ATTACHED;
	_buffer = 0;
}

/**
 * Destructor - close file if opened with constructor or open function
 */
MessageFile::~MessageFile()
{
	if (_status == OPENED) close();
}

/**
 * Open a messages file.
 *
 * @param file_name file name to open
 * @returns true if message file opened OK
 */
bool MessageFile::open(const std::string &file_name)
{
	if (_status == OPENED) close();
	int flags, size;
	if (_swix(MessageTrans_FileInfo, _IN(1) |_OUT(0)|_OUT(2), file_name.c_str(), &flags, &size ) == 0)
	{
		_buffer = new char[size];
		if (_swix(MessageTrans_OpenFile, _INR(0,2), _messageFD, file_name.c_str(), _buffer) == 0)
		{
			_status = OPENED;
		}
	}
	return (_status == OPENED);
}

/**
 * Closes the message file if its has been successfully opened
 */
void MessageFile::close()
{
	if (_status == OPENED)
	{
		swix_check(_swix(MessageTrans_CloseFile, _IN(0), _messageFD));
		_status = CLOSED;
		delete [] _buffer;
		_buffer = 0;
	}
}

/**
 * Attach this messages file class to the given message file descriptor.
 *
 * Does not close the given the file in any circumstances.
 */
void MessageFile::attach(int *messageFD)
{
	if (_status == OPENED) close();
	for (int j = 0; j < 4; j++) _messageFD[j] = *messageFD++;
	_status = ATTACHED;
}

/**
 * Checks to see if the message file contains the given token
 *
 * @param token name of token to check
 * @returns true if token is in message file.
 */
bool MessageFile::contains(const std::string &token) const
{
	_kernel_swi_regs regs;
	regs.r[0] = reinterpret_cast<int>(_messageFD);
	regs.r[1] = reinterpret_cast<int>(token.c_str());
	regs.r[2] = 0; // No buffer needed here
	regs.r[3] = 0; // No buffer needed here
	regs.r[4] = 0; // pointer to parameter %0. No substitution required here.
	regs.r[5] = 0; // pointer to parameter %1. No substitution required here.
	regs.r[6] = 0; // pointer to parameter %2. No substitution required here.
	regs.r[7] = 0; // pointer to parameter %3. No substitution required here.
	return (_kernel_swi(MessageTrans_Lookup, &regs, &regs) == 0);
}

/**
 * Get the string with the given token name without argument substitution
 *
 * @param token token name to look up in message file. A default message can be
 * added to this parameter by appending to the token name a colon (:) and then
 * the default message.
 *
 * @throws tbx::OsError if the message does not exist and no default is given
 */
std::string MessageFile::message(const std::string &token) const
{
	_kernel_swi_regs regs;
	regs.r[0] = reinterpret_cast<int>(_messageFD);
	regs.r[1] = reinterpret_cast<int>(token.c_str());
	regs.r[2] = 0; // No buffer needed here
	regs.r[3] = 0; // No buffer needed here
	regs.r[4] = 0; // pointer to parameter %0. No substitution required here.
	regs.r[5] = 0; // pointer to parameter %1. No substitution required here.
	regs.r[6] = 0; // pointer to parameter %2. No substitution required here.
	regs.r[7] = 0; // pointer to parameter %3. No substitution required here.
	swix_check(_kernel_swi(MessageTrans_Lookup, &regs, &regs));

	const char *result = reinterpret_cast<const char *>(regs.r[2]);
	return std::string(result, regs.r[3]);
}

/**
 * Get the string with the given token name and one argument substituted
 *
 * @param token token name to look up in message file. A default message
 *        can be added to this parameter by appending to the token name
 *        a colon (:) and then the default message.
 * @param arg0 This string will replace "%0" in the returned message.
 * @param max_size Maximum size for the string that will be returned.
 *        defaults to 255 characters
 * @throws tbx::OsError if the message does not exist and no default is given
 */
std::string MessageFile::message(const std::string &token, const std::string &arg0, int max_size /*= 255*/) const
{
	char buffer[max_size+1];

	_kernel_swi_regs regs;
	regs.r[0] = reinterpret_cast<int>(_messageFD);
	regs.r[1] = reinterpret_cast<int>(token.c_str());
	regs.r[2] = reinterpret_cast<int>(buffer);
	regs.r[3] = max_size+1; // No buffer needed here
	regs.r[4] = reinterpret_cast<int>(arg0.c_str()); // pointer to parameter %0.
	regs.r[5] = 0; // pointer to parameter %1. No substitution required here.
	regs.r[6] = 0; // pointer to parameter %2. No substitution required here.
	regs.r[7] = 0; // pointer to parameter %3. No substitution required here.
	swix_check(_kernel_swi(MessageTrans_Lookup, &regs, &regs));

	return std::string(buffer);
}

/**
 * Get the string with the given token name and two arguments substituted
 *
 * @param token token name to look up in message file. A default message
 *        can be added to this parameter by appending to the token name
 *        a colon (:) and then the default message.
 * @param arg0 This string will replace "%0" in the returned message.
 * @param arg1 This string will replace "%1" in the returned message.
 * @param max_size Maximum size for the string that will be returned.
 *        defaults to 255 characters
 * @throws tbx::OsError if the message does not exist and no default is given
 */
std::string MessageFile::message(const std::string &token, const std::string &arg0, const std::string &arg1, int max_size /*= 255*/) const
{
	char buffer[max_size+1];

	_kernel_swi_regs regs;
	regs.r[0] = reinterpret_cast<int>(_messageFD);
	regs.r[1] = reinterpret_cast<int>(token.c_str());
	regs.r[2] = reinterpret_cast<int>(buffer);
	regs.r[3] = max_size+1; // No buffer needed here
	regs.r[4] = reinterpret_cast<int>(arg0.c_str()); // pointer to parameter %0.
	regs.r[5] = reinterpret_cast<int>(arg1.c_str()); // pointer to parameter %1.
	regs.r[6] = 0; // pointer to parameter %2. No substitution required here.
	regs.r[7] = 0; // pointer to parameter %3. No substitution required here.
	swix_check(_kernel_swi(MessageTrans_Lookup, &regs, &regs));

	return std::string(buffer);
}

/**
 * Get the string with the given token name and three arguments substituted
 *
 * @param token token name to look up in message file. A default message
 *        can be added to this parameter by appending to the token name
 *        a colon (:) and then the default message.
 * @param arg0 This string will replace "%0" in the returned message.
 * @param arg1 This string will replace "%1" in the returned message.
 * @param arg2 This string will replace "%2" in the returned message.
 * @param max_size Maximum size for the string that will be returned.
 *        defaults to 255 characters
 * @throws tbx::OsError if the message does not exist and no default is given
 */
std::string MessageFile::message(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, int max_size /*= 255*/) const
{
	char buffer[max_size+1];

	_kernel_swi_regs regs;
	regs.r[0] = reinterpret_cast<int>(_messageFD);
	regs.r[1] = reinterpret_cast<int>(token.c_str());
	regs.r[2] = reinterpret_cast<int>(buffer);
	regs.r[3] = max_size+1; // No buffer needed here
	regs.r[4] = reinterpret_cast<int>(arg0.c_str()); // pointer to parameter %0.
	regs.r[5] = reinterpret_cast<int>(arg1.c_str()); // pointer to parameter %1.
	regs.r[6] = reinterpret_cast<int>(arg2.c_str()); // pointer to parameter %2.
	regs.r[7] = 0; // pointer to parameter %3. No substitution required here.
	swix_check(_kernel_swi(MessageTrans_Lookup, &regs, &regs));

	return std::string(buffer);
}

/**
 * Get the string with the given token name and four arguments substituted
 *
 * @param token token name to look up in message file. A default message
 *        can be added to this parameter by appending to the token name
 *        a colon (:) and then the default message.
 * @param arg0 This string will replace "%0" in the returned message.
 * @param arg1 This string will replace "%1" in the returned message.
 * @param arg2 This string will replace "%2" in the returned message.
 * @param arg3 This string will replace "%2" in the returned message.
 * @param max_size Maximum size for the string that will be returned.
 *        defaults to 255 characters
 * @throws tbx::OsError if the message does not exist and no default is given
 */
std::string MessageFile::message(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, const std::string &arg3, int max_size /*= 255*/) const
{
	char buffer[max_size+1];

	_kernel_swi_regs regs;
	regs.r[0] = reinterpret_cast<int>(_messageFD);
	regs.r[1] = reinterpret_cast<int>(token.c_str());
	regs.r[2] = reinterpret_cast<int>(buffer);
	regs.r[3] = max_size+1; // No buffer needed here
	regs.r[4] = reinterpret_cast<int>(arg0.c_str()); // pointer to parameter %0.
	regs.r[5] = reinterpret_cast<int>(arg1.c_str()); // pointer to parameter %1.
	regs.r[6] = reinterpret_cast<int>(arg2.c_str()); // pointer to parameter %2.
	regs.r[7] = reinterpret_cast<int>(arg3.c_str()); // pointer to parameter %3.
	swix_check(_kernel_swi(MessageTrans_Lookup, &regs, &regs));

	return std::string(buffer);
}

/**
 * Get the string with the given token name and converts it from the
 * GSTrans format.
 *
 * @param token token name to look up in message file. A default message
 *        can be added to this parameter by appending to the token name
 *        a colon (:) and then the default message.
 * @param max_size Maximum size for the string that will be returned.
 *        defaults to 255 characters
 * @throws tbx::OsError if the message does not exist and no default is given
 */
std::string MessageFile::gsmessage(const std::string &token, int max_size /*= 255*/) const
{
	char buffer[max_size+1];

	_kernel_swi_regs regs;
	regs.r[0] = reinterpret_cast<int>(_messageFD);
	regs.r[1] = reinterpret_cast<int>(token.c_str());
	regs.r[2] = reinterpret_cast<int>(buffer);
	regs.r[3] = max_size+1; // No buffer needed here
	regs.r[4] = 0; // pointer to parameter %0. No substitution required here.
	regs.r[5] = 0; // pointer to parameter %1. No substitution required here.
	regs.r[6] = 0; // pointer to parameter %2. No substitution required here.
	regs.r[7] = 0; // pointer to parameter %3. No substitution required here.
	swix_check(_kernel_swi(MessageTrans_GSLookup, &regs, &regs));

	return std::string(buffer);
}

/**
 * Get the string with the given token name and one argument substituted
 * and converts it from the GSTrans format.
 *
 * @param token token name to look up in message file. A default message
 *        can be added to this parameter by appending to the token name
 *        a colon (:) and then the default message.
 * @param arg0 This string will replace "%0" in the returned message.
 * @param max_size Maximum size for the string that will be returned.
 *        defaults to 255 characters
 * @throws tbx::OsError if the message does not exist and no default is given
 */
std::string MessageFile::gsmessage(const std::string &token, const std::string &arg0, int max_size /*= 255*/) const
{
	char buffer[max_size+1];

	_kernel_swi_regs regs;
	regs.r[0] = reinterpret_cast<int>(_messageFD);
	regs.r[1] = reinterpret_cast<int>(token.c_str());
	regs.r[2] = reinterpret_cast<int>(buffer);
	regs.r[3] = max_size+1; // No buffer needed here
	regs.r[4] = reinterpret_cast<int>(arg0.c_str()); // pointer to parameter %0.
	regs.r[5] = 0; // pointer to parameter %1. No substitution required here.
	regs.r[6] = 0; // pointer to parameter %2. No substitution required here.
	regs.r[7] = 0; // pointer to parameter %3. No substitution required here.
	swix_check(_kernel_swi(MessageTrans_GSLookup, &regs, &regs));

	return std::string(buffer);
}

/**
 * Get the string with the given token name and two arguments substituted
 * and converts it from the GSTrans format.
 *
 * @param token token name to look up in message file. A default message
 *        can be added to this parameter by appending to the token name
 *        a colon (:) and then the default message.
 * @param arg0 This string will replace "%0" in the returned message.
 * @param arg1 This string will replace "%1" in the returned message.
 * @param max_size Maximum size for the string that will be returned.
 *        defaults to 255 characters
 * @throws tbx::OsError if the message does not exist and no default is given
 */
std::string MessageFile::gsmessage(const std::string &token, const std::string &arg0, const std::string &arg1, int max_size /*= 255*/) const
{
	char buffer[max_size+1];

	_kernel_swi_regs regs;
	regs.r[0] = reinterpret_cast<int>(_messageFD);
	regs.r[1] = reinterpret_cast<int>(token.c_str());
	regs.r[2] = reinterpret_cast<int>(buffer);
	regs.r[3] = max_size+1; // No buffer needed here
	regs.r[4] = reinterpret_cast<int>(arg0.c_str()); // pointer to parameter %0.
	regs.r[5] = reinterpret_cast<int>(arg1.c_str()); // pointer to parameter %1.
	regs.r[6] = 0; // pointer to parameter %2. No substitution required here.
	regs.r[7] = 0; // pointer to parameter %3. No substitution required here.
	swix_check(_kernel_swi(MessageTrans_GSLookup, &regs, &regs));

	return std::string(buffer);
}

/**
 * Get the string with the given token name and three arguments substituted
 * and converts it from the GSTrans format.
 *
 * @param token token name to look up in message file. A default message
 *        can be added to this parameter by appending to the token name
 *        a colon (:) and then the default message.
 * @param arg0 This string will replace "%0" in the returned message.
 * @param arg1 This string will replace "%1" in the returned message.
 * @param arg2 This string will replace "%2" in the returned message.
 * @param max_size Maximum size for the string that will be returned.
 *        defaults to 255 characters
 * @throws tbx::OsError if the message does not exist and no default is given
 */
std::string MessageFile::gsmessage(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, int max_size /*= 255*/) const
{
	char buffer[max_size+1];

	_kernel_swi_regs regs;
	regs.r[0] = reinterpret_cast<int>(_messageFD);
	regs.r[1] = reinterpret_cast<int>(token.c_str());
	regs.r[2] = reinterpret_cast<int>(buffer);
	regs.r[3] = max_size+1; // No buffer needed here
	regs.r[4] = reinterpret_cast<int>(arg0.c_str()); // pointer to parameter %0.
	regs.r[5] = reinterpret_cast<int>(arg1.c_str()); // pointer to parameter %1.
	regs.r[6] = reinterpret_cast<int>(arg2.c_str()); // pointer to parameter %2.
	regs.r[7] = 0; // pointer to parameter %3. No substitution required here.
	swix_check(_kernel_swi(MessageTrans_GSLookup, &regs, &regs));

	return std::string(buffer);
}

/**
 * Get the string with the given token name and four arguments substituted
 * and converts it from the GSTrans format.
 *
 * @param token token name to look up in message file. A default message
 *        can be added to this parameter by appending to the token name
 *        a colon (:) and then the default message.
 * @param arg0 This string will replace "%0" in the returned message.
 * @param arg1 This string will replace "%1" in the returned message.
 * @param arg2 This string will replace "%2" in the returned message.
 * @param arg3 This string will replace "%2" in the returned message.
 * @param max_size Maximum size for the string that will be returned.
 *        defaults to 255 characters
 * @throws tbx::OsError if the message does not exist and no default is given
 */
std::string MessageFile::gsmessage(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, const std::string &arg3, int max_size /*= 255*/) const
{
	char buffer[max_size+1];

	_kernel_swi_regs regs;
	regs.r[0] = reinterpret_cast<int>(_messageFD);
	regs.r[1] = reinterpret_cast<int>(token.c_str());
	regs.r[2] = reinterpret_cast<int>(buffer);
	regs.r[3] = max_size+1; // No buffer needed here
	regs.r[4] = reinterpret_cast<int>(arg0.c_str()); // pointer to parameter %0.
	regs.r[5] = reinterpret_cast<int>(arg1.c_str()); // pointer to parameter %1.
	regs.r[6] = reinterpret_cast<int>(arg2.c_str()); // pointer to parameter %2.
	regs.r[7] = reinterpret_cast<int>(arg3.c_str()); // pointer to parameter %3.
	swix_check(_kernel_swi(MessageTrans_GSLookup, &regs, &regs));

	return std::string(buffer);
}


}
