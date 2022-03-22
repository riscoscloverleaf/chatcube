/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2021 Alan Buckley   All Rights Reserved.
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

#include "window.h"
#include "gadget.h"
#include "eventrouter.h"
#include "abouttobeshownlistener.h"
#include "hasbeenhiddenlistener.h"
#include "redrawlistener.h"
#include "openwindowlistener.h"
#include "closewindowlistener.h"
#include "pointerlistener.h"
#include "mouseclicklistener.h"
#include "keylistener.h"
#include "caretlistener.h"
#include "scrollrequestlistener.h"
#include "loadermanager.h"
#include "modeinfo.h"
#include "sprite.h"
#include "caret.h"
#include "res/reswindow.h"
#include "res/resgadget.h"
#include "res/resshortcut.h"

#include "swis.h"
#include "swixcheck.h"

using namespace tbx;

Window::Window(const res::ResWindow &object_template) : ShowFullObject(object_template)
{
}

/**
 * Show this window as a sub window (nested child) of
 * an existing window.
 *
 * @param spec ShowSubWindowSpec specifying the location, parent
 * and alignment flags for positioning this window in it's parent.
 */
void Window::show_as_subwindow(const ShowSubWindowSpec &spec, ObjectId parent, ComponentId cmp_id)
{
	swix_check(_swix(0x44EC3, _INR(0,5),
			4,  // Show as sub window flag
			_handle,
			1,  // Show using full spec
			&(spec.visible_area),
			parent,
            cmp_id));
}


/**
 * Add a gadget to the window.
 *
 * If the window is visible it will be shown immediately
 * If the gadgets component id is -1 it will be allocated an unused component id
 *
 * @param gadget_template - gadget template to add
 * @returns new gadget created
 * @throws OsError if add fails
 */
Gadget Window::add_gadget(const res::ResGadget &gadget_template)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[1] = _handle;
	regs.r[2] = 1;
	regs.r[3] = (int)gadget_template.header();

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return Gadget(_handle, (ComponentId)regs.r[0]);
}

/**
 * Remove a gadget from the window.
 *
 * If the window is visible this will cause a redraw of the Window
 *
 * Automatically removes any listeners that were added to the gadget
 */
void Window::remove_gadget(ComponentId component_id)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[1] = _handle;
	regs.r[2] = 2;
	regs.r[3] = component_id;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	Gadget g(_handle, component_id);
	g.remove_all_listeners();
}


/**
 * Set menu used with this window
 *
 * @param menu new menu to use.
 */
void Window::menu(Menu menu)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[1] = _handle;
	regs.r[2] = 3;
	regs.r[3] = menu.handle();

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Removes the menu from the window
 */
void Window::remove_menu()
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[1] = _handle;
	regs.r[2] = 3;
	regs.r[3] = 0;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get the current menu used by this object
 *
 * @returns Menu on Window. If there is no menu the return menus null() method
 * will be true.
 */
Menu Window::menu() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[1] = _handle;
	regs.r[2] = 4;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return tbx::Menu(regs.r[0]);
}

/**
 * Add a keyboard shortcut
 *
 * @param shortcut object with information about the shortcut
 */
void Window::add_shorcut(const res::ResShortcut &shortcut)
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[1] = _handle;
	regs.r[2] = 9;
	regs.r[3] = 1;
	regs.r[4] = (int)shortcut.header();

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Add multiple keyboard shortcuts from an array
 *
 * @param shortcuts array of objects with information about the shortcuts
 * @param num number of shortcuts in the shortcuts array
 */
void Window::add_shortcuts(const res::ResShortcut *shortcuts, int num)
{
	char temp[num * res::SHORTCUT_SIZE];
	char *p = temp;
	for (int sc = 0; sc < num; sc++)
	{
		memcpy(p, shortcuts->header(), res::SHORTCUT_SIZE);
		p+= res::SHORTCUT_SIZE;
		shortcuts++;
	}
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[1] = _handle;
	regs.r[2] = 9;
	regs.r[3] = num;
	regs.r[4] = (int)temp;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Remove all window shortcuts
 */
void Window::remove_all_shortcuts()
{
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[1] = _handle;
	regs.r[2] = 10;
	regs.r[3] = -1;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Remove given window shortcuts
 */
void Window::remove_shortcuts(const res::ResShortcut *shortcuts, int num)
{
	char temp[num * res::SHORTCUT_SIZE];
	char *p = temp;
	for (int sc = 0; sc < num; sc++)
	{
		memcpy(p, shortcuts->header(), res::SHORTCUT_SIZE);
		p+= res::SHORTCUT_SIZE;
		shortcuts++;
	}
	_kernel_swi_regs regs;
	regs.r[0] = 0;
	regs.r[1] = _handle;
	regs.r[2] = 10;
	regs.r[3] = num;
	regs.r[4] = (int)temp;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Set toolbars for this window.
 *
 * pointers set to 0 means don't change
 */
void Window::set_toolbars(Window *internal_bottom_left, Window *internal_top_left, Window *external_bottom_left, Window *external_top_left)
{
	_kernel_swi_regs regs;
	regs.r[0] = ((internal_bottom_left) ? 1 : 0)
			| ((internal_top_left) ? 2 : 0)
			| ((external_bottom_left) ? 4 : 0)
			| ((external_top_left) ? 8 : 0);
	regs.r[1] = _handle;
	regs.r[2] = 18;
	if (internal_bottom_left) regs.r[3] = internal_bottom_left->handle();
	if (internal_top_left) regs.r[4] = internal_top_left->handle();
	if (external_bottom_left) regs.r[5] = external_bottom_left->handle();
	if (external_top_left) regs.r[6] = external_top_left->handle();

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get toolbars
 */
void Window::get_toolbars(Window *internal_bottom_left, Window *internal_top_left, Window *external_bottom_left, Window *external_top_left)
{
	_kernel_swi_regs regs;
	regs.r[0] = ((internal_bottom_left) ? 1 : 0)
			| ((internal_top_left) ? 2 : 0)
			| ((external_bottom_left) ? 4 : 0)
			| ((external_top_left) ? 8 : 0);
	regs.r[1] = _handle;
	regs.r[2] = 19;
	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	if (internal_bottom_left) *internal_bottom_left = tbx::Window(regs.r[0]);
	if (internal_top_left) *internal_top_left = tbx::Window(regs.r[1]);
	if (external_bottom_left) *external_bottom_left = tbx::Window(regs.r[2]);
	if (external_top_left) *external_top_left = tbx::Window(regs.r[3]);
}

/**
 * Set internal bottom left toolbar
 */
void Window::ibl_toolbar(Window window)
{
	_kernel_swi_regs regs;
	regs.r[0] = 1;
	regs.r[1] = _handle;
	regs.r[2] = 18;
	regs.r[3] = window.handle();

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get internal bottom left toolbar
 */
Window Window::ibl_toolbar() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 1;
	regs.r[1] = _handle;
	regs.r[2] = 19;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return tbx::Window(regs.r[0]);
}

/**
 * Set internal top left toolbar
 */
void Window::itl_toolbar(Window window)
{
	_kernel_swi_regs regs;
	regs.r[0] = 2;
	regs.r[1] = _handle;
	regs.r[2] = 18;
	regs.r[4] = window.handle();

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get internal top left toolbar
 */
Window Window::itl_toolbar() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 2;
	regs.r[1] = _handle;
	regs.r[2] = 19;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return tbx::Window(regs.r[1]);
}
/**
 * Set external bottom left toolbar
 */
void Window::ebl_toolbar(Window window)
{
	_kernel_swi_regs regs;
	regs.r[0] = 4;
	regs.r[1] = _handle;
	regs.r[2] = 18;
	regs.r[5] = window.handle();

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get external bottom left toolbar
 */
Window Window::ebl_toolbar() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 4;
	regs.r[1] = _handle;
	regs.r[2] = 19;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return tbx::Window(regs.r[2]);
}
/**
 * Set external top left toolbar
 */
void Window::etl_toolbar(Window window)
{
	_kernel_swi_regs regs;
	regs.r[0] = 8;
	regs.r[1] = _handle;
	regs.r[2] = 18;
	regs.r[6] = window.handle();

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get external top left toolbar
 */
Window Window::etl_toolbar() const
{
	_kernel_swi_regs regs;
	regs.r[0] = 8;
	regs.r[1] = _handle;
	regs.r[2] = 19;

	swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	return tbx::Window(regs.r[3]);
}

/**
 * Set the work area extent for the window.
 */
void Window::extent(const BBox &new_extent)
{
	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,3), 0, _handle, 15, &(new_extent.min.x)));
}

/**
 * Get the current work area extent for the window
 */
BBox Window::extent() const
{
	BBox ex;
	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,3), 0, _handle, 16, &(ex.min.x)));

	return ex;
}

/**
 * Force the area to be redrawn.
 *
 * @param work_area area to redraw in work area coordinates
 */
void Window::force_redraw(const BBox &work_area)
{
	// Run Toolbox_ObjectMiscOp
	swix_check(_swix(0x44ec6, _INR(0,3), 0, _handle, 17, &(work_area.min.x)));
}

/**
 * Update the window immediately using any registered redrawers.
 *
 * The redrawing routines should not remove themselves from the
 * window during this call.
 *
 * @param bounds area to update in work area co-ordinates
 * @param background redrawer to paint the background or 0 for none.
 */
void Window::update(const BBox &bounds, RedrawListener *background /*= 0*/)
{
	int more;
	IdBlock id_block(*this);
	PollBlock poll_block;
	EventRouter::WindowEventListenerItem *first_item = event_router()->find_window_event_listener(_handle, 1);

	if (first_item || background)
	{
		poll_block.word[0] = window_handle();
		poll_block.word[1] = bounds.min.x;
		poll_block.word[2] = bounds.min.y;
		poll_block.word[3] = bounds.max.x;
		poll_block.word[4] = bounds.max.y;

		swix_check(_swix(Wimp_UpdateWindow, _IN(1)|_OUT(0), &poll_block, &more));
		while (more)
		{
			RedrawEvent e(id_block, poll_block);
			if (background) background->redraw(e);
			EventRouter::WindowEventListenerItem *redraw = first_item;
			while(redraw)
			{
				static_cast<RedrawListener *>(redraw->listener)->redraw(e);
				redraw = redraw->next;
			}
			if (_swix(Wimp_GetRectangle, _IN(1)|_OUT(0), &poll_block, &more) != 0) more = 0;
		}
	}
}

/**
 * Update the window immediately using the given redrawers.
 *
 * @param bounds area to update in work area co-ordinates
 * @param redrawers array of redrawers to use to update the window
 * @param redrawer_count number of redrawers in the array.
 */
void Window::update(const BBox &bounds, RedrawListener **redrawers, int redrawer_count)
{
	int more;
	IdBlock id_block(*this);
	PollBlock poll_block;

	if (redrawers && redrawer_count)
	{
		poll_block.word[0] = window_handle();
		poll_block.word[1] = bounds.min.x;
		poll_block.word[2] = bounds.min.y;
		poll_block.word[3] = bounds.max.x;
		poll_block.word[4] = bounds.max.y;

		swix_check(_swix(Wimp_UpdateWindow, _IN(1)|_OUT(0), &poll_block, &more));
		while (more)
		{
			RedrawEvent e(id_block, poll_block);
			for (int j = 0; j < redrawer_count; j++)
			{
				redrawers[j]->redraw(e);
			}
			if (_swix(Wimp_GetRectangle, _IN(1)|_OUT(0), &poll_block, &more) != 0) more = 0;
		}
	}
}


/**
 * Get the gadget with the given component id.
 *
 * Note: The id is not validated.
 */
Gadget Window::gadget(ComponentId component_id)
{
	return Gadget(_handle, component_id);
}

/**
 * Get the window from the wimp window handle
 *
 * @param window_handle the wimp window handle
 * @returns A window object for the handle. If the handle was not found
 *          an uninitialised (null) window will be returned.
 */
Window Window::from_handle(WindowHandle window_handle)
{
	ObjectId handle = NULL_ObjectId;

	// Use Window_WimpToToolbox to get object ids
	_swix(0x82884, _INR(0,2)|_OUT(0), 0, window_handle, -1, &handle);

	return Window(handle);
}

/**
 * Get a gadget from a given wimp window/icon handle pair.
 *
 * @param window_handle the wimp window handle
 * @param icon_handle the wimp icon handle
 * @param window pointer to Window to update with the details of the window
 *        or 0 if window details are not required. If the window was not
 *        found the Window will be set to be uninitialised (null).
 * @returns a Gadget for the given handles. If not found the an uninitialised
 *         (null) gadget is returned.
 */
Gadget Window::from_handle(WindowHandle window_handle, IconHandle icon_handle, Window *window /* = 0*/)
{
	ObjectId handle = NULL_ObjectId;
	ComponentId id = NULL_ComponentId;

	// Use Window_WimpToToolbox to get object ids
	_swix(0x82884, _INR(0,2)|_OUTR(0,1), 0, window_handle, icon_handle,
			&handle, &id);

	if (window) window->_handle = handle;

	return Gadget(handle, id);
}

/**
 * Gets a summary of the current state of the window.
 *
 * Usually this call isn't needed as the main information
 * it returns is given during redraw events anyway.
 *
 * It can be useful if the state is required outside of
 * a redraw event or if the window flags are required.
 *
 * @param state window state structure to update with the state
 * @throws OsError unable to retrieve state for window.
 */
void Window::get_state(WindowState &state) const
{
	state._window_handle = window_handle();
	swix_check(_swix(Wimp_GetWindowState, _INR(1,2), &(state._window_handle),0));
}

/**
 * Gets a summary of the current state of the window.
 *
 * Usually this call isn't needed as the main information
 * it returns is given during redraw events anyway.
 *
 * It can be useful if the state is required outside of
 * a redraw event or if the window flags are required.
 *
 * This version alsoe returns the parent window and alignment flags
 * for sub windows
 *
 * @param state window state structure to update with the state
 * @param parent Updated to parent window of a subwindow (may be null())
 * @param align_flags alignment flags of a subwindow
 * @throws OsError unable to retrieve state for window.
 */
void Window::get_state(WindowState &state, Window &parent, int &align_flags) const
{
    WindowHandle pwh;
	state._window_handle = window_handle();
	swix_check(_swix(Wimp_GetWindowState, _INR(1,2)|_OUTR(3,4),
	 &(state._window_handle),
     0x4b534154, // "TASK" magic word to mean use extended call
     &pwh,
     &align_flags
	 ));

	 if (pwh == -1) parent = Window();
	 else parent = Window::from_handle(pwh);
}

/**
 * Get complete information about the windows state
 *
 * @param info updated with information about the window
 * @throws OsError unable to retrieve state for window.
 */
void Window::get_info(WindowInfo &info) const
{
	info._window_handle = window_handle();
	swix_check(_swix(Wimp_GetWindowInfo, _IN(1), ((int)(&(info._window_handle)))|1));
}

/**
 * Open window in a new location.
 *
 * This should only be called once a window has been shown.
 * Resize, scroll and/or change the window stack position.
 *
 * Note that all coordinates are rounded down to whole numbers of pixels
 *
 * @param open_info new information to be used by the for the window
 * @throws OsError if unable to open the window
 */
void Window::open_window(const WindowOpenInfo &open_info)
{
	swix_check(_swix(Wimp_OpenWindow, _INR(1,2),
	        &(open_info._window_handle),
	        0 // Ensure non-extended version is used
	        ));
}

/**
 * Open a window as a subwindow.
 *
 * A subwindow is a child window nested inside another window
 *
 * @param open_info new information to be used by the window.
 * @param parent Window to make this a subwindow of
 * @param align_flags use one the AlignFlags to specify how the edges of
 * the subwindow are aligned to its parent window.
 */
void Window::open_subwindow(const WindowOpenInfo &open_info, Window &parent, unsigned int align_flags)
{
    swix_check(_swix(Wimp_OpenWindow, _INR(1,4),
            &(open_info._window_handle),
            0x4b534154, // "TASK" magic word to mean use extended call
            parent.window_handle(),
            align_flags));
}

/**
 * Scroll window to given coordinates
 *
 * This should only be called once a window has been shown.
 *
 * @throws OsError if unable to scroll window
 */
void Window::scroll(int x, int y)
{
	WindowState state;
	get_state(state);
	state.visible_area().scroll().x = x;
	state.visible_area().scroll().y = y;
	open_window(state);
}

/**
 * Scroll window to given coordinates
 *
 * This should only be called once a window has been shown.
 *
 * @throws OsError if unable to scroll window
 */
void Window::scroll(const Point &pos)
{
	WindowState state;
	get_state(state);
	state.visible_area().scroll() = pos;
	open_window(state);
}

/**
 * Returns the current scroll coordinates in the window
 *
 * This should only be called once a window has been shown.
 *
 * @throws OsError if unable to retrieve them
 */
Point Window::scroll() const
{
	WindowState state;
	get_state(state);
	return state.visible_area().scroll();
}

/**
 * Set the size of the window
 *
 * This should only be called once a window has been shown.
 *
 * @throws OsError if unable to set the new size
 */
void Window::size(int width, int height)
{
	WindowState state;
	get_state(state);
	state.visible_area().bounds().size(width, height);
	open_window(state);
}
/**
 * Set the size of the window
 *
 * This should only be called once a window has been shown.
 *
 * @throws OsError if unable to set the new size
 */
void Window::size(const Size &size)
{
	WindowState state;
	get_state(state);
	state.visible_area().bounds().size(size);
	open_window(state);
}

/**
 * Returns the size of the window
 *
 * This should only be called once a window has been shown.
 *
 * @throws OsError if unable to get the size
 */
Size Window::size() const
{
	WindowState state;
	get_state(state);
	return state.visible_area().bounds().size();
}

/**
 * Set the position of the top left of the visible
 * area of the Window on the desktop
 *
 * This should only be called once a window has been shown.
 *
 * @throws OsError if unable to reposition the window
 */
void Window::top_left(int x, int y)
{
	WindowState state;
	get_state(state);
	BBox &bounds = state.visible_area().bounds();
	bounds.move(x - bounds.min.x, y - bounds.max.y);
	open_window(state);
}

/**
 * Set the position of the top left of the visible
 * area of the Window on the desktop
 *
 * This should only be called once a window has been shown.
 *
 * @throws OsError if unable to reposition the window
 */
void Window::top_left(const Point &pos)
{
	WindowState state;
	get_state(state);
	BBox &bounds = state.visible_area().bounds();
	bounds.move(pos.x - bounds.min.x, pos.y - bounds.max.y);
	open_window(state);
}

/**
 * Get the position of the top left of the visible
 * area of the Window on the desktop
 *
 * This should only be called once a window has been shown.
 *
 * @throws OsError if unable to reposition the window
 */
Point Window::top_left() const
{
	WindowState state;
	get_state(state);
	const BBox &bounds = state.visible_area().bounds();
	return Point (bounds.min.x, bounds.max.y);
}

/**
 * Set position and size of the visible area of window on the desktop
 *
 * This should only be called once a window has been shown.
 *
 * @throws OsError if unable to reposition the window
 */
void Window::bounds(const BBox &bounds)
{
	WindowState state;
	get_state(state);
	state.visible_area().bounds() = bounds;
	open_window(state);
}

/**
 * Get position and size of the visible area of window on the desktop
 *
 * This should only be called once a window has been shown.
 *
 * @throws OsError if unable to reposition the window
 */
BBox Window::bounds() const
{
	WindowState state;
	get_state(state);
	return state.visible_area().bounds();
}

/**
 * Get the bounding box that completely covers the window on the
 * screen.
 *
 * Note: The window must be visible for this call if the Nested
 * Window Manager is not installed.
 *
 * @returns Bounding box for window (min inclusive, max exclusive).
 * @throws OsError if call fails
 */
BBox Window::outline() const
{
	int block[5];
	block[0] = window_handle();
    swix_check(_swix(Wimp_GetWindowOutline, _IN(1), block));

	return BBox(block[1],block[2],block[3],block[4]);
}


/**
 * Fast copy of a part of the work area to somewhere else.
 *
 * Uses VDU primitives where possible and invalidates other areas.
 *
 * @param bounds - rectangle to copy in work area coordinates
 * @param to new location in work area coordinates
 */
void Window::block_copy(const BBox &bounds, const Point &to)
{
	_kernel_swi_regs regs;
	regs.r[0] = window_handle();
	regs.r[1] = bounds.min.x;
	regs.r[2] = bounds.min.y;
	regs.r[3] = bounds.max.x;
	regs.r[4] = bounds.max.y;
	regs.r[5] = to.x;
	regs.r[6] = to.y;

	swix_check(_kernel_swi(Wimp_BlockCopy, &regs, &regs));
}

/**
 * Sets the focus to an invisible caret a postion 0,0 in the window.
 *
 * For more control on the position and visibility of the caret use
 * the Caret class.
 */
void Window::focus()
{
	Caret(*this).set();
}

/**
 * Add a listener for when the window is about to be shown.
 *
 * The default about to be shown event must be enabled in
 * the toolbox resources for the listeners to be called.
 */
void Window::add_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	add_listener(0x82880, listener, about_to_be_shown_router);
}

/**
 * Remove a listener for when a window is about to be shown.
 */
void Window::remove_about_to_be_shown_listener(AboutToBeShownListener *listener)
{
	remove_listener(0x82880, listener);
}

/**
 * Add A listener for when the window has been hidden.
 *
 * The default has been hidden event must be enabled in
 * the toolbox resources for the listeners to be called.
 */
void Window::add_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	add_listener(0x82890, listener, has_been_hidden_router);
}

/**
 * Remove a listener for when a window has been hidden
 */
void Window::remove_has_been_hidden_listener(HasBeenHiddenListener *listener)
{
	remove_listener(0x82890, listener);
}

/**
 * Add a listener to be called when the window needs redrawing.
 *
 * This listener will only be called if the auto redraw flag on
 * the window has been unset.
 *
 * The listeners are called in the order they have been added
 */
void Window::add_redraw_listener(RedrawListener *listener)
{
	event_router()->add_window_event_listener(_handle, 1, listener);
}

/**
 * Remove a listener for redraw events.
 */
void Window::remove_redraw_listener(RedrawListener *listener)
{
	event_router()->remove_window_event_listener(_handle, 1, listener);
}

/**
 * Add a listener to be called when the window is opened.
 *
 * This listener is only called it the auto open flag in
 * the window resource is unset.
 *
 * An open window event is generated whenever this visible
 * area, scroll position or stacking order of a window
 * is changed. This includes when the Window is first show.
 *
 * The tbx library automatically calls the Wimp_OpenWindow
 * swi to realise the change on the screen, so it should not
 * be called from the listener.
 */
void Window::add_open_window_listener(OpenWindowListener *listener)
{
	event_router()->add_window_event_listener(_handle, 2, listener);
}

/**
 * Remove a listener for window opened events.
 */
void Window::remove_open_window_listener(OpenWindowListener *listener)
{
	event_router()->remove_window_event_listener(_handle, 2, listener);
}

/**
 * Add a listener to be called when the window has received a
 * request to close.
 *
 * This event is only generated when the auto close flag in
 * the resource is unset.
 */
void Window::add_close_window_listener(CloseWindowListener *listener)
{
	event_router()->add_window_event_listener(_handle, 3, listener);
}

/**
 * Remove a listener for close window events
 */
void Window::remove_close_window_listener(CloseWindowListener *listener)
{
	event_router()->remove_window_event_listener(_handle, 3, listener);
}

/**
 * Add a listener for the pointer leaving the visible work area of
 * the window.
 */
void Window::add_pointer_leaving_listener(PointerLeavingListener *listener)
{
	event_router()->add_window_event_listener(_handle, 4, listener);
}

/**
 * Remove a listener for pointer leaving events
 */
void Window::remove_pointer_leaving_listener(PointerLeavingListener *listener)
{
	event_router()->remove_window_event_listener(_handle, 4, listener);
}

/**
 * Add a listener of the pointer entering the visible work area of the window
 */
void Window::add_pointer_entering_listener(PointerEnteringListener *listener)
{
	event_router()->add_window_event_listener(_handle, 5, listener);
}

/**
 * Remove a listener for pointer entering events
 */
void Window::remove_pointer_entering_listener(PointerEnteringListener *listener)
{
	event_router()->remove_window_event_listener(_handle, 5, listener);
}


/**
 * Add a listener for mouse click events.
 *
 * This listener will only be called if an approriate button type
 * has been set in the Windows resources.
 */
void Window::add_mouse_click_listener(MouseClickListener *listener)
{
	event_router()->add_window_event_listener(_handle, 6, listener);
}

/**
 * Remove a listener for mouse click events.
 */
void Window::remove_mouse_click_listener(MouseClickListener *listener)
{
	event_router()->remove_window_event_listener(_handle, 6, listener);
}

/**
 * Add listener for a mouse click on a window or any of its components
 *
 * @details  Adds a listener that runs when a mouse is clicked on the window or
 *           any of the components it contains that pass through the mouse button
 *           clicks. The window/component may need to be created with the flags to
 *           enable mouse click events.
 *           This event is called in addition to any window or gadget mouse click
 *           listeners.
 */
void Window::add_all_mouse_click_listener(MouseClickListener *listener)
{
	event_router()->add_window_event_listener(_handle, WINDOW_AND_COMPONENT_MOUSE_CLICK, listener);
}
/**
 * Remove listener for key press on window or any of its components.
 *
 */
void Window::remove_all_mouse_click_listener(MouseClickListener *listener)
{
	event_router()->remove_window_event_listener(_handle, WINDOW_AND_COMPONENT_MOUSE_CLICK, listener);
}

/**
 * Add a listener for key pressed events on this window
 * @param listener Key listener to call when a key if pressed
 */
void Window::add_key_listener(KeyListener *listener)
{
	event_router()->add_window_event_listener(_handle, 8, listener);
}

/**
 * Remove a listener for key pressed events on this window .
 */
void Window::remove_key_listener(KeyListener *listener)
{
	event_router()->remove_window_event_listener(_handle, 8, listener);
}

/**
 * Add a listener for key pressed events on this window or the components it contains
 * @details The key event is only generated by some components as others may use it
 *          and not pass it on.
 *          If a component or window key pressed listener also intercepts this event
 *          and sets the key used field this event will not be called as well.
 * @param listener Key listener to call when a key if pressed
 */
void Window::add_all_key_listener(KeyListener *listener)
{
	event_router()->add_window_event_listener(_handle,WINDOW_AND_COMPONENT_KEY_PRESSED, listener);
}

/**
 * Remove a listener for key pressed events on this window or the components it contains
 */
void Window::remove_all_key_listener(KeyListener *listener)
{
	event_router()->remove_window_event_listener(_handle, WINDOW_AND_COMPONENT_KEY_PRESSED, listener);
}

/**
 * Add a listener for when the window loses the caret
 */
void Window::add_lose_caret_listener(LoseCaretListener *listener)
{
	event_router()->add_window_event_listener(_handle, 11, listener);
}

/**
 * Remove a listener for when the window loses the caret
 */
void Window::remove_lose_caret_listener(LoseCaretListener *listener)
{
	event_router()->remove_window_event_listener(_handle, 11, listener);
}

/**
 * Add a listener for when the window gains the caret
 */
void Window::add_gain_caret_listener(GainCaretListener *listener)
{
	event_router()->add_window_event_listener(_handle, 12, listener);
}

/**
 * Remove a listener for when the window gains the caret
 */
void Window::remove_gain_caret_listener(GainCaretListener *listener)
{
	event_router()->remove_window_event_listener(_handle, 12, listener);
}

/**
 * Add listener for when a click has occurred on a window's scroll bar.
 *
 * The appropriate flag must of been set in the window when it was
 * created for this event to be generated.
 */
void Window::add_scroll_request_listener(ScrollRequestListener *listener)
{
	event_router()->add_window_event_listener(_handle, 10, listener);
}

/**
 * Remove listener for when a click has occurred on a window's scroll bar.
 */
void Window::remove_scroll_request_listener(ScrollRequestListener *listener)
{
	event_router()->remove_window_event_listener(_handle, 10, listener);
}

/**
 * Add a file loader.
 *
 * @param loader the loader to add
 * @param file_type the file type for the loader or -2 (the default) for
 *        any type.
 */
void Window::add_loader(Loader *loader, int file_type /*=-2*/)
{
	LoaderManager *manager = LoaderManager::instance();
	if (manager == 0) manager = new LoaderManager();
	manager->add_loader(_handle, NULL_ComponentId, file_type, loader);
}

/**
 * Remove a file loader.
 *
 * @param loader the loader to remove
 * @param file_type the file type for the loader or -2 (the default) for
 *        any type.
 */
void Window::remove_loader(Loader *loader, int file_type /*=-2*/)
{
	LoaderManager *manager = LoaderManager::instance();
	if (manager != 0)
		manager->remove_loader(_handle, NULL_ComponentId, file_type, loader);
}

/**
 * Start drag operation of the mouse pointer.
 *
 * Usually called in response to a mouse drag
 *
 * handler is called when the drag finishes.
 *
 * @param handler handler to be called when drag finishes
 */
void Window::drag_point(DragHandler *handler) const
{
	ModeInfo m;
	Size screen_size = m.screen_size();

	int drag[10];
	drag[0] = window_handle();
	drag[1] = 7; // Drag point
	drag[2] = 0; // Not used
	drag[3] = 0; // Not used
	drag[4] = 0; // Not used
	drag[5] = 0; // Not used
	drag[6] = 0; // Min screen x - inclusive
	drag[7] = 0; // Min screen y - inclusive
	drag[8] = screen_size.width; // Max screen X - exclusive
	drag[9] = screen_size.height; // Max screen Y - exclusive

	swix_check(_swix(Wimp_DragBox, _IN(1), drag));
	event_router()->set_drag_handler(handler);
}

/**
 * Drag mouse pointer constrained to this window
 *
 * @param handler handler to be called when drag finishes
 */
void Window::drag_point_local(DragHandler *handler) const
{
	WindowState state;
	get_state(state);

	int drag[10];
	drag[0] = window_handle();
	drag[1] = 7;
	drag[2] = 0; // Not used
	drag[3] = 0; // Not used
	drag[4] = 0; // Not used
	drag[5] = 0; // Not used
	drag[6] = state.visible_area().bounds().min.x;
	drag[7] = state.visible_area().bounds().min.y;
	drag[8] = state.visible_area().bounds().max.x;
	drag[9] = state.visible_area().bounds().max.y;

	swix_check(_swix(Wimp_DragBox, _IN(1), drag));
	event_router()->set_drag_handler(handler);
}

/**
 * Drag mouse pointer constrained to given bounds
 *
 * @param bounds point is constrains in (screen coordinates)
 * @param handler handler to be called when drag finishes
 */
void Window::drag_point(const BBox &bounds, DragHandler *handler) const
{
	int drag[10];
	drag[0] = window_handle();
	drag[1] = 7; // Drag point
	drag[2] = 0; // Not used
	drag[3] = 0; // Not used
	drag[4] = 0; // Not used
	drag[5] = 0; // Not used
	drag[6] = bounds.min.x;
	drag[7] = bounds.min.y;
	drag[8] = bounds.max.x;
	drag[9] = bounds.max.y;

	swix_check(_swix(Wimp_DragBox, _IN(1), drag));
	event_router()->set_drag_handler(handler);
}

/**
 * Drag fixed rotating dots box
 *
 * @param box box to drag
 * @param handler handler to be called when drag finishes
 */

void Window::drag_box(const BBox &box, DragHandler *handler) const
{
	ModeInfo m;
	Size screen_size = m.screen_size();
	int drag[10];
	drag[0] = window_handle();
	drag[1] = 5; // Drag box
	drag[2] = box.min.x;
	drag[3] = box.min.y;
	drag[4] = box.max.x;
	drag[5] = box.max.y;
	drag[6] = 0; // Min screen x - inclusive
	drag[7] = 0; // Min screen y - inclusive
	drag[8] = screen_size.width; // Max screen X - exclusive
	drag[9] = screen_size.height; // Max screen Y - exclusive

	swix_check(_swix(Wimp_DragBox, _IN(1), drag));
	event_router()->set_drag_handler(handler);
}

/**
 * Drag fixed rotating dots box constrained to this window
 *
 * @param box box to drag
 * @param handler handler to be called when drag finishes
 */

void Window::drag_box_local(const BBox &box, DragHandler *handler) const
{
	WindowState state;
	get_state(state);

	int drag[10];
	drag[0] = window_handle();
	drag[1] = 5;
	drag[2] = box.min.x;
	drag[3] = box.min.y;
	drag[4] = box.max.x;
	drag[5] = box.max.y;
	drag[6] = state.visible_area().bounds().min.x;
	drag[7] = state.visible_area().bounds().min.y;
	drag[8] = state.visible_area().bounds().max.x;
	drag[9] = state.visible_area().bounds().max.y;

	swix_check(_swix(Wimp_DragBox, _IN(1), drag));
	event_router()->set_drag_handler(handler);
}

/**
 * Drag fixed rotating dots box constrained to given bounds
 *
 * @param box box to drag
 * @param bounds box is constrains in (screen coordinates)
 * @param handler handler to be called when drag finishes
 */
void Window::drag_box(const BBox &box, const BBox &bounds, DragHandler *handler) const
{
	int drag[10];
	drag[0] = window_handle();
	drag[1] = 5; // Drag box
	drag[2] = box.min.x;
	drag[3] = box.min.y;
	drag[4] = box.max.x;
	drag[5] = box.max.y;
	drag[6] = bounds.min.x;
	drag[7] = bounds.min.y;
	drag[8] = bounds.max.x;
	drag[9] = bounds.max.y;

	swix_check(_swix(Wimp_DragBox, _IN(1), drag));
	event_router()->set_drag_handler(handler);
}

/**
 * Start dragging of rubber rotating dashes box.
 *
 * @param start point of box that is dragged in screen coordinates
 * @param handler handler called when drag is finished
 */

void Window::drag_rubber_box(const Point &start, DragHandler *handler) const
{
	ModeInfo m;
	Size screen_size = m.screen_size();
	int drag[10];
	drag[0] = window_handle();
	drag[1] = 6;
	drag[2] = start.x;
	drag[3] = start.y;
	drag[4] = start.x;
	drag[5] = start.y;
	drag[6] = 0; // Min screen x - inclusive
	drag[7] = 0; // Min screen y - inclusive
	drag[8] = screen_size.width; // Max screen X - exclusive
	drag[9] = screen_size.height; // Max screen Y - exclusive

	swix_check(_swix(Wimp_DragBox, _IN(1), drag));
	event_router()->set_drag_handler(handler);
}

/**
 * Start dragging of rubber rotating dashes box confined
 * to inside the window.
 *
 * @param start point of box that is dragged in screen coordinates
 * @param handler handler called when drag is finished
 */
void Window::drag_rubber_box_local(const Point &start, DragHandler *handler) const
{
	WindowState state;
	get_state(state);

	int drag[10];
	drag[0] = window_handle();
	drag[1] = 6;
	drag[2] = start.x;
	drag[3] = start.y;
	drag[4] = start.x;
	drag[5] = start.y;
	drag[6] = state.visible_area().bounds().min.x;
	drag[7] = state.visible_area().bounds().min.y;
	drag[8] = state.visible_area().bounds().max.x;
	drag[9] = state.visible_area().bounds().max.y;

	swix_check(_swix(Wimp_DragBox, _IN(1), drag));
	event_router()->set_drag_handler(handler);
}

/**
 * Start dragging of rubber rotating dashes box confined
 * to inside the window.
 *
 * @param start point of box that is dragged in screen coordinates
 * @param bounds box is constrains in (screen coordinates)
 * @param handler handler called when drag is finished
 */
void Window::drag_rubber_box(const Point &start, const BBox &bounds, DragHandler *handler) const
{
	int drag[10];
	drag[0] = window_handle();
	drag[1] = 6;
	drag[2] = start.x;
	drag[3] = start.y;
	drag[4] = start.x;
	drag[5] = start.y;
	drag[6] = bounds.min.x;
	drag[7] = bounds.min.y;
	drag[8] = bounds.max.x;
	drag[9] = bounds.max.y;

	swix_check(_swix(Wimp_DragBox, _IN(1), drag));
	event_router()->set_drag_handler(handler);
}

/**
 * Cancel current drag operation
 */
void Window::cancel_drag() const
{
	swix_check(_swix(Wimp_DragBox, _IN(1), 0));
	event_router()->cancel_drag();
}

/**
 * Drag a sprite in the desktop
 *
 * @param sprite the sprite to drag. A copy is made to it does not have to exist once this routine returns.
 * @param box the box the sprite is contained within.
 * @param handler the drag handler that will be notified when the drag finishes.
 * @param flags location of sprite in the box and other sprite dragging options
 */
void Window::drag_sprite(const Sprite &sprite, const BBox &box, DragHandler *handler, int flags /*= 0*/) const
{
	swix_check(_swix(DragASprite_Start, _INR(0,3),
			flags, sprite.area_id(), sprite.name().c_str(),
			&box.min.x
			));
	event_router()->set_drag_handler(handler, DragASprite_Stop);
}

/**
 * Drag a sprite within the window the pointer is in the desktop
 *
 * @param sprite the sprite to drag. A copy is made to it does not have to exist once this routine returns.
 * @param box the box the sprite is contained within.
 * @param handler the drag handler that will be notified when the drag finishes.
 * @param flags location of sprite in the box and other sprite dragging options.
 */

void Window::drag_sprite_local(const Sprite &sprite, const BBox &box, DragHandler *handler, int flags /*= 0*/) const
{
	flags |= 16; // Restrict drag to local window flag
	swix_check(_swix(DragASprite_Start, _INR(0,4),
			flags, sprite.area_id(), sprite.name().c_str(),
			&box.min.x, 0
			));
	event_router()->set_drag_handler(handler, DragASprite_Stop);
}

/**
 * Drag a sprite within the specified bounds
 *
 * @param sprite the sprite to drag. A copy is made to it does not have to exist once this routine returns.
 * @param box the box the sprite is contained within.
 * @param bounds the bounding box the drag is restricted to.
 * @param handler the drag handler that will be notified when the drag finishes.
 * @param flags location of sprite in the box and other sprite dragging options.
 */

void Window::drag_sprite(const Sprite &sprite, const BBox &box, const BBox &bounds, DragHandler *handler, int flags /*= 0*/) const
{
	flags |= 32; // Restrict drag to specified window
	swix_check(_swix(DragASprite_Start, _INR(0,4),
			flags, sprite.area_id(), sprite.name().c_str(),
			&box.min.x, &bounds.min.x
			));
	event_router()->set_drag_handler(handler, DragASprite_Stop);
}
