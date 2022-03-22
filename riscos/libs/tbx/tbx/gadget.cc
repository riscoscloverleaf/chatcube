/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2015 Alan Buckley   All Rights Reserved.
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
 * gadget.cc
 *
 *  Created on: 12-Sep-2008
 *      Author: alanb
 */

#include "gadget.h"
#include "tbxexcept.h"
#include "swixcheck.h"
#include "swis.h"
#include "loadermanager.h"

using namespace tbx;

/**
 * Check the underlying gadget class for this object has
 * the given class id.
 *
 * @param class_id class id to check
 * @throws GadgetClassError if gadget class id is different or NULL.
 */
void Gadget::check_toolbox_class(int class_id) const
{
	if (_id == NULL_ComponentId) throw GadgetClassError();

	int tbox_class_id;
	swix_check(_swix(0x44ec6, _INR(0,3) | _OUT(0), 0,  _handle,
		70, _id, &tbox_class_id));

	// class is in lower 16 bits (top 16bits is size)
	if ((tbox_class_id  & 0xFFFF) != class_id) throw GadgetClassError();
}

/**
 * Get the toolbox class for a gadget
 * @returns gadget class
 * @throws GadgetClassError if gadget class id is NULL.
 */
int Gadget::toolbox_class() const
{
	if (_id == NULL_ComponentId) throw GadgetClassError();

	int tbox_class_id;
	swix_check(_swix(0x44ec6, _INR(0,3) | _OUT(0), 0,  _handle,
		70, _id, &tbox_class_id));

	// class is in lower 16 bits (top 16bits is size)
	return (tbox_class_id & 0xFFFFF);
}

/**
 * Get the toolbox class and size for a gadget
 * @returns gadget class (bottom 16 bits), size (top 16 bits)
 * @throws GadgetClassError if gadget class id is NULL.
 */
int Gadget::toolbox_class_and_size() const
{
	if (_id == NULL_ComponentId) throw GadgetClassError();

	int tbox_class_id;
	swix_check(_swix(0x44ec6, _INR(0,3) | _OUT(0), 0,  _handle,
		70, _id, &tbox_class_id));

	return tbox_class_id;
}

/**
 * Return the gadget flags.
 *
 * @returns the current flags for the gadget
 */
unsigned int Gadget::flags() const
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 64;
    regs.r[3] = _id;

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return regs.r[0];
}

/**
 * Sets bits in the gadgets flags
 *
 * @param set the flags to set
 */
void Gadget::set_flag(unsigned int set)
{
	_kernel_swi_regs regs;

    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 64;
    regs.r[3] = _id;

    // Run Toolbox_ObjectMiscOp - to get the current flags
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	regs.r[4] = regs.r[0] | set;
	regs.r[0] = 0;
	regs.r[2] = 65;
    // Run Toolbox_ObjectMiscOp - to set the current flags
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Clear bits in the gadget flags
 *
 * @param clear flags to clear
 */
void Gadget::clear_flag(unsigned int clear)
{
	_kernel_swi_regs regs;

    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 64;
    regs.r[3] = _id;

    // Run Toolbox_ObjectMiscOp - to get the current flags
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

	regs.r[4] = regs.r[0] & ~clear;
	regs.r[0] = 0;
	regs.r[2] = 65;
    // Run Toolbox_ObjectMiscOp - to set the current flags
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Fade the gadget
 */
void Gadget::fade()
{
	set_flag(1<<31);
}

/**
 * Clear the gadgets fade state
 */
void Gadget::unfade()
{
	clear_flag(1<<31);
}

/**
 * Fade/unfade the gadget
 *
 * @param fade true to fade the gadget/false to unfade it.
 */
void Gadget::fade(bool fade)
{
	if (fade) set_flag(1<<31);
	else clear_flag(1<<31);
}

/**
 * Check if the gadget is faded
 *
 * @returns true if the gadget is faded
 */
bool Gadget::faded() const
{
	return (flags() & (1 << 31)) != 0;
}

/**
 * Set input focus to the gadget.
 *
 * The gadget must be a writeable field or a composite gadget
 * that includes a writeable field.
 */
void Gadget::focus()
{
	_kernel_swi_regs regs;

    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 69;
    regs.r[3] = _id;

    // Run Toolbox_ObjectMiscOp - to get the current flags
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Get list of icons used in this gadget
 *
 * @returns vector of icons used (could be empty)
 */
std::vector<IconHandle> Gadget::icon_list()
{
	std::vector<IconHandle> result;
	_kernel_swi_regs regs;

	regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 68;
    regs.r[3] = _id;
    regs.r[4] = 0; // 0 to get size of buffer
    regs.r[5] = 0;
    // Run Toolbox_ObjectMiscOp - to get size of buffer
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    if (regs.r[5] > 0)
    {
    	int num = regs.r[5] / 4;
    	IconHandle temp[num];
    	regs.r[4] = (int)temp;
        // Run Toolbox_ObjectMiscOp - to get read the icons
        swix_check(_kernel_swi(0x44ec6, &regs, &regs));
        result.assign(temp, temp + num);
    }

    return result;
}

/**
 * Get the bounding box of the gadget
 */
BBox Gadget::bounds() const
{
	BBox bounds;
	_kernel_swi_regs regs;

	regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 72;
    regs.r[3] = _id;
    regs.r[4] = reinterpret_cast<int>(&bounds.min.x);
    // Run Toolbox_ObjectMiscOp - to get bounds
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return bounds;
}

/**
 * Set the bounds of the gadget.
 *
 * This can move and resize the gadget.
 *
 * @param bounds - new bounds for gadget
 */
void Gadget::bounds(const BBox &bounds)
{
	_kernel_swi_regs regs;

	regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 71;
    regs.r[3] = _id;
    regs.r[4] = reinterpret_cast<int>(&bounds.min.x);
    // Run Toolbox_ObjectMiscOp - to set bounds
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

}

/**
 * Get the bottom left location of gadget
 *
 * @returns Point containing bottom left of the gadget
 */
Point Gadget::bottom_left() const
{
	BBox bounds;
	_kernel_swi_regs regs;

	regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 72;
    regs.r[3] = _id;
    regs.r[4] = reinterpret_cast<int>(&bounds.min.x);
    // Run Toolbox_ObjectMiscOp - to get bounds
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return bounds.min;
}

/**
 * Get the top left location of gadget
 *
 * @returns Point containing top left of the gadget
 */
Point Gadget::top_left() const
{
	BBox bounds;
	_kernel_swi_regs regs;

	regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 72;
    regs.r[3] = _id;
    regs.r[4] = reinterpret_cast<int>(&bounds.min.x);
    // Run Toolbox_ObjectMiscOp - to get bounds
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return Point(bounds.min.x, bounds.max.y);
}

/**
 * Move gadget (without resizing)
 *
 * @param pos - new bottom left of gadget
 */
void Gadget::move_to(const Point &pos)
{
	BBox bounds;
	_kernel_swi_regs regs;

	regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 72;
    regs.r[3] = _id;
    regs.r[4] = reinterpret_cast<int>(&bounds.min.x);
    // Run Toolbox_ObjectMiscOp - to get bounds
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    bounds.move_to(pos);
    regs.r[2] = 71;
    // Run Toolbox_ObjectMiscOp - to set bounds
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Move gadget without resizing
 *
 * @param x new minimum x coordinate
 * @param y new minimum y coordinate
 */
void Gadget::move_to(int x, int y)
{
	BBox bounds;
	_kernel_swi_regs regs;

	regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 72;
    regs.r[3] = _id;
    regs.r[4] = reinterpret_cast<int>(&bounds.min.x);
    // Run Toolbox_ObjectMiscOp - to get bounds
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    bounds.move_to(x,y);

    regs.r[2] = 71;
    // Run Toolbox_ObjectMiscOp - to get bounds
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Move gadget relative to it's current position without resizing
 *
 * @param bx amount to move gadget in x direction (across)
 * @param by amount to move gadget in y direction (up)
 */
void Gadget::move_by(int bx, int by)
{
	BBox bounds;
	_kernel_swi_regs regs;

	regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 72;
    regs.r[3] = _id;
    regs.r[4] = reinterpret_cast<int>(&bounds.min.x);
    // Run Toolbox_ObjectMiscOp - to get bounds
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    bounds.move(bx,by);

    regs.r[2] = 71;
    // Run Toolbox_ObjectMiscOp - to get bounds
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Return the size of the gadget
 *
 * @returns size of gadget
 */
Size Gadget::size() const
{
	BBox bounds;
	_kernel_swi_regs regs;

	regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 72;
    regs.r[3] = _id;
    regs.r[4] = reinterpret_cast<int>(&bounds.min.x);
    // Run Toolbox_ObjectMiscOp - to get bounds
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return bounds.size();
}

/**
 * Set size of gadget without moving it
 *
 * @param sz new size of gadget
 */
void Gadget::size(const Size &sz)
{
	BBox bounds;
	_kernel_swi_regs regs;

	regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = 72;
    regs.r[3] = _id;
    regs.r[4] = reinterpret_cast<int>(&bounds.min.x);
    // Run Toolbox_ObjectMiscOp - to get bounds
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    bounds.size(sz);

    regs.r[2] = 71;
    // Run Toolbox_ObjectMiscOp - to set bounds
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}

/**
 * Add a file loader.
 *
 * @param loader the loader to add
 * @param file_type the file type for the loader or -2 (the default) for
 *        any type.
 */
void Gadget::add_loader(Loader *loader, int file_type /*=-2*/)
{
	LoaderManager *manager = LoaderManager::instance();
	if (manager == 0) manager = new LoaderManager();
	manager->add_loader(_handle, _id, file_type, loader);
}

/**
 * Remove a file loader.
 *
 * @param loader the loader to remove
 * @param file_type the file type for the loader or -2 (the default) for
 *        any type.
 */
void Gadget::remove_loader(Loader *loader, int file_type /*=-2*/)
{
	LoaderManager *manager = LoaderManager::instance();
	if (manager != 0)
		manager->remove_loader(_handle, _id, file_type, loader);
}
