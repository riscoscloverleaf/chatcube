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
 * messagefile.h
 *
 *  Created on: 14 Oct 2010
 *      Author: alanb
 */

#ifndef TBX_MESSAGEFILE_H_
#define TBX_MESSAGEFILE_H_

#include <string>

namespace tbx {

/**
 * Class to lookup token translations from a messages file
 */
class MessageFile
{
private:
	int _messageFD[4];
	char *_buffer;
	enum Status {CLOSED, OPENED, ATTACHED} _status;
public:
	MessageFile();
	MessageFile(const std::string &file_name);
	MessageFile(int *messageFD);
	~MessageFile();

	bool open(const std::string &file_name);
	void close();
	void attach(int *messageFD);

	/**
	 * Returns true if message file is open
	 */
	bool is_open() const {return (_status != CLOSED);}

	bool contains(const std::string &token) const;

	std::string message(const std::string &token) const;
	std::string message(const std::string &token, const std::string &arg0, int max_size = 255) const;
	std::string message(const std::string &token, const std::string &arg0, const std::string &arg1, int max_size = 255) const;
	std::string message(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, int max_size = 255) const;
	std::string message(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, const std::string &arg3, int max_size = 255) const;

	std::string gsmessage(const std::string &token, int max_size = 255) const;
	std::string gsmessage(const std::string &token, const std::string &arg0, int max_size = 255) const;
	std::string gsmessage(const std::string &token, const std::string &arg0, const std::string &arg1, int max_size = 255) const;
	std::string gsmessage(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, int max_size = 255) const;
	std::string gsmessage(const std::string &token, const std::string &arg0, const std::string &arg1, const std::string &arg2, const std::string &arg3, int max_size = 255) const;
};

}

#endif /* TBX_MESSAGEFILE_H_ */
