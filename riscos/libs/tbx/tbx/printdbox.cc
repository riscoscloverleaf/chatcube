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
 * printdbox.cc
 *
 *  Created on: 01-Jul-2010
 *      Author: alanb
 */

#include "printdbox.h"
#include "abouttobeshownlistener.h"
#include "hasbeenhiddenlistener.h"
#include "swixcheck.h"
#include "tbxexcept.h"
#include "res/resprintdbox.h"

#include <swis.h>

namespace tbx {

PrintDbox::PrintDbox(const res::ResPrintDbox &object_template) : ShowFullObject(object_template) {}

void PrintDbox::page_range(int start, int end)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,4), 0, _handle, 1, start, end));
}

void PrintDbox::all_pages()
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,4), 0, _handle, 1, -1, -1));
}

void PrintDbox::page_range(int *start, int *end) const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,2) | _OUTR(0,1), 0, _handle, 4, start, end));
}

/**
 * This event is raised just before the PrintDbox underlying window is
 * about to be shown.
 */
void PrintDbox::add_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	add_listener(0x82b00, listener, about_to_be_shown_router);
}

/**
 * Remove about to be shown listener
 */
void PrintDbox::remove_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	remove_listener(0x82b00, listener);
}

/**
 * This event is raised after the PrintDbox dialog has been completed
 */
void PrintDbox::add_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	add_listener(0x82b01, listener, has_been_hidden_router);
}

/**
 * Remove has been hidden listener
 */
void PrintDbox::remove_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	remove_listener(0x82b01, listener);
}

static void printdbox_setup_about_to_be_shown_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	PrintDboxSetupAboutToBeShownEvent ev(id_block, data);
    static_cast<PrintDboxSetupAboutToBeShownListener*>(listener)->printdbox_setup_about_to_be_shown(ev);
}

/**
 * Add listener that is called before the dialog associated with the setup
 * button is shown.
 */
void PrintDbox::add_setup_about_to_be_shown_listener(PrintDboxSetupAboutToBeShownListener *listener)
{
	add_listener(0x82b02, listener, printdbox_setup_about_to_be_shown_router);
}

/**
 * Remove listener that is called before the dialog associated with the setup
 * button is shown.
 */
void PrintDbox::remove_setup_about_to_be_shown_listener(PrintDboxSetupAboutToBeShownListener *listener)
{
	remove_listener(0x82b02, listener);
}

static void printdbox_setup_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	EventInfo ev(id_block, data);
    static_cast<PrintDboxSetupListener*>(listener)->printdbox_setup(ev);
}

/**
 * Add listener that is called if setup button is pressed and it has no
 * associated window.
 */
void PrintDbox::add_setup_listener(PrintDboxSetupListener *listener)
{
	add_listener(0x82b04, listener, printdbox_setup_router);
}

/**
 * Remove listener that is called if setup button is pressed and it has no
 * associated window.
 */
void PrintDbox::remove_setup_listener(PrintDboxSetupListener *listener)
{
	remove_listener(0x82b04, listener);
}

static void printdbox_print_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	PrintDboxPrintEvent ev(id_block, data);
    static_cast<PrintDboxPrintListener*>(listener)->printdbox_print(ev);
}

/**
 * Add listener for when print is selected on the print dialogue
 */
void PrintDbox::add_print_listener(PrintDboxPrintListener *listener)
{
	add_listener(0x82b05, listener, printdbox_print_router);
}

/**
 * Remove listener for when print is selected on the print dialogue
 */
void PrintDbox::remove_print_listener(PrintDboxPrintListener *listener)
{
	remove_listener(0x82b05, listener);
}

static void printdbox_save_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	PrintDboxSaveEvent ev(id_block, data);
    static_cast<PrintDboxSaveListener*>(listener)->printdbox_save(ev);
}


/**
 * Add listener for save button on print dialogue
 */
void PrintDbox::add_save_listener(PrintDboxSaveListener *listener)
{
	add_listener(0x82b03, listener, printdbox_save_router);
}

/**
 * Remove listener for save button on print dialogue
 */
void PrintDbox::remove_save_listener(PrintDboxSaveListener *listener)
{
	remove_listener(0x82b03, listener);
}

}
