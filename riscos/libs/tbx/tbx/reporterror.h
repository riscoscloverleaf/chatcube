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
 * reporterror.h
 *
 *  Created on: 10-Sep-2008
 *      Author: alanb
 */

#ifndef REPORTERROR_H_
#define REPORTERROR_H_

#include "kernel.h"
#include <string>

namespace tbx
{
    /**
     * Values returned from report error
     */
	enum ReportErrorReply {NONE, OK, CANCEL};
	/**
	 * Flags for report error
	 */
	enum ReportErrorFlags
	{
		REF_OK_BUTTON = 1, /**< Provide an OK button */
		REF_CANCEL_BUTTON = 2, /**< Provide a Cancel button */
		REF_HIGHLIGHT_CANCEL = 4, /**< Highlight the cancel button */
		REF_NO_SPACE_TO_CONTINUE = 8, /**< In a text-style window do not prompt to continue */
		REF_NO_TITLE_PREFIX = 16, /**< Don't prefix application name with error from title bar */
		REF_LEAVE_OPEN = 32, /**< Return immediately leaving window open - Note you must close the box before the next WIMP poll */
		REF_CLOSE = 64 /**< Select one box depending on bit 0 and 1 and close window */
	};

	ReportErrorReply report_error(_kernel_oserror *err, const char *title, int flags = 0);
	ReportErrorReply report_error(_kernel_oserror *err, int flags = 0);
	ReportErrorReply report_error(const char* msg, int flags = 0);
	ReportErrorReply report_error(const char* msg, const char *title, int flags = 0);
	ReportErrorReply report_error(const std::string &msg, int flags = 0);
	ReportErrorReply report_error(const std::string &msg, const std::string &title, int flags = 0);

	void report_error_close(ReportErrorReply reply);
}

#endif /* REPORTERROR_H_ */
