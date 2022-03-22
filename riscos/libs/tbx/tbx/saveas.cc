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

#include "saveas.h"
#include "abouttobeshownlistener.h"
#include "hasbeenhiddenlistener.h"
#include "res/ressaveas.h"

#include "swis.h"
#include "swixcheck.h"

namespace tbx {

SaveAs::SaveAs(const res::ResSaveAs &object_template) : ShowFullObject(object_template) {}

/**
 * Set the data addresses for Type 1 saves.
 *
 * @param data Address of data to be saved
 * @param size Size in bytes of the data to be saved
 * @param selection Address of selection (if any)
 * @param selection_size Size of selection in bytes (if any)
 */

void SaveAs::set_data_address(void *data, int size, void *selection /* = 0*/, int selection_size /*= 0*/)
{
	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,6), 0, _handle, 10,
			data, size, selection, selection_size));

}

/**
 * Call this as part of a type 3 RAM transfer in a SaveAsFillBufferListener
 * to give the buffer to transfer and its size.
 *
 * @param buffer pointer of data to transfer
 * @param size size of data in buffer in bytes
 */
void SaveAs::buffer_filled(void *buffer, int size)
{
	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,4), 0, _handle, 11,
			buffer, size));

}

/**
 * Call this after saving a file in the SaveAsSaveToFileListener for
 * type 2 and 3 transfers
 *
 * @param successful set to true if save was successful
 * @param file_name actual file name client saved to.
 */
void SaveAs::file_save_completed(bool successful, std::string file_name)
{
	swix_check(_swix(0x44ec6, _INR(0,3), successful, _handle, 12,
			reinterpret_cast<int>(file_name.c_str())
			));
}

/**
 * This event is raised just before the saveas underlying window is
 * about to be shown.
 *
 * Commonly it is used to set the file name, file type, file size and
 * if there is a selection available.
 * For save type 1 it can also be used to set the data buffer address
 *
 * @param listener listener to add
 */
void SaveAs::add_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	add_listener(0x82bc0, listener, about_to_be_shown_router);
}

/**
 * Remove about to be shown listener
 *
 * @param listener listener to remove
 */
void SaveAs::remove_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	remove_listener(0x82bc0, listener);
}

/**
 * Add listener for when save as dialogue has been hidden.
 *
 * This uses the same toolbox event as the dialogue completed listener, but
 * does not return the additional information.
 *
 * @param listener listener to add
 */
void SaveAs::add_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	add_listener(0x82bc1, listener, has_been_hidden_router);
}

/**
 * Remove listener for when save as dialogue has been hidden.
 *
 * This uses the same toolbox event as the dialogue completed listener, but
 * does not return the additional information.
 *
 * @param listener listener to remove
 */
void SaveAs::remove_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	remove_listener(0x82bc1, listener);
}


static void saveas_dialog_completed_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	SaveAsDialogueCompletedEvent ev(id_block, data);
    static_cast<SaveAsDialogueCompletedListener*>(listener)->saveas_dialogue_completed(ev);
}

/**
 * Add listener to be called when the dialogue has been completed
 *
 * @param listener listener to add
 */
void SaveAs::add_dialogue_completed_listener(SaveAsDialogueCompletedListener *listener)
{
	add_listener(0x82bc1, listener, saveas_dialog_completed_router);
}

/**
 * Remove listener for when dialog has been completed
 *
 * @param listener listener to remove
 */
void SaveAs::remove_dialogue_completed_listener(SaveAsDialogueCompletedListener *listener)
{
	remove_listener(0x82bc1, listener);
}

static void saveas_save_completed_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	SaveAsSaveCompletedEvent ev(id_block, data);
    static_cast<SaveAsSaveCompletedListener*>(listener)->saveas_save_completed(ev);
}

/**
 * Add listener for when the save operation has completed
 *
 * @param listener listener to add
 */
void SaveAs::add_save_completed_listener(SaveAsSaveCompletedListener *listener)
{
	add_listener(0x82bc4, listener, saveas_save_completed_router);
}

/**
 * Remove save as completed listener
 *
 * @param listener listener to remove
 */
void SaveAs::remove_save_completed_listener(SaveAsSaveCompletedListener *listener)
{
	remove_listener(0x82bc4, listener);
}


static void saveas_save_to_file_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	SaveAs saveas(id_block.self_object());
	std::string file_name(reinterpret_cast<const char *>(&data.word[4]));
	static_cast<SaveAsSaveToFileHandler *>(listener)->saveas_save_to_file(
			saveas, (data.word[3]&1)!=0, file_name);
}

/**
 * Set handler to do save to a file
 *
 * @param handler object called to do the save
 */
void SaveAs::set_save_to_file_handler(SaveAsSaveToFileHandler *handler)
{
	set_handler(0x82bc2, handler, saveas_save_to_file_router);
}

static void saveas_fill_buffer_router(IdBlock &id_block, PollBlock &data, Listener *listener)
{
	SaveAs saveas(id_block.self_object());
	static_cast<SaveAsFillBufferHandler *>(listener)->saveas_fill_buffer(
			saveas, (data.word[3]&1)!=0, data.word[4],
			(void *)data.word[5], data.word[6]);
}

/**
 * Set handler to fill a buffer
 *
 * @param handler object called to fill the buffer
 */
void SaveAs::set_fill_buffer_handler(SaveAsFillBufferHandler *handler)
{
	set_handler(0x82bc3, handler, saveas_fill_buffer_router);
}

}
