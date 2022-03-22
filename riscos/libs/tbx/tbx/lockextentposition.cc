/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2015 Alan Buckley   All Rights Reserved.
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

#include "lockextentposition.h"
#include <stdexcept>

namespace tbx
{

/**
 * Construct lock extent position for a given window
 *
 * @param w window to lock gadget positions in
 */
LockExtentPosition::LockExtentPosition(tbx::Window &w) :
   _window(w),
   _num_gadgets(0),
   _items(0)
{
   _last_extent = _window.extent();
}

/**
 * Destructor free resources and remove window listener used
 */
LockExtentPosition::~LockExtentPosition()
{
   delete [] _items;
}

/**
 * Set the window to lock the gadget positions too
 *
 * This can only be changed if there are no gadgets added.
 *
 * @param window the window to lock the positions too
 *
 * @throws logic_error if the window is already set and gadgets have been locked
 */
void LockExtentPosition::window(tbx::Window &window)
{
   if (!_window.null())
   {
      if (_items) throw std::logic_error("Can't change window once gadgets have been added");
   }
   _window = window;
   _last_extent = window.extent();
}

/**
 * Add a gadget to the list to be locked.
 *
 * @param gadget the gadget to add
 * @param lock_flags flags determine how to lock the gadget edges.
 * Use a combination of the LockFlags enum.
 */
void LockExtentPosition::add_gadget(tbx::Gadget gadget, unsigned int lock_flags)
{
   Item *tmp = new Item[_num_gadgets+1];
   if (_num_gadgets > 0)
   {
      for (int j = 0; j < _num_gadgets; j++)
      {
         tmp[j] = _items[j];
      }
      delete [] _items;
   }
   _items = tmp;
   _items[_num_gadgets].first = gadget;
   _items[_num_gadgets++].second = lock_flags;
}

/**
 * Add a gadget to the list to be locked from its component id
 *
 * @param id component id of gadget to be locked
 * @param lock_flags flags determine how to lock the gadget edges.
 * Use a combination of the LockFlags enum.
 */
void LockExtentPosition::add_gadget(tbx::ComponentId id, unsigned int lock_flags)
{
    add_gadget(_window.gadget(id), lock_flags);
}

/**
 * Add range of gadgets with the same lock flags
 *
 * @param from first gadget id
 * @param to last gadget id
 * @param lock_flags flags determine how to lock the gadget edges.
 * Use a combination of the LockFlags enum.
 */
void LockExtentPosition::add_gadgets(tbx::ComponentId from, tbx::ComponentId to, unsigned int lock_flags)
{
   int add = to - from + 1;
   Item *tmp = new Item[_num_gadgets+add];
   if (_num_gadgets > 0)
   {
      for (int j = 0; j < _num_gadgets; j++)
      {
         tmp[j] = _items[j];
      }
      delete [] _items;
   }
   _items = tmp;
   for (int comp = from; comp <= to; comp++)
   {
       _items[_num_gadgets].first = _window.gadget(comp);
       _items[_num_gadgets++].second = lock_flags;
   }
}

/**
 * Remove gadget from list
 *
 * @param gadget gadget to remove
 * @throws std::invalid_argument if gadget has not been added
 */
void LockExtentPosition::remove_gadget(tbx::Gadget gadget)
{
   int index = -1;
   int j;
   for (j = 0; j < _num_gadgets && index == -1; j++)
   {
       if (_items[j].first == gadget) index = j;
   }
   if (index == -1) throw std::invalid_argument("Gadget has not been added");

   Item *tmp;
   if (_num_gadgets == 1) tmp = 0;
   else
   {
      tmp = new Item[_num_gadgets-1];
      for (j = 0; j < index; j++) tmp[j] = _items[j];
      for (j = index+1; j < _num_gadgets; j++) tmp[j-1] = _items[j];
   }
   _num_gadgets--;
   delete [] _items;
   _items = tmp;
}

/**
 * Remove gadget given its component id
 *
 * @param id component id to remove
 * @throws std::invalid_argument if id has not been added
 */
void LockExtentPosition::remove_gadget(tbx::ComponentId id)
{
   remove_gadget(_window.gadget(id));
}

/**
 * Remove a range of gadgets
 *
 * @param from first gadget id
 * @param to last gadget id
 * @throws std::invalid_argument if ids have not been added
 */
void LockExtentPosition::remove_gadgets(tbx::ComponentId from, tbx::ComponentId to)
{
   for (tbx::ComponentId comp = from; comp <= to; comp++)
   {
      remove_gadget(_window.gadget(comp));
   }
}
/**
 * Clear list of gadget locked in place
 */
void LockExtentPosition::clear()
{
   delete [] _items;
   _items = 0;
   _num_gadgets = 0;
}

/**
 * Method to check if window extent has changed and move
 * the gadgets if necessary
 */
void LockExtentPosition::extent_changed()
{
    tbx::BBox new_extent = _window.extent();
    int left_move = new_extent.min.x  - _last_extent.min.x;
    int right_move = new_extent.max.x - _last_extent.max.x;
    int bottom_move = new_extent.min.y - _last_extent.min.y;
    int top_move = new_extent.max.y - _last_extent.max.y;
    unsigned int move_flags = 0;
    if (left_move) move_flags |= left_to_left | right_to_left;
    if (right_move) move_flags |= left_to_right | right_to_right;
    if (bottom_move) move_flags |= bottom_to_bottom | top_to_bottom;
    if (top_move) move_flags |= bottom_to_top | top_to_top;

    if (move_flags)
    {
       _last_extent = new_extent;
       tbx::BBox bounds;

       for (int j = 0; j < _num_gadgets; j++)
       {
           unsigned int flags = _items[j].second;

           if (flags & move_flags)
           {
              tbx::Gadget &g = _items[j].first;
              bounds = g.bounds();
              if (flags & left_to_left) bounds.min.x += left_move;
              if (flags & left_to_right) bounds.min.x += right_move;
              if (flags & right_to_left) bounds.max.x += left_move;
              if (flags & right_to_right) bounds.max.x += right_move;
              if (flags & bottom_to_top) bounds.min.y += top_move;
              if (flags & bottom_to_bottom) bounds.min.y += bottom_move;
              if (flags & top_to_top) bounds.max.y += top_move;
              if (flags & top_to_bottom) bounds.max.y += bottom_move;

              g.bounds(bounds);

           }
       }
    }
}

} /* namespace tbx */
