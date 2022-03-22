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
 * dociconbar.cc
 *
 *  Created on: 8 Oct 2010
 *      Author: alanb
 */

#include "dociconbar.h"
#include "docmanager.h"
#include "../application.h"
#include "../iconbar.h"

namespace tbx
{
namespace doc
{


/**
* Constructor. Automatically creates a listener to detect when
* the iconbar resource "Iconbar" is created so it can then
* attach the listeners it needs.
*/
DocIconbar::DocIconbar()
{
	tbx::app()->set_autocreate_listener("Iconbar", this);
}

/**
* Iconbar has been created so attach listeners for loading and
* creating new documents.
*
* It also removes the auto create listener as it is no longer
* needed.
*/
void DocIconbar::auto_created(std::string template_name, tbx::Object object)
{
	tbx::Iconbar iconbar(object);
	tbx::app()->clear_autocreate_listener(template_name);

	DocManager *manager = DocManager::instance();
	if (!manager) return; // Shouldn't happen

	// Add command to create a new document
	iconbar.add_select_command(manager->new_document_command());

	// Add command to quit application
	iconbar.add_command(0x4FFF, manager->quit_command());

	// Add loader to load a new document
	iconbar.add_loader(manager->file_loader(), manager->file_type());

	// Add opener to open a new document
	tbx::app()->add_opener(manager->file_loader(), manager->file_type());
}

}
}

