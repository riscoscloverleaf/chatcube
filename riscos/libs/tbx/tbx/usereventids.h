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
 * usereventids.h
 *
 *  Created on: 10-Sep-2008
 *      Author: alanb
 */

#ifndef USEREVENTIDS_H_
#define USEREVENTIDS_H_

namespace tbx
{
	/**
     * First event ID available to be used in any application
     */
	const int FIRST_USER_EID = 1;
	/**
     * Last event ID available to be used in any application
     */
	const int LAST_USER_EID = 0x4FFF;

	/**
     * First event ID available to be shared between multiple applications
     */
	const int FIRST_SHARE_EID = 0x5000;
	/**
     * Last event ID available to be shared between multiple applications
     */
	const int LAST_SHARE_EID = 0x5FFF;

	/**
     * First event ID for dynamically allocated IDs
     */
	const int FIRST_DYNAMIC_EID = 0x6000;
	/**
     * Last event ID for dynamically allocated IDs
     */
	const int LAST_DYNAMIC_EID = 0x6FFF;

	/**
	 * First event ID reserved for the TBX library
	 */
	const int FIRST_TBX_EID = 0x7000;
	/**
	 * Last event ID reserved for the TBX library
	 */
	const int LAST_TBX_EID = 0x7FFF;
}

#endif /* USEREVENTIDS_H_ */
