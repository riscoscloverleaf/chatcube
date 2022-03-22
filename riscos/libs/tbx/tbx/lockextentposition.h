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

#ifndef TBX_LOCKEXTENTPOSITION_H
#define TBX_LOCKEXTENTPOSITION_H

#include "window.h"
#include "gadget.h"
#include <utility>

namespace tbx
{

/**
 * Class to lock the position of gadgets relative to
 * the work area of the window they are in.
 *
 * As there is no extent change listener for the window,
 * the extent_changed method must be called after any
 * change ot the window extent.
 */
class LockExtentPosition
{
   tbx::Window _window;
   tbx::BBox _last_extent;
   int _num_gadgets;
   typedef std::pair<tbx::Gadget, unsigned int> Item;
   Item *_items;

public:
   LockExtentPosition() : _num_gadgets(0), _items(0) {}
   LockExtentPosition(tbx::Window &w);
   virtual ~LockExtentPosition();

   /**
    * Return window gadgets are locked to
    */
   tbx::Window &window() {return _window;}
   void window(tbx::Window &window);

   /**
    * Lock states, shifted for each edge
    */
   enum LockFlags
      {
         left_to_left = 1,
         left_to_right = 2,
         right_to_left = 4,
         right_to_right = 8,
         bottom_to_top = 16,
         bottom_to_bottom = 32,
         top_to_top = 64,
         top_to_bottom = 128
      };

   void add_gadget(tbx::Gadget gadget, unsigned int lock_flags);
   void add_gadget(tbx::ComponentId id, unsigned int lock_flags);
   void add_gadgets(tbx::ComponentId from, tbx::ComponentId to, unsigned int lock_flags);
   void remove_gadget(tbx::Gadget gadget);
   void remove_gadget(tbx::ComponentId id);
   void remove_gadgets(tbx::ComponentId from, tbx::ComponentId to);
   void clear();

   void extent_changed();
};

} /* namespace tbx */

#endif
