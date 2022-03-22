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
#ifndef _TBX_GADGETCOPYVALUE_H
#define _TBX_GADGETCOPYVALUE_H

#include "writablefield.h"
#include "displayfield.h"
#include "button.h"
#include "numberrange.h"
#include "optionbutton.h"
#include "radiobutton.h"
#include "slider.h"
#include "stringset.h"

namespace tbx
{

/**
 * Copy the main value property from one gadget to another.
 *
 * Both gadgets should be of the same type and have the same configuration.
 * Gadgets supported are: Button (text only), DisplayField, NumberRange,
 * OptionButton, RadioButton, Slider, StringSet and WritableField.
 *
 * @param from_gadget the gadget to copy the value from
 * @param to_gadget the gadget to copy the value to
 */
inline void gadget_copy_value(Gadget &from_gadget, Gadget &to_gadget)
{
   switch(from_gadget.toolbox_class())
    {
        case WritableField::TOOLBOX_CLASS:
          {
             WritableField from = from_gadget;
             WritableField to = to_gadget;
             to.text(from.text());
          }
          break;

        case DisplayField::TOOLBOX_CLASS:
             DisplayField(to_gadget).text(DisplayField(from_gadget).text());
             break;

        case Button::TOOLBOX_CLASS:
             {
                Button from(from_gadget);
                Button to(to_gadget);
                if (from.has_text() && to.has_text())
                {
                    to.value(from.value());
                }
             }
             break;

        case NumberRange::TOOLBOX_CLASS:
            NumberRange(to_gadget).value(NumberRange(from_gadget).value());
            break;

        case OptionButton::TOOLBOX_CLASS:
            OptionButton(to_gadget).on(OptionButton(from_gadget).on());
            break;
         case RadioButton::TOOLBOX_CLASS:
            RadioButton(to_gadget).on(RadioButton(from_gadget).on());
            break;

         case Slider::TOOLBOX_CLASS:
            Slider(to_gadget).value(Slider(from_gadget).value());
            break;

         case StringSet::TOOLBOX_CLASS:
            StringSet(to_gadget).selected(StringSet(from_gadget).selected());
            break;

        default:
           // Ignore what we don't understand
           break;
     }
}

} /* end of tbx namespace */

#endif

