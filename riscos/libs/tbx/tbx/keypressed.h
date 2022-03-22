/*
 * tbx RISC OS toolbox library
 *
 * CopyRIGHT_ (C) 2021 Alan Buckley   All Rights Reserved.
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
 * keypressed.h
 *
 *  Created on: 14-Jan-2021
 *      Author: alanb
 */

#ifndef TBX_KEYPRESSED_H_
#define TBX_KEYPRESSED_H_

#include "kernel.h"

namespace tbx {

/**
 * Internal key numbers for the keypressed function.
 */
enum InternalKey
{
    INTERNAL_KEY_PRINT = -33,
    INTERNAL_KEY_F1 = -114, INTERNAL_KEY_F2 = -115, INTERNAL_KEY_F3 = -116,
    INTERNAL_KEY_F4 = -21,  INTERNAL_KEY_F5 = -117, INTERNAL_KEY_F6 = -118,
    INTERNAL_KEY_F7 = -23,  INTERNAL_KEY_F8 = -119, INTERNAL_KEY_F9 = -120,
    INTERNAL_KEY_F10 = -31, INTERNAL_KEY_F11 = -29, INTERNAL_KEY_F12 = -30,
    INTERNAL_KEY_A = -66,  INTERNAL_KEY_B = -101, INTERNAL_KEY_C = -83,
    INTERNAL_KEY_D = -51,  INTERNAL_KEY_E = -35,  INTERNAL_KEY_F = -68,
    INTERNAL_KEY_G = -84,  INTERNAL_KEY_H = -85,  INTERNAL_KEY_I = -38,
    INTERNAL_KEY_J = -70,  INTERNAL_KEY_K = -71,  INTERNAL_KEY_L = -87,
    INTERNAL_KEY_M = -102, INTERNAL_KEY_N = -86,  INTERNAL_KEY_O = -55,
    INTERNAL_KEY_P = -56,  INTERNAL_KEY_Q = -17,  INTERNAL_KEY_R = -52,
    INTERNAL_KEY_S = -82,  INTERNAL_KEY_T = -36,  INTERNAL_KEY_U = -54,
    INTERNAL_KEY_V = -100, INTERNAL_KEY_W = -34,  INTERNAL_KEY_X = -67,
    INTERNAL_KEY_Y = -69,  INTERNAL_KEY_Z = -98,
    INTERNAL_KEY_0 = -40, INTERNAL_KEY_1 = -49, INTERNAL_KEY_2 = -50,
    INTERNAL_KEY_3 = -18, INTERNAL_KEY_4 = -19, INTERNAL_KEY_5 = -20,
    INTERNAL_KEY_6 = -53, INTERNAL_KEY_7 = -37, INTERNAL_KEY_8 = -22,
    INTERNAL_KEY_NINE = -39,
    INTERNAL_KEY_COMMA = -103, INTERNAL_KEY_EQUALS = -24,
    INTERNAL_KEY_PERIOD = -104,	INTERNAL_KEY_SLASH = -105,
    INTERNAL_KEY_LEFT_BRACKET = -57, INTERNAL_KEY_BACKSLASH = -121,
    INTERNAL_KEY_RIGHT_BRACKET = -89, INTERNAL_KEY_SEMICOLON = -88,
    INTERNAL_KEY_ESC = -113, INTERNAL_KEY_TAB = -97,
    INTERNAL_KEY_CAPSLOCK = -65, INTERNAL_KEY_SCROLLLOCK = -32,
    INTERNAL_KEY_NUMLOCK = -78, INTERNAL_KEY_BREAK = -45,
    INTERNAL_KEY_TILDE = -46, INTERNAL_KEY_BACKSPACE = -48,
    INTERNAL_KEY_INSERT = -62, INTERNAL_KEY_HOME = -63,
    INTERNAL_KEY_PAGEUP = -64, INTERNAL_KEY_PAGEDOWN = -79,
    INTERNAL_KEY_QUOTES = -80,
    INTERNAL_KEY_SHIFT = -1,
    INTERNAL_KEY_LEFT_SHIFT = -4, INTERNAL_KEY_RIGHT_SHIFT= -7,
    INTERNAL_KEY_CTRL = -2,
    INTERNAL_KEY_LEFT_CTRL = -5, INTERNAL_KEY_RIGHT_CTRL = -8,
    INTERNAL_KEY_ALT = -3,
    INTERNAL_KEY_LEFT_ALT = -6, INTERNAL_KEY_RIGHT_ALT = -9,
    INTERNAL_KEY_SPACEBAR = -99, INTERNAL_KEY_DELETE = -90,
    INTERNAL_KEY_RETURN = -74, INTERNAL_KEY_COPY = -106,
    INTERNAL_KEY_UP_ARROW = -58, INTERNAL_KEY_LEFT_ARROW = -26,
    INTERNAL_KEY_RIGHT_ARROW = -122, INTERNAL_KEY_DOWN_ARROW = -42,
    INTERNAL_KEY_KEYPAD_0 = -107, INTERNAL_KEY_KEYPAD_1 = -108,
    INTERNAL_KEY_KEYPAD_2 = -125, INTERNAL_KEY_KEYPAD_3 = -109,
    INTERNAL_KEY_KEYPAD_4 = -123, INTERNAL_KEY_KEYPAD_5 = -124,
    INTERNAL_KEY_KEYPAD_6 = -27, INTERNAL_KEY_KEYPAD_7 = -28,
    INTERNAL_KEY_KEYPAD_8 = -43, INTERNAL_KEY_KEYPAD_9 = -44,
    INTERNAL_KEY_KEYPAD_PLUS = -59, INTERNAL_KEY_KEYPAD_EQUALS = -60,
    INTERNAL_KEY_KEYPAD_PERIOD = -77, INTERNAL_KEY_KEYPAD_DIVIDE = -75,
    INTERNAL_KEY_KEYPAD_HASH = -91,
    INTERNAL_KEY_KEYPAD_MULTIPLY = -92, INTERNAL_KEY_KEYPAD_ENTER = -61,
    INTERNAL_KEY_SELECT_MOUSE_BUTTON = -10,
    INTERNAL_KEY_MENU_MOUSE_BUTTON = -11,
    INTERNAL_KEY_ADJUST_MOUSE_BUTTON = -12
};

/**
* Check if a key is being pressed when this function is called.
* @param key_code Internal key code
* @returns true if key is pressed
*/
inline bool key_pressed(InternalKey key_code)
{
 return (_kernel_osbyte(129, (int)key_code, 0xFF) != 0);
}

}

#endif /* TBX_KEYPRESSED_H_ */