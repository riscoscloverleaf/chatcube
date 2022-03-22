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

#ifndef _TBX_VIEW_GADGETLISTVIEW_
#define _TBX_VIEW_GADGETLISTVIEW_

#include "../window.h"
#include "../gadget.h"
#include "../margin.h"
#include "../res/reswindow.h"

namespace tbx
{
namespace view
{
class GadgetListView;

/**
 * Interface for copying from one row to another
 */
class GadgetListCopyRow
{
   public:
     GadgetListCopyRow() {}
     virtual ~GadgetListCopyRow() {}

     /**
      * Copy data from one row in a gadget list to another
      *
      * Base version just copies the value for the
      * standard gadget types.
      *
      * @param glv GadgetListView to copy rows from
      * @param from_row row to copy from
      * @param to_row row to copy to
      */
     virtual void copy_row(GadgetListView &glv, unsigned int from_row, unsigned int to_row);
};

/**
 * Class to create a list view with each line made up of
 * one or more gadgets
 */
class GadgetListView
{
private:
   tbx::Window _window;
   tbx::res::ResGadget **_res_gadgets;
   tbx::ComponentId _first_gadget_id;
   tbx::ComponentId _last_gadget_id;
   unsigned int _num_gadgets;
   tbx::Margin _margin;
   unsigned int _width;
   unsigned int _row_height;
   unsigned int _spacing;
   unsigned int _count;
   GadgetListCopyRow *_copy_interface;
   bool _updates_enabled;
   static GadgetListCopyRow _default_copy;

public:
   GadgetListView();
   ~GadgetListView();

   void setup(tbx::Window window, tbx::res::ResWindow &res_window, tbx::ComponentId first_gadget_id, tbx::ComponentId last_gadget_id, unsigned int spacing, bool first_row_created);

   /**
    * Return the number of gadgets in a row
    */
   unsigned int num_gadgets() const {return _num_gadgets;}
   /**
    * Return the number of rows
    */
   unsigned int count() const {return _count;}

   /**
    * Return the window this view is attached to
    */
   tbx::Window &window() {return _window;}

   /**
    * Return the margin around the data in the window.
    */
   const tbx::Margin &margin() const {return _margin;}
   void margin(const tbx::Margin &margin);

   void copy_interface(GadgetListCopyRow *copy);

   /**
    * Check if updating of the window extent is enabled
    * @returns true if updates are enabled
    */
   bool updates_enabled() const {return _updates_enabled;}
   /**
    * Set if updating of the window extent is enabled
    *
    * After turning on updates call update_window_extent to refresh the
    * windows extent
    &
    * @param enabled true to turn on extent updates
    */
   void updates_enabled(bool enabled) {_updates_enabled = enabled;}

   void update_window_extent();

   /**
    * Return the gadget_id for a specific row and column
    *
    * @param row the 0 based row number
    * @param col the 0 based column number
    * @returns component id at the given row or column
    */
   tbx::ComponentId gadget_id(int row, int col) const
   {
      return row * _num_gadgets + _first_gadget_id + col;
   }

   /**
    * Return the gadget for a specified row and column
    *
    */
   tbx::Gadget gadget(int row, int col)
   {
      return _window.gadget(row * _num_gadgets + _first_gadget_id + col);
   }

   tbx::ComponentId add(unsigned int count = 1);
   tbx::ComponentId insert(unsigned int row, unsigned int count = 1);
   void remove(unsigned int row, unsigned int count = 1);
   void clear();

   void copy(unsigned int from_row, unsigned int to_row);
};


} /* end of view namespace */
} /* end of tbx namespace */


#endif
