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
 * dcs.cc
 *
 *  Created on: 05-Jun-2009
 *      Author: alanb
 */

#include "dcs.h"
#include "abouttobeshownlistener.h"
#include "hasbeenhiddenlistener.h"
#include "res/resdcs.h"

namespace tbx {

DCS::DCS(const res::ResDCS &object_template) : ShowFullObject(object_template) {}

/**
 * This event is raised just before the DCS underlying window is
 * about to be shown.
 *
 * @param listener about to be shown listener to add
 */
void DCS::add_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	add_listener(0x82a80, listener, about_to_be_shown_router);
}

/**
 * Remove about to be shown listener
 *
 * @param listener about to be shown listener to remove
 */
void DCS::remove_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	remove_listener(0x82a80, listener);
}

/**
 * This event is raised after the DCS dialog has been completed
 *
 * @param listener has been hidden listener to add
 */
void DCS::add_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	add_listener(0x82a83, listener, has_been_hidden_router);
}

/**
 * Remove has been hidden listener
 *
 * @param listener has been hidden listener to remove
 */
void DCS::remove_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	remove_listener(0x82a83, listener);
}

//! @cond INTERNAL
static void dcs_discard_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	EventInfo ev(id_block, data);
    static_cast<DCSDiscardListener*>(listener)->dcs_discard(ev);
}
//! @endcond

/**
 * Add listener to be called when discard is selected from the dialogue
 *
 * @param listener discard selected listener to add
 */
void DCS::add_discard_listener(DCSDiscardListener *listener)
{
	add_listener(0x82a81, listener, dcs_discard_router);
}

/**
 * Remove listener to be called when discard is selected from the dialogue
 *
 * @param listener discard selected listener to remove
 */
void DCS::remove_discard_listener(DCSDiscardListener *listener)
{
	remove_listener(0x82a81, listener);
}

//! @cond INTERNAL
static void dcs_save_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	EventInfo ev(id_block, data);
    static_cast<DCSSaveListener*>(listener)->dcs_save(ev);
}
//! @endcond

/**
 * Add listener to be called when save is selected from the dialogue
 *
 * @param listener save selected listener to add
 */
void DCS::add_save_listener(DCSSaveListener *listener)
{
	add_listener(0x82a82, listener, dcs_save_router);
}

/**
 * Remove listener to be called when save is selected from the dialogue
 *
 * @param listener save selected listener to remove
 */
void DCS::remove_save_listener(DCSSaveListener *listener)
{
	remove_listener(0x82a82, listener);
}

//! @cond INTERNAL
static void dcs_cancel_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	EventInfo ev(id_block, data);
   static_cast<DCSCancelListener*>(listener)->dcs_cancel(ev);
}
//! @endcond

/**
 * Add listener to be called when cancel is selected from the dialogue
 *
 * @param listener cancel selected listener to add
 */
void DCS::add_cancel_listener(DCSCancelListener *listener)
{
	add_listener(0x82a84, listener, dcs_cancel_router);
}

/**
 * Remove listener to be called when cancel is selected from the dialogue
 *
 * @param listener cancel selected listener to remove
 */
void DCS::remove_cancel_listener(DCSCancelListener *listener)
{
	remove_listener(0x82a84, listener);
}


}
