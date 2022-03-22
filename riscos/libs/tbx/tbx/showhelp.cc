/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2013 Alan Buckley   All Rights Reserved.
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
 * ShowHelp.cc
 *
 *  Created on: 11-Jun-2009
 *      Author: alanb
 */

#include "showhelp.h"
#include "application.h"
#include <kernel.h>

namespace tbx
{
ShowHelp::ShowHelp()
{
}

ShowHelp::~ShowHelp()
{
}

/**
 * Display the application !Help file
 */
void ShowHelp::execute()
{
	std::string cmd("Filer_Run " + app()->directory() + "!Help");
	_kernel_oscli(cmd.c_str());
}
}
