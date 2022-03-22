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
 * printdbox.h
 *
 *  Created on: 01-Jul-2010
 *      Author: alanb
 */

#ifndef TBX_PRINTDBOX_H_
#define TBX_PRINTDBOX_H_

#include "showfullobject.h"
#include "listener.h"
#include "eventinfo.h"
#include "window.h"

namespace tbx {

// Listeners
class AboutToBeShownListener;
class HasBeenHiddenListener;
class PrintDboxSetupAboutToBeShownListener;
class PrintDboxSetupListener;
class PrintDboxPrintListener;
class PrintDboxSaveListener;

namespace res
{
    class ResPrintDbox;
}

/**
 * Class to show a toolbox PrintDbox object.
 * The PrintDbox object shows a dialog that allows the user to
 * set a number of print options.
 */
class PrintDbox : public tbx::ShowFullObject
{
public:
	enum { TOOLBOX_CLASS = 0x82b00 };

	/**
	 * Construct creates an unassigned PrintDbox.
	 *
	 * It must be assigned to a value before the other methods can be used.
	 */
	PrintDbox() {};
	/**
	 * Construct a PrintDbox from another PrintDbox.
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other PrintDbox
	 *
	 * @param other PrintDbox to copy.
	 */
	PrintDbox(const PrintDbox &other) : ShowFullObject(other._handle) {}
	/**
	 * Construct a PrintDbox from an Object that refers to a PrintDbox
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a PrintDbox
	 */
	PrintDbox(const Object &other) : ShowFullObject(other)	{check_toolbox_class(PrintDbox::TOOLBOX_CLASS);}
	/**
	 * Create a PrintDbox from the named template
	 *
	 * @param template_name The name of the template to create the object from
	 * @throws OsError if an object cannot be created from the named template
	 * @throws ObjectClassException if the template is not for a PrintDbox
	 */
	PrintDbox(const std::string &template_name) : ShowFullObject(template_name)	{check_toolbox_class(PrintDbox::TOOLBOX_CLASS);}
	/**
	 * Create a PrintDbox from an in memory template resource
	 *
	 * @param object_template The template to create the object from
	 * @throws OsError if an object cannot be created from the template
	 */
	PrintDbox(const res::ResPrintDbox &object_template);

	/**
	 * Assign this PrintDbox from another.
	 *
	 * Both objects will then refer to the same underlying toolbox object
	 *
	 * @param other PrintDbox to copy
	 * @returns reference to this PrintDbox
	 */
	PrintDbox &operator=(const PrintDbox &other) {_handle = other.handle(); return *this;}
	/**
	 * Assign this PrintDbox from an Object that refers to a PrintDbox
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @returns reference to this PrintDbox
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a PrintDbox
	 */
	PrintDbox &operator=(const Object &other) {_handle = other.handle(); check_toolbox_class(TOOLBOX_CLASS); return *this;}
	/**
	 * Check if this PrintDbox refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they refer to the same underlying toolbox object
	 */
	bool operator==(const Object &other) const {return (_handle == other.handle());}
	/**
	 * Check if this PrintDbox does not refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they do not refer to the same underlying toolbox object
	 */
	bool operator!=(const Object &other) const {return (_handle != other.handle());}


	// Properties
	/**
	 * Return the underlying toolbox window used to implement the PrintDbox object
	 */
	Window window() const {return Window((ObjectId)int_property(0));}

	/**
	 * Set range of pages
	 *
	 * @param start first page
	 * @param end last page
	 */
	void page_range(int start, int end);

	/**
	 * Set to all pages
	 */
	void all_pages();

	/**
	 * Get page range
	 *
	 * If all pages are selected *start will be -1.
	 *
	 * @param start pointer to value to hold start of range
	 * @param end pointer to value to hold end of range.
	 */
	void page_range(int *start, int *end) const;

	/**
	 * Set number of copies
	 */
	void copies(int number) {int_property(3, number);}

	/**
	 * Get number of copies
	 */
	int copies() const {return int_property(4);}

	/**
	 * Set percentage to scale by
	 */
	void scale(int pc) {int_property(5, pc);}

	/**
	 * Get percentage to scale by
	 */
	int scale() const {return int_property(6);}

	/**
	 * Set orientation sideways
	 * @param value true for sideways, false for Upright
	 */
	void sideways(bool value) {bool_property(7, value);}

	/**
	 * Get orientation sideways
	 */
	bool sideways() const {return bool_property(8);}

	/**
	 * Return title
	 */
	std::string title() const {return string_property(9);}

	/**
	 * Return title length
	 */
	int title_length() const {return string_property_length(9);}

	/**
	 * Set draft mode
	 * @param value true to use draft printing
	 */
	void draft(bool value) {bool_property(10, value);}

	/**
	 * Get draft mode
	 */
	bool draft() const {return bool_property(11);}

	// Listeners
	void add_about_to_be_shown_listener(AboutToBeShownListener *listener);
	void remove_about_to_be_shown_listener(AboutToBeShownListener *listener);
	void add_has_been_hidden_listener(HasBeenHiddenListener *listener);
	void remove_has_been_hidden_listener(HasBeenHiddenListener *listener);
	void add_setup_about_to_be_shown_listener(PrintDboxSetupAboutToBeShownListener *listener);
	void remove_setup_about_to_be_shown_listener(PrintDboxSetupAboutToBeShownListener *listener);
	void add_setup_listener(PrintDboxSetupListener *listener);
	void remove_setup_listener(PrintDboxSetupListener *listener);
	void add_print_listener(PrintDboxPrintListener *listener);
	void remove_print_listener(PrintDboxPrintListener *listener);
	void add_save_listener(PrintDboxSaveListener *listener);
	void remove_save_listener(PrintDboxSaveListener *listener);
};

/**
 * The dialog box associated with the setup button is about to be shown event
 */
class PrintDboxSetupAboutToBeShownEvent : public EventInfo
{
public:
	/**
	 * Construct setup about to be shown event from Toolbox and WIMP information
	 *
	 * @param id_block Toolbox information
	 * @param data WIMP event information
	 */
	PrintDboxSetupAboutToBeShownEvent(IdBlock &id_block, PollBlock &data) :
		EventInfo(id_block, data)
	{
	}

	/**
	 * Setup window that is about to be shown
	 */
	Window setup_window() const {return tbx::Window((ObjectId)_data.word[4]);}

	/**
	 * Enumeration describing how the setup box is about to be shown
	 */
	enum ShowType
	{
		SHOW_DEFAULT = 0, //!< Shown in default place
		SHOW_FULL_SPEC = 1, //!< Full details of window used
		SHOW_TOP_LEFT = 2 //<! Shown with top left corner as specified point
	};

	/**
	 * Get how the setup windows was shown
	 *
	 * @returns show type enumeration value
	 */
	ShowType show_type() const {return ShowType(_data.word[5]);}

	/**
	 * Get top left coordinate of where the window will be shown
	 *
	 * @returns top left window coordinate
	 */
	Point &top_left() const {return reinterpret_cast<Point &>(_data.word[6]);}

	/**
	 * Get reference to top left coordinate so it can be changed
	 * in the event
	 *
	 * @returns Point reference to coordinate to allow it to be changed
	 */
	Point &top_left() {return reinterpret_cast<Point &>(_data.word[6]);}

	/**
	 * Get reference to the full specification for the show so
	 * it can be modified.
	 *
	 * @returns reference to the full show specification
	 */
	ShowFullSpec &full_spec() {return reinterpret_cast<ShowFullSpec &>(_data.word[6]);}
};

/**
 * Setup dialog is about to be shown listener
 */
class PrintDboxSetupAboutToBeShownListener : public Listener
{
public:
	virtual ~PrintDboxSetupAboutToBeShownListener() {};

	/**
	 * Method called by the listener when the setup dialog is about to be shown
	 *
	 * @param ev information on the event
	 */
	virtual void printdbox_setup_about_to_be_shown(const PrintDboxSetupAboutToBeShownEvent &ev) = 0;
};

/**
 * Print button has been pressed event
 */
class PrintDboxPrintEvent : public EventInfo
{
public:
	/**
	 * Construct the PrintDboxEvent from the information returned by the Toolbox/WIMP
	 *
	 * @param id_block information on Toolbox objects affected
	 * @param data WIMP information on the event
	 */
	PrintDboxPrintEvent(IdBlock &id_block, PollBlock &data) :
		EventInfo(id_block, data) {}

	/**
	 * true if orientation was set to sideway, false for upright
	 */
	bool sideways() const {return ((_data.word[3] & 1) != 0);}

	/**
	 * Start page or -1 for all
	 */
	int start() const {return _data.word[4];}

	/**
	 * End page
	 */
	int end() const {return _data.word[5];}

	/**
	 * Number of copies
	 */
	int copies() const {return _data.word[6];}

	/**
	 * Scale percentage
	 */
	int scale() const {return _data.word[7];}
};

/**
 * Event information for the save listener.
 */
typedef PrintDboxPrintEvent PrintDboxSaveEvent;

/**
 * Listener for Print selected on PrintDbox
 */
class PrintDboxPrintListener : public Listener
{
public:
	virtual ~PrintDboxPrintListener() {}

	/**
	 * Method called when the print button is selected
	 *
	 * @param event information on what to print
	 */
	virtual void printdbox_print(const PrintDboxPrintEvent &event) = 0;
};

/**
 * Listener for Save selected on PrintDbox
 */
class PrintDboxSaveListener : public Listener
{
public:
	virtual ~PrintDboxSaveListener() {}

	/**
	 * Method called when the save button is selected
	 *
	 * @param event the settings to save
	 */
	virtual void printdbox_save(const PrintDboxPrintEvent &event) = 0;
};

/**
 * Listener for Setup presses when there is not dialogue associated with the button
 */
class PrintDboxSetupListener : public Listener
{
public:
	virtual ~PrintDboxSetupListener() {}

	/**
	 * Method called when setup is clicked if there is no dialogue
	 *
	 * @param setup_event information on the object involved with this event
	 */
	virtual void printdbox_setup(const EventInfo &setup_event) = 0;
}
;
}

#endif /* TBX_PRINTDBOX_H_ */
