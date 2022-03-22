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


#include "component.h"
#include "tbxexcept.h"
#include "swixcheck.h"
#include "swis.h"
#include "eventrouter.h"
#include "commandrouter.h"
#include "command.h"
#include "usereventlistener.h"

namespace tbx {

/**
 * Add a command to this Component.
 *
 * For more details on events and commands see
 * @subpage events
 *
 * @param command_id The event id to associate the command with.
 * @param command The command to run when this event is received.
 * @throws ComponentNullError toolbox handle is NULL_ComponentId
 */
void Component::add_command(int command_id, Command *command)
{
	if (_id == NULL_ComponentId) throw ComponentNullError();

	event_router()->add_object_listener(_handle, _id, command_id, command, &command_router);
}

/**
 * Remove a command from this component.
 *
 * @param command_id The event id that was associated with the command.
 * @param command The command to remove.
 * @throws ComponentNullError toolbox handle is NULL_ComponentId
 */
void Component::remove_command(int command_id, Command *command)
{
	if (_id == NULL_ComponentId) throw ComponentNullError();

	event_router()->remove_object_listener(_handle, _id, command_id, command);
}

/**
 * Add a user event listener.
 *
 * For more details on events and commands see
 * @subpage events
 *
 * @param event_id The event ID to associate the listener with.
 * @param listener The listener to use.
 * @throws ComponentNullError toolbox handle is NULL_ComponentId
 */
void Component::add_user_event_listener(int event_id, UserEventListener *listener)
{
	if (_id == NULL_ComponentId) throw ComponentNullError();

	event_router()->add_object_listener(_handle, _id, event_id, listener, &user_event_router);
}

/**
 * Remove a user event listener  from this component.
 *
 * @param event_id The event id that was associated with the listener.
 * @param listener The listener to remove.
 * @throws ComponentNullError toolbox handle is NULL_ComponentId
 */
void Component::remove_user_event_listener(int event_id, UserEventListener *listener)
{
	if (_id == NULL_ComponentId) throw ComponentNullError();

	event_router()->remove_object_listener(_handle, _id, event_id, listener);
}

/**
 * Remove all the listeners and commands from this component.
 *
 * @throws ComponentNullError if this is an uninitialised component
 */
void Component::remove_all_listeners()
{
	if (_id == NULL_ComponentId) throw ComponentNullError();

	event_router()->remove_all_listeners(_handle, _id);
}

/**
 * Helper function to add listeners to this component.
 *
 * Derived classes usually call this specifying the action and handler
 * for the events specific to them.
 */
void Component::add_listener( int action, Listener *listener, RawToolboxEventHandler handler)
{
	if (_id == NULL_ComponentId) throw ComponentNullError();

	event_router()->add_object_listener(_handle, _id, action, listener, handler);
}

/**
 * Helper function to remove listeners from this component.
 */
void Component::remove_listener(int action, Listener *listener)
{
	if (_id == NULL_ComponentId) throw ComponentNullError();

	event_router()->remove_object_listener(_handle, _id, action, listener);
}

/**
 * Helper function to add a listener for a WIMP window event
 */
void Component::add_window_listener(int event_code, Listener *listener)
{
	if (_id == NULL_ComponentId) throw ComponentNullError();

	event_router()->add_window_event_listener(_handle, _id, event_code, listener);
}

/**
 * Helper function to remove a listener for a WIMP window event
 */
void Component::remove_window_listener(int event_code, Listener *listener)
{
	if (_id == NULL_ComponentId) throw ComponentNullError();

	event_router()->remove_window_event_listener(_handle, _id, event_code, listener);
}

/**
 *  Get an integer property from the toolbox Component.
 *
 *  Helper function to implement specific properties in subclasses.
 *  Calls Toolbox_ObjectMiscOp and returns value in r0.
 *
 * @param property_id the method code to get the property
 * @returns value of property
 * @throws  OsError
*/

int Component::int_property(int property_id) const
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = property_id;
    regs.r[3] = _id;

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return regs.r[0];
}

/**
 *  Set an integer property from the toolbox object.
 *
 *  Helper function to implement specific properties in subclasses.
 *  Calls Toolbox_ObjectMiscOp and with value in r4.
 *
 * @param property_id the method code to get the property
 * @param value the new value for the property
 * @throws  OsError
*/

void Component::int_property(int property_id, int value)
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = property_id;
    regs.r[3] = _id;
    regs.r[4] = value;

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 *  Get a boolean property from the toolbox Component.
 *
 *  Helper function to implement specific properties in subclasses.
 *  Calls Toolbox_ObjectMiscOp and returns value in r0.
 *
 * @param property_id the method code to get the property
 * @returns value of property
 * @throws  OsError
*/

bool Component::bool_property(int property_id) const
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = property_id;
    regs.r[3] = _id;

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return (regs.r[0] != 0);
}

/**
 *  Set a boolean property from the toolbox object.
 *
 *  Helper function to implement specific properties in subclasses.
 *  Calls Toolbox_ObjectMiscOp and with value in r4.
 *
 * @param property_id the method code to get the property
 * @param value The new value for the property
 * @throws  OsError
*/
void Component::bool_property(int property_id, bool value)
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = property_id;
    regs.r[3] = _id;
    regs.r[4] = value;

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 *  Get a string property from the toolbox object.
 *
 *  Helper function to implement specific properties in subclasses.
 *  Calls Toolbox_ObjectMiscOp and with buffer for return in r4, size in r5.
 *
 * @param property_id the method code to get the property
 * @returns The value from the property
 * @throws  OsError
*/
std::string Component::string_property(int property_id) const
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = property_id;
    regs.r[3] = _id;
    regs.r[4] = 0;
    regs.r[5] = 0;

    // Run Toolbox_ObjectMiscOp to get the size of the buffer
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    std::string value;
    int len = regs.r[5];
    if (len)
    {
       char buffer[len];
       regs.r[4] = reinterpret_cast<int>(buffer);
       regs.r[5] = len;

       // Run Toolbox_ObjectMiscOp to get the buffer
       swix_check(_kernel_swi(0x44ec6, &regs, &regs));

       value = buffer;
    }

    return value;
}

/**
 *  Get a the length of a string property from the toolbox object.
 *
 *  Helper function to implement specific properties in subclasses.
 *  Calls Toolbox_ObjectMiscOp and with buffer for return in r3, size in r4.
 *
 * @param property_id the method code to get the property
 * @returns The length of the property
 * @throws  OsError
*/

int Component::string_property_length(int property_id) const
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = property_id;
    regs.r[3] = _id;;
    regs.r[4] = 0;
	regs.r[5] = 0;

    // Run Toolbox_ObjectMiscOp to get the size of the buffer
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return regs.r[5] - 1; // Return value include space for terminating null
}


/**
 *  Set a string property in the toolbox object.
 *
 *  Helper function to implement specific properties in subclasses.
 *  Calls Toolbox_ObjectMiscOp and with value pointed to by r3.
 *
 * @param property_id the method code to get the property
 * @param value the new string value for the property
 * @throws  OsError
*/

void Component::string_property(int property_id, const std::string &value)
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = property_id;
    regs.r[3] = _id;
    regs.r[4] = reinterpret_cast<int>(const_cast<char *>(value.c_str()));

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Check if a particular flag is set in a property.
 *
 * @param property_id - id to check
 * @param flag	- flag value to test
 * @return true if flag is set
 * @throws OsError
 */
bool Component::flag_property(int property_id, int flag) const
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = property_id;
    regs.r[3] = _id;

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return (regs.r[0] & flag) !=0;
}

/**
 * Set a particular flag in a property.
 *
 * @param property_id - property id to get the flags,
 *    property_id+1 must be the id to set the flags
 * @param flag	- flag value to set
 * @param value - true set the flag, false clear it
 * @throws OsError
 */
void Component::flag_property(int property_id, int flag, bool value)
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = property_id;
    regs.r[3] = _id;

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    int state = regs.r[0];

    if (value != ((state & flag)!= 0))
    {
    	// Only update if necessary
    	regs.r[0] = 0;
        regs.r[1] = _handle;
        regs.r[2] = property_id + 1;
        regs.r[3] = _id;
        regs.r[4] = (value) ? (state | flag) : (state & ~flag);

        // Run Toolbox_ObjectMiscOp
        swix_check(_kernel_swi(0x44ec6, &regs, &regs));
    }
}


}
