/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2012 Alan Buckley   All Rights Reserved.
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
#ifndef TBX_RES_Window_H
#define TBX_RES_Window_H

#include "resobject.h"
#include "resgadget.h"
#include "resshortcut.h"
#include "resiteratorbase.h"
#include "../handles.h"
#include "../colour.h"

namespace tbx
{
typedef int *OsSpriteAreaPtr;

namespace res
{
/**
 * Constant to put default focus to the window
 */
const ComponentId FOCUS_TO_WINDOW = (ComponentId)-2;

/**
 * Window resource object
 */
class ResWindow : public ResObject
{
   public:
	   enum { CLASS_ID = 0x82880 };

      /**
      * Constructor and empty ResWindow
      * @param name template name of the object
	  * @param toolbar true to set defaults for a toolbar
      */
      ResWindow(std::string name, bool toolbar = false) :
          ResObject(name, CLASS_ID, 102, 36 + 164)
      {
		if (toolbar) flags(16); // is a toolbar flag
		else flags(6); // Default to auto-show/auto-close
		init_message(4, 0); // Help message
		init_string(12, 0); // Pointer shape
		init_string(28, 0); // Menu
	    default_focus(-1);
		show_event(-1);
		hide_event(-1);
		init_string(60, 0); // internal_bl
		init_string(64, 0); // internal_tl
		init_string(68, 0); // external_bl
		init_string(72, 0); // external_tl
		if (toolbar)
		{
			visible_xmin(154);
			visible_ymin(828);
			visible_xmax(554);
			visible_ymax(1228);
		} else
		{
			visible_xmin(32);
			visible_ymin(840);
			visible_xmax(724);
			visible_ymax(1264);
		}
		behind(-1);
		if (toolbar) window_flags(0x80000032);
		else window_flags(0xff000012); // Window flags - TODO: lookup what bits mean
		int_value(108, 0x1070207); // Colours - title and work fore and back
		int_value(112, 0xc0103); // scroll bar for and bakcand title focus
		if (toolbar)
		{
			work_ymin(-1000);
			work_xmax(1000);
		} else
		{
			work_ymin(-1024);
			work_xmax(1280);
		}
		title_flags(0x119); // TODO: Look up what these mean
		int_value(144, 0x640064);  // Min width and height 100x100
		init_string(152, 0); // title_validation
		init_sprite_area_ref(140); // window sprite area
		if (toolbar)
		{
			init_message(148, 0); // title_text
			title_buflen(0); // Title buffer length
		} else
		{
			init_message(148, "Window object"); // title_text
			title_buflen(14); // Title buffer length
		}
		init_object_ref(36); // keyboard short cuts pointer
		init_object_ref(44); // Gadget pointer
      }

      /**
      * Copy constructor
      *
      * @param other object to create copy of
      */
      ResWindow(const ResWindow &other) : ResObject(other) {}

      /**
      * Construct from a ResObject
      *
      * @param other object to create copy of
      */
      ResWindow(const ResObject &other) : ResObject(other) {check_class_id(CLASS_ID);}

      /**
      * Assignment
      *
      * @param other object to create copy of
      */
      ResWindow&operator=(const ResWindow&other)
      {
         ResObject::operator=(other);
         return *this;
      }

      /**
      * Assignment from ResObject
      *
      * @param other object to create copy of
      */
      ResWindow&operator=(const ResObject &other)
      {
         other.check_class_id(CLASS_ID);
		 ResObject::operator =(other);
         return *this;
      }

	  /**
	   * Get all flags as a word raw access.
	   *
	   * Not normally used as there are named methods that allow access
	   * to the individual items in the flags
	   *
	   */
	  unsigned int flags() const {return uint_value(0);}

	  /**
	   * Set all flags as a word
	   *
	   * Not normally used as there are named methods that allow access
	   * to the individual items in the flags
	   */
	  void flags(unsigned int value) {uint_value(0, value);}

	  /**
	   * Check if the about to be shown event will be generated
	   *
	   * @returns true if the about to be shown event will be generated
	   */
	  bool generate_about_to_be_shown() const {return flag(0,1);}
	  /**
	   * Set if the about to be shown event will be generated
	   *
	   * @param value set to true if the about to be shown event should be generated
	   */
	  void generate_about_to_be_shown(bool value) {flag(0,1, value);}

	  /**
	   * Check if window is automatically shown.
	   *
	   * Automatically open this Window when a WIMP OpenWindowRequest is received
	   * (when set the client will not see the underlying WIMP requests)
	   *
	   * @returns true if window is automatically shown
	   */
	  bool auto_show() const {return flag(0,2);}
	  /**
	   * Set if window is automatically shown.
	   *
	   * Automatically open this Window when a WIMP OpenWindowRequest is received
	   * (when set the client will not see the underlying WIMP requests)
	   *
	   * @param value set to true if window is automatically shown
	   */
	  void auto_show(bool value) {flag(0,2, value);}

	  /**
	   * Check if window is automatically closed.
	   *
	   * Automatically close this Window when a WIMP CloseWindowRequest is received
	   * (when set the client will not see the underlying WIMP requests)
	   *
	   * @returns true if window will be automatically closed
	   */
	  bool auto_close() const {return flag(0,4);}
	  /**
	   * Set if window is automatically closed.
	   *
	   * Automatically close this Window when a WIMP CloseWindowRequest is received
	   * (when set the client will not see the underlying WIMP requests)
	   *
	   * @param value set to true if window should be automatically closed
	   */
	  void auto_close(bool value) {flag(0,4, value);}

	  /**
	   * Check if an event will be generated when the window has been hidden.
	   *
	   * @returns true if the event will be generated
	   */
	  bool generate_has_been_hidden() const {return flag(0,8);}
	  /**
	   * Set if an event will be generated when the window has been hidden.
	   *
	   * @param value set to true if the event should be generated
	   */
	  void generate_has_been_hidden(bool value) {flag(0,8, value);}

	  /**
	   * Check if this template is for a toolbar
	   *
	   * @returns true if the template is for a toolbar
	   */
	  bool toolbar() const {return flag(0,16);}
	  /**
	   * Set if this template is for a toolbar
	   *
	   * @param value set to true if the template is for a toolbar
	   */
	  void toolbar(bool value) {flag(0,16, value);}

   	   /**
	    * Get the help message
	    *
	    * @returns pointer to zero terminated help message or 0 if none.
		*/
	   const char *help_message() const {return message(4);}

	   /**
	    * Set the help message
		*
		* @param value pointer to zero terminated help message or 0 if none.
		* @param max_length maximum length for help message or -1 to use current value.
		*                   This is alway adjusted to allow for the full length of the help message.
		*/
	   void help_message(const char *value, int max_length = -1) {message_with_length(4, value, max_length);}
	   /**
	    * Set the help message
		*
		* @param value help message.
		* @param max_length maximum length for help message or -1 to use current value.
		*                   This is alway adjusted to allow for the full length of the help message.
		*/
	   void help_message(const std::string &value, int max_length = -1) {message_with_length(4, value, max_length);}

	   /**
	    * Get the maximum help message length
		*/
	   int max_help_message() const {return int_value(8);}

	   /**
	    * Get the name of the sprite to use for the mouse pointer
	    * when it is over this window.
	    *
	    * @returns pointer to zero terminated sprite name or 0 if none
	    */
	   const char *pointer_shape() const {return string(12);}
	   /**
	    * Set the name of the sprite to use for the mouse pointer
	    * when it is over this window.
	    *
	    * @param value pointer to zero terminated sprite name or 0 if none
	    * @param max_length maximum length of sprite name the pointer shape
	    * will be set to or -1 to use the length of value.
	    */
	   void pointer_shape(const char *value, int max_length = -1) {string_with_length(12, value, max_length);}
	   /**
	    * Set the name of the sprite to use for the mouse pointer
	    * when it is over this window.
	    *
	    * @param value sprite name
	    * @param max_length maximum length of sprite name the pointer shape
	    * will be set to or -1 to use the length of value.
	    */
	   void pointer_shape(const std::string &value, int max_length = -1) {string_with_length(12, value, max_length);}

   	   /**
	    * Get the maximum pointer shape length
		*/
	   int max_pointer_shape() const {return int_value(16);}
	   /**
	    * Get the X position of the hotspot in the pointer
	    *
	    * @returns X position of hotspot
	    */
	   int pointer_x_hot() const {return int_value(20);}
	   /**
	    * Set the X position of the hotspot in the pointer
	    *
	    * @param value X position of hotspot
	    */
	   void pointer_x_hot(int value) {int_value(20, value);}
	   /**
	    * Get the Y position of the hotspot in the pointer
	    *
	    * @returns Y position of hotspot
	    */
	   int pointer_y_hot() const {return int_value(24);}
	   /**
	    * Set the Y position of the hotspot in the pointer
	    *
	    * @param value Y position of hotspot
	    */
	   void pointer_y_hot(int value) {int_value(24, value);}

	   /**
	    * Get the menu to be shown for this menu
	    *
	    * @returns pointer to zero terminated menu object name or 0 for none
	    */
	   const char *menu() const {return string(28);}
	   /**
	    * Set the menu to be shown for this menu
	    *
	    * @param value pointer to zero terminated menu object name or 0 for none
	    */
	   void menu(const char *value) {string(28, value);}
	   /**
	    * Set the menu to be shown for this menu
	    *
	    * @param value menu object name
	    */
	   void menu(const std::string &value) {string(28, value);}
	   /**
	    * Get the number of shortcut keys in this window
	    *
	    * @returns the number of shortcut keys
	    */
	   int num_shortcuts() const {return int_value(32);}
	   /**
	    * Get the number of gadgets in this window
	    *
	    * @returns the number of gadgets
	    */
	   int num_gadgets() const {return int_value(40);}

	   /**
	    * Get the component ID of the gadget to get the default focus.
	    *
	    * @returns the component ID, FOCUS_TO_WINDOW if focus is on
	    *          the window or NULL_ComponentID for none
		*/
	   ComponentId default_focus() const {return int_value(48);}
	   /**
	    * Set the component ID of the gadget to get the default focus.
	    *
	    * @param id the component ID, FOCUS_TO_WINDOW if focus should be
	                on the window or NULL_ComponentID for none
		*/
	   void default_focus(ComponentId id) {int_value(48, id);}

	   /**
	    * Get the event ID of the event generated when the window is shown.
		*
		* generate_about_to_be_shown must be set for this event to be generated
		*
		* @returns event ID or -1 for default.
		*/
	   int show_event() const {return int_value(52);}
	   /**
	    * Set the event ID of the event generated when the window is shown.
		*
		* generate_about_to_be_shown must be set for this event to be generated
		*
		* @param event_id event ID or -1 for default.
		*/
	   void show_event(int event_id) {int_value(52, event_id);}

	   /**
	    * Get the event ID of the event generated when the window has been hidden.
		*
		* generate_has_been_hidden must be set for this event to be generated
		*
		* @returns event ID or -1 for default.
		*/
   	   int hide_event() const {return int_value(56);}
	   /**
	    * Set the event ID of the event generated when the window has been hidden.
		*
		* generate_has_been_hidden must be set for this event to be generated
		*
		* @param value event ID or -1 for default.
		*/
	   void hide_event(int value) {int_value(56, value);}

	   /**
	    * Get the name of the object to use for the internal bottom left toolbar.
	    *
	    * @returns pointer to zero terminated toolbar object name or 0 if none.
	    */
	   const char *internal_bl_toolbar() const {return string(60);}
	   /**
	    * Set the name of the object to use for the internal bottom left toolbar.
	    *
	    * @param value pointer to zero terminated toolbar object name or 0 if none.
	    */
	   void internal_bl_toolbar(const char *value) {string(60, value);}
	   /**
	    * Set the name of the object to use for the internal bottom left toolbar.
	    *
	    * @param value toolbar object name.
	    */
	   void internal_bl_toolbar(const std::string &value) {string(60, value);}
	   /**
	    * Get the name of the object to use for the internal top left toolbar.
	    *
	    * @returns pointer to zero terminated toolbar object name or 0 if none.
	    */
   	   const char *internal_tl_toolbar() const {return string(64);}
	   /**
	    * Set the name of the object to use for the internal top left toolbar.
	    *
	    * @param value pointer to zero terminated toolbar object name or 0 if none.
	    */
	   void internal_tl_toolbar(const char *value) {string(64, value);}
	   /**
	    * Set the name of the object to use for the internal top left toolbar.
	    *
	    * @param value toolbar object name.
	    */
	   void internal_tl_toolbar(const std::string &value) {string(64, value);}

	   /**
	    * Get the name of the object to use for the external bottom left toolbar.
	    *
	    * @returns pointer to zero terminated toolbar object name or 0 if none.
	    */
	   const char *external_bl_toolbar() const {return string(68);}
	   /**
	    * Set the name of the object to use for the external bottom left toolbar.
	    *
	    * @param value pointer to zero terminated toolbar object name or 0 if none.
	    */
	   void external_bl_toolbar(const char *value) {string(68, value);}
	   /**
	    * Set the name of the object to use for the external bottom left toolbar.
	    *
	    * @param value toolbar object name.
	    */
	   void external_bl_toolbar(const std::string &value) {string(68, value);}

	   /**
	    * Get the name of the object to use for the external top left toolbar.
	    *
	    * @returns pointer to zero terminated toolbar object name or 0 if none.
	    */
	   const char *external_tl_toolbar() const {return string(72);}
	   /**
	    * Set the name of the object to use for the external top left toolbar.
	    *
	    * @param value pointer to zero terminated toolbar object name or 0 if none.
	    */
	   void external_tl_toolbar(const char *value) {string(72, value);}
	   /**
	    * Set the name of the object to use for the external top left toolbar.
	    *
	    * @param value toolbar object name.
	    */
	   void external_tl_toolbar(const std::string &value) {string(72, value);}

// window  88  WimpWindow
	   /**
	    * Get the minimum X coordinate of the visible part of the window
	    *
	    * @returns X coordinate in OS units
	    */
	   int visible_xmin() const {return int_value(76);}
	   /**
	    * Set the minimum X coordinate of the visible part of the window
	    *
	    * @param value X coordinate in OS units
	    */
	   void visible_xmin(int value) {int_value(76, value);}
	   /**
	    * Get the minimum Y coordinate of the visible part of the window
	    *
	    * @returns Y coordinate in OS units
	    */
	   int visible_ymin() const {return int_value(80);}
	   /**
	    * Set the minimum Y coordinate of the visible part of the window
	    *
	    * @param value Y coordinate in OS units
	    */
	   void visible_ymin(int value) {int_value(80, value);}
	   /**
	    * Get the maximum X coordinate of the visible part of the window
	    *
	    * @returns X coordinate in OS units
	    */
	   int visible_xmax() const {return int_value(84);}
	   /**
	    * Set the maximum X coordinate of the visible part of the window
	    *
	    * @param value X coordinate in OS units
	    */
	   void visible_xmax(int value) {int_value(84, value);}
	   /**
	    * Get the maximum Y coordinate of the visible part of the window
	    *
	    * @returns Y coordinate in OS units
	    */
	   int visible_ymax() const {return int_value(88);}
	   /**
	    * Set the maximum Y coordinate of the visible part of the window
	    *
	    * @param value Y coordinate in OS units
	    */
	   void visible_ymax(int value) {int_value(88, value);}
	   /**
	    * Get the X scroll offset into the work area
	    *
	    * @returns X scroll offset in OS units
	    */
	   int scroll_x() const {return int_value(92);}
	   /**
	    * Set the X scroll offset into the work area
	    *
	    * @param value X scroll offset in OS units
	    */
	   void scroll_x(int value) {int_value(92, value);}
	   /**
	    * Get the Y scroll offset into the work area
	    *
	    * @returns Y scroll offset in OS units
	    */
	   int scroll_y() const {return int_value(96);}
	   /**
	    * Set the Y scroll offset into the work area
	    *
	    * @param value Y scroll offset in OS units
	    */
	   void scroll_y(int value) {int_value(96, value);}
	   /**
	    * Get the WIMP window handle this window will be opened behind
	    *
	    * @returns WIMP window handle or -1 for on top
	    */
	   int behind() const {return int_value(100);}
	   /**
	    * Set the WIMP window handle this window will be opened behind
	    *
	    * @param value WIMP window handle or -1 for on top
	    */
	   void behind(int value) {int_value(100, value);}
	   /**
	    * Get the window flags
	    *
	    * This gets all the window flags as one number, use the methods below
	    * for a more readable way to interrogate individual window flags
	    *
	    * @returns the window flags
	    */
	   unsigned int window_flags() const {return uint_value(104);}
	   /**
	    * Set the window flags
	    *
	    * This sets all the window flags as one number, use the methods
	    * below for a more readble way to set the individual window flags.
	    *
	    * @param value the new window flags
	    */
	   void window_flags(unsigned int value) {uint_value(104, value);}

       /**
        * Check if the window will be moveable
        *
        * @returns true if the window is moveable
        */
       bool moveable() const {return flag(104,2);}
       /**
        * Set if the window is moveable
        *
        * @param move true to make window moveable
        */
       void moveable(bool move) {flag(104,2,move);}
       /**
        * Check if window can be redrawn entirely by the WIMP.
        *
        * i.e there are no user graphics in the work area
        *
        * @returns true if WIMP can redraw whole Window
        */
       bool auto_redraw() const {return flag(104,16);}
       /**
        * Set if window can be redrawn entirely by the WIMP.
        *
        * i.e there are no user graphics in the work area
        *
        * @param on true if WIMP can redraw whole Window
        */
       void auto_redraw(bool on) {flag(104,16,on);}
       /**
        * Check if a window is a pane
        *
        * @returns true if the window is a pane
        */
       bool pane() const {return flag(104,32);}
       /**
        * Set if a window is a pane
        *
        * @param p true if the window is a pane
        */
       void pane(bool p) {flag(104,32,p);}
       /**
         * Check if window is allowed off screen
         *
         * @returns true if window is allowed off screen
         */
       bool allow_off_screen() const {return flag(104,64);}
       /**
        * Set if window is allowed off screen
        *
        * @param allow true if window is allowed off screen
        */
       void allow_off_screen(bool allow) {flag(104,64, allow);}
       /**
        * Check if a Scroll_Request event if returned with auto-repeat
        * on the arrow icons and no auto-repeat on the outer scroll
        * bar region.
        *
        * @returns if user scroll is on
        */
       bool user_scroll() const {return flag(104,256);}
       /**
        * Set if a Scroll_Request event if returned with auto-repeat
        * on the arrow icons and no auto-repeat on the outer scroll
        * bar region.
        *
        * @param scroll true to turn user scroll on
        */
       void user_scroll(bool scroll) {flag(104,256,scroll);}
       /**
        * Check if a Scroll_Request event if returned with no auto-repeat
        * on the arrow icons and no auto-repeat on the outer scroll
        * bar region.
        *
        * @returns if user scroll is on
        */
       bool user_scroll_debounced() const {return flag(104,512);}
       /**
        * Set if a Scroll_Request event if returned with no auto-repeat
        * on the arrow icons and no auto-repeat on the outer scroll
        * bar region.
        *
        * @param scroll true to turn user scroll on
        */
       void user_scroll_debounced(bool scroll) {flag(104,512,scroll);}
       /**
        * Check if window colours are given as gcol numbers
        *
        * @returns true if window colours are given as gcol numbers
        */
        bool real_colours() const {return flag(104,1024);}
        /**
         * Set if window colours are given as gcol numbers
         *
         * @param real true if window colours are given as gcol numbers
         */
        void real_colours(bool real) {flag(104,1024, real);}
        /**
         * Check if window can be opened below this
         *
         * @returns true if this is the top window
         */
         bool top_window() const {return flag(104,2048);}
        /**
         * Set if window can be opened below this
         *
         * @param tw true if this is the top window
         */
        void top_window(bool tw) {flag(104,2048,tw);}
        /**
         * Check if generates events for hot keys
         *
         * @returns true if events are generated for hot keys
         */
         bool hot_keys() const {return flag(104,4096);}
        /**
         * Set if generates events for hot keys
         *
         * @param generate true if events are generated for hot keys
         */
        void hot_keys(bool generate) {flag(104,4096,generate);}
        /**
         * Check if window is forced on screen
         *
         * @returns true if window if forced on screen
         */
         bool force_on_screen() const {return flag(104,8192);}
        /**
         * Set if window is forced on screen
         *
         * @param on true if window if forced on screen
         */
      void force_on_screen(bool on) {flag(104,8192,on);}
      /**
       * Check if right extent is ignored
       *
       * @returns true if right extent is ignored
       */
      bool ignore_right_extent() const {return flag(104,16384);}
      /**
       * Set if right extent is ignored
       *
       * @param ignore true if right extent is ignored
       */
      void ignore_right_extent(bool ignore) {flag(104,16384,ignore);}
      /**
       * Check if lower extent is ignored
       *
       * @returns true if lower extent is ignored
       */
      bool ignore_lower_extent() const {return flag(104,32768);}
      /**
       * Set if lower extent is ignored
       *
       * @param ignore true if lower extent is ignored
       */
      void ignore_lower_extent(bool ignore) {flag(104,32768,ignore);}
       /**
        * Check if window has a back icon
        *
        * @returns true if window has a back icon
        */
       bool back_icon() const {return flag(104,(1<<24));}
       /**
        * Set if window has a back icon
        *
        * @param has_icon true if window has a back icon
        */
       void back_icon(bool has_icon) {flag(104,(1<<24), has_icon);}
       /**
        * Check if window has a close icon
        *
        * @returns true if window has a close icon
        */
       bool close_icon() const {return flag(104,(1<<25));}
       /**
        * Set if window has a close icon
        *
        * @param has_icon true if window has a close icon
        */
       void close_icon(bool has_icon) {flag(104,(1<<25), has_icon);}
       /**
        * Check if window has a title bar
        *
        * @returns true if window has a title bar
        */
       bool titlebar() const {return flag(104,(1<<26));}
       /**
        * Set if window has a title bar
        *
        * @param has_tb true if window has a title bar
        */
       void titlebar(bool has_tb) {flag(104,(1<<26), has_tb);}
       /**
        * Check if window has a toggle size icon
        *
        * @returns true if window has a toggle size icon
        */
       bool toggle_size_icon() const {return flag(104,(1<<27));}
       /**
        * Set if window has a toggle size icon
        *
        * @param has_icon true if window has a toggle size icon
        */
       void toggle_size_icon(bool has_icon) {flag(104,(1<<27), has_icon);}
       /**
        * Check if window has a vertical scroll bar
        *
        * @returns true if window has a vertical scroll bar
        */
       bool vscrollbar() const {return flag(104,(1<<28));}
       /**
        * Set if window has a vertical scroll bar
        *
        * @param has_vsb true if window has a vertical scroll bar
        */
       void vscrollbar(bool has_vsb) {flag(104,(1<<28), has_vsb);}
       /**
        * Check if window has an adjust size icon
        *
        * @returns true if window has an adjust size icon
        */
       bool adjust_size_icon() const {return flag(104,(1<<29));}
       /**
        * Set if window has an adjust size icon
        *
        * @param has_icon true if window has an adjust size icon
        */
       void adjust_size_icon(bool has_icon) {flag(104,(1<<29), has_icon);}
       /**
        * Check if window has a horizontal scroll bar
        *
        * @returns true if window has a horizontal scroll bar
        */
       bool hscrollbar() const {return flag(104,(1<<30));}
       /**
        * Set if window has a horizontal scroll bar
        *
        * @param has_hsb true if window has a horizontal scroll bar
        */
       void hscrollbar(bool has_hsb) {flag(104,(1<<30), has_hsb);}

	   /**
	    * Get the foreground/text colour for the title bar
	    *
	    * @returns WIMP colour
	    */
	   WimpColour title_fore() const {return byte_value(108);}
	   /**
	    * Set the foreground/text colour for the title bar
	    *
	    * @param value WIMP colour
	    */
	   void title_fore(WimpColour value) {byte_value(108, value);}
	   /**
	    * Get the background colour for the title bar
	    *
	    * @returns WIMP colour
	    */
	   WimpColour title_back() const {return byte_value(109);}
	   /**
	    * Set the background colour for the title bar
	    *
	    * @param value WIMP colour
	    */
	   void title_back(WimpColour value) {byte_value(109, value);}
	   /**
	    * Get the work area foreground colour
	    *
	    * @returns WIMP colour
	    */
	   WimpColour  work_fore() const {return byte_value(110);}
	   /**
	    * Set the work area foreground colour
	    *
	    * @param value WIMP colour
	    */
	   void work_fore(WimpColour value) {byte_value(110, value);}
	   /**
	    * Get the work area background colour
	    *
	    * @returns WIMP colour
	    */
	   WimpColour work_back() const {return byte_value(111);}
	   /**
	    * Set the work area background colour
	    *
	    * @param value WIMP colour
	    */
	   void work_back(WimpColour value) {byte_value(111, value);}
	   /**
	    * Get the work area scroll bar outer colour
	    *
	    * @returns WIMP colour
	    */
	   WimpColour scroll_outer() const {return byte_value(112);}
	   /**
	    * Set the scroll bar outer colour
	    *
	    * @param value WIMP colour
	    */
	   void scroll_outer(WimpColour value) {byte_value(112, value);}
	   /**
	    * Get the scroll bar inner colour
	    *
	    * @returns WIMP colour
	    */
	   WimpColour scroll_inner() const {return byte_value(113);}
	   /**
	    * Set the scroll bar inner colour
	    *
	    * @param value WIMP colour
	    */
	   void scroll_inner(WimpColour value) {byte_value(113, value);}
	   /**
	    * Get the background colour for the title bar when it has
	    * the input focus.
	    *
	    * @returns WIMP colour
	    */
	   WimpColour title_inputfocus() const {return byte_value(114);}
	   /**
	    * Set the background colour for the title bar when it has
	    * the input focus.
	    *
	    * @param value WIMP colour
	    */
	   void title_inputfocus(WimpColour value) {byte_value(114, value);}
	   // Note: 115 is a filler byte and must be 0
	   /**
	    * Get the minimum X coordinate of the work area extent
	    *
	    * @returns X coordinate in OS units
	    */
   	   int work_xmin() const {return int_value(116);}
	   /**
	    * Set the minimum X coordinate of the work area extent
	    *
	    * @param value X coordinate in OS units
	    */
	   void work_xmin(int value) {int_value(116, value);}
	   /**
	    * Get the minimum Y coordinate of the work area extent
	    *
	    * @returns Y coordinate in OS units
	    */
	   int work_ymin() const {return int_value(120);}
	   /**
	    * Set the minimum Y coordinate of the work area extent
	    *
	    * @param value Y coordinate in OS units
	    */
	   void work_ymin(int value) {int_value(120, value);}
	   /**
	    * Get the maximum X coordinate of the work area extent
	    *
	    * @returns X coordinate in OS units
	    */
	   int work_xmax() const {return int_value(124);}
	   /**
	    * Set the maximum X coordinate of the work area extent
	    *
	    * @param value X coordinate in OS units
	    */
	   void work_xmax(int value) {int_value(124, value);}
	   /**
	    * Get the maximum Y coordinate of the work area extent
	    *
	    * @returns Y coordinate in OS units
	    */
	   int work_ymax() const {return int_value(128);}
	   /**
	    * Set the maximum Y coordinate of the work area extent
	    *
	    * @param value Y coordinate in OS units
	    */
	   void work_ymax(int value) {int_value(128, value);}
	   /**
	    * Get the title flags
	    *
	    * @returns title flags 32 bit value
	    */
	   unsigned int title_flags() const {return uint_value(132);}
	   /**
	    * Set the title flags
	    *
	    * @param value title flags 32 bit value
	    */
	   void title_flags(unsigned int value) {uint_value(132, value);}
	   //TODO: setters/getters for bits of title flags
	   /**
	    * Get the button type
	    *
	    * @return button type
	    */
	   unsigned int button_type() const {return uint_value(136);}
	   /**
	    * Set the button type
	    *
	    * @param value button type
	    */
	   void button_type(unsigned int value) {uint_value(136, value);}
	   //TODO: enum for button_type
	   /**
	    * Get a pointer to the sprite area used for client icons on
	    * this window.
	    *
	    * @returns OS Sprite area pointer
	    */
	   OsSpriteAreaPtr sprite_area() const {return (OsSpriteAreaPtr)uint_value(140);}
	   /**
	    * Set a pointer to the sprite area used for client icons on
	    * this window.
	    *
	    * @param value OS Sprite area pointer
	    */
	   void sprite_area(OsSpriteAreaPtr value) {uint_value(140, (unsigned int)value);}
	   /**
	    * Get the minimum width for the window
	    *
	    * @returns minimum width in OS units
	    */
	   unsigned short min_width() const {return ushort_value(144);}
	   /**
	    * Set the minimum width for the window
	    *
	    * @param value minimum width in OS units
	    */
	   void min_width(unsigned short value) {ushort_value(144, value);}
	   /**
	    * Get the minimum height for the window
	    *
	    * @returns minimum height in OS units
	    */
	   unsigned short min_height() const {return ushort_value(146);}
	   /**
	    * Set the minimum height for the window
	    *
	    * @param value minimum height in OS units
	    */
	   void min_height(unsigned short value) {ushort_value(146, value);}

	   /**
	    * Get the text for the title of the window
	    *
	    * @returns pointer to zero terminated title text
	    */
	   const char *title_text() const {return message(148);}
	   /**
	    * Set the text for the title of the window
	    *
	    * @param value pointer to zero terminated title text
	    */
	   void title_text(const char *value) {message(148, value);}
	   /**
	    * Set the text for the title of the window
	    *
	    * @param value title text
	    */
	   void title_text(const std::string &value) {message(148, value);}
	   /**
	    * Get the title validation
	    *
	    * @returns pointer to zero terminated title validation string or -1 if none.
	    */
	   const char *title_validation() const {return string(152);}
	   /**
	    * Set the title validation
	    *
	    * @param value pointer to zero terminated title validation string or -1 if none.
	    */
	   void title_validation(const char *value) {string(152, value);}
	   /**
	    * Set the title validation
	    *
	    * @param value title validation string
	    */
	   void title_validation(const std::string &value) {string(152, value);}
	   /**
	    * Get the length of the buffer to hold the title
	    *
	    * @returns title buffer length
	    */
	   int title_buflen() const {return int_value(156);}
	   /**
	    * Set the length of the buffer to hold the title
	    *
	    * @param value title buffer length
	    */
	   void title_buflen(int value) {int_value(156, value);}

	   // num_icons  at 160 (must be zero)

	   /**
	    * Constant iterator for shortcuts
	    */
	   class const_shortcut_iterator : public ResIteratorBase<ResWindow>
	   {
		   const_shortcut_iterator(const ResWindow *window, int offset) : ResIteratorBase<ResWindow>(window, offset) {}
		   friend class ResWindow;
	   public:
		   /**
		    * Increment to the next shortcut
		    *
		    * @returns incremented iterator
		    */
		   const_shortcut_iterator &operator++() {_offset += SHORTCUT_SIZE; return *this;}
		   /**
		    * Increment to the next shortcut
		    *
		    * @returns iterator before increment
		    */
		   const_shortcut_iterator operator++(int) {const_shortcut_iterator tmp(*this); _offset += SHORTCUT_SIZE; return tmp;}
		   /**
		    * Get the shortcut pointed to by this iterator
		    *
		    * @returns ResShortcut pointed to
		    */
		   ResShortcut operator*() const {return _object->shortcut_at_offset(_offset);}
	   };
	   friend class const_shortcut_iterator;

	   /**
	    * Get constant iterator to first shortcut
	    *
	    * @returns constant shortcut iterator
	    */
	   const_shortcut_iterator shortcut_begin() const {return const_shortcut_iterator(this, first_shortcut_offset());}
	   /**
	    * Get constant iterator to end of shortcuts
	    *
	    * @returns constant shortcut iterator to position after the last shortcut
	    */
	   const_shortcut_iterator shortcut_end() const {return const_shortcut_iterator(this, end_shortcut_offset());}
	   /**
	    * Get constant iterator to first shortcut
	    *
	    * @returns constant shortcut iterator
	    */
	   const_shortcut_iterator shortcut_cbegin() const {return const_shortcut_iterator(this, first_shortcut_offset());}
	   /**
	    * Get constant iterator to end of shortcuts
	    *
	    * @returns constant shortcut iterator to position after the last shortcut
	    */
	   const_shortcut_iterator shortcut_cend() const {return const_shortcut_iterator(this, end_shortcut_offset());}

	   const_shortcut_iterator find_shortcut(int key_code) const;

	   /**
	    * Iterator for shortcuts
	    */
   	   class shortcut_iterator : public ResIteratorBase<ResWindow>
	   {
		   shortcut_iterator(const ResWindow *window, int offset) : ResIteratorBase<ResWindow>(window, offset) {}
		   friend class ResWindow;
	   public:
		   /**
		    * Increment to the next shortcut
		    *
		    * @returns incremented iterator
		    */
		   shortcut_iterator &operator++() {_offset += SHORTCUT_SIZE; return *this;}
		   /**
		    * Increment to the next shortcut
		    *
		    * @returns iterator before increment
		    */
		   shortcut_iterator operator++(int) {shortcut_iterator tmp(*this); _offset += SHORTCUT_SIZE; return tmp;}
		   /**
		    * Get the shortcut pointed to by this iterator
		    *
		    * @returns ResShortcut pointed to
		    */
		   ResShortcut operator*() const {return _object->shortcut_at_offset(_offset);}
	   };
	   friend class shortcut_iterator;

	   /**
	    * Get iterator to first shortcut
	    *
	    * @returns shortcut iterator
	    */
	   shortcut_iterator shortcut_begin() {return shortcut_iterator(this, first_shortcut_offset());}
	   /**
	    * Get iterator to end of shortcuts
	    *
	    * @returns shortcut iterator to position after the last shortcut
	    */
	   shortcut_iterator shortcut_end() {return shortcut_iterator(this, end_shortcut_offset());}
	   //TODO: Document use of SpecialKeys from KeyListener
	   shortcut_iterator find_shortcut(int key_code);

	   ResShortcut shortcut(int key_code) const;
	   bool contains_shortcut(int key_code) const;
	   void add_shortcut(const ResShortcut &shortcut);
	   void replace_shortcut(const ResShortcut &shortcut);
	   void erase_shortcut(int key_code);

	   shortcut_iterator insert_shortcut(shortcut_iterator pos, const ResShortcut &shortcut);
	   shortcut_iterator replace_shortcut(shortcut_iterator pos, const ResShortcut &shortcut);
	   shortcut_iterator erase_shortcut(shortcut_iterator pos);


	   /**
	    * Constant iterator for gadgets in the window resource
	    */
	   class const_gadget_iterator : public ResIteratorBase<ResWindow>
	   {
		   const_gadget_iterator(const ResWindow *window, int offset) : ResIteratorBase<ResWindow>(window, offset) {}
		   friend class ResWindow;
	   public:
		   /**
		    * Increment to the next gadget
		    *
		    * @returns incremented iterator
		    */
		   const_gadget_iterator &operator++() {_object->next_gadget(_offset); return *this;}
		   /**
		    * Increment to the next gadget
		    *
		    * @returns iterator before increment
		    */
		   const_gadget_iterator operator++(int) {const_gadget_iterator tmp(*this); _object->next_gadget(_offset); return tmp;}
		   /**
		    * Get the gadget pointed to by this iterator
		    *
		    * @returns ResGadget pointed to
		    */
		   ResGadget operator*() const {return _object->gadget_at_offset(_offset);}
	   };
	   friend class const_gadget_iterator;

	   /**
	    * Get constant iterator to first gadget
	    *
	    * @returns constant iterator for first gadget
	    */
	   const_gadget_iterator gadget_begin() const {return const_gadget_iterator(this, first_gadget_offset());}
	   /**
	    * Get constant iterator to the end of the gadgets
	    *
	    * @returns constant iterator to the position after the last gadget
	    */
	   const_gadget_iterator gadget_end() const {return const_gadget_iterator(this, end_gadget_offset());}
	   /**
	    * Get constant iterator to first gadget
	    *
	    * @returns constant iterator for first gadget
	    */
	   const_gadget_iterator gadget_cbegin() const {return const_gadget_iterator(this, first_gadget_offset());}
	   /**
	    * Get constant iterator to the end of the gadgets
	    *
	    * @returns constant iterator to the position after the last gadget
	    */
	   const_gadget_iterator gadget_cend() const {return const_gadget_iterator(this, end_gadget_offset());}
	   const_gadget_iterator find_gadget(int component_id) const;

	   /**
	    * Iterator for gadgets in the window resource
	    */
   	   class gadget_iterator : public ResIteratorBase<ResWindow>
	   {
		   gadget_iterator(const ResWindow *window, int offset) : ResIteratorBase<ResWindow>(window, offset) {}
		   friend class ResWindow;
	   public:
		   /**
		    * Increment to the next gadget
		    *
		    * @returns incremented iterator
		    */
		   gadget_iterator &operator++() {_object->next_gadget(_offset); return *this;}
		   /**
		    * Increment to the next gadget
		    *
		    * @returns iterator before increment
		    */
		   gadget_iterator operator++(int) {gadget_iterator tmp(*this); _object->next_gadget(_offset); return tmp;}
		   /**
		    * Get the gadget pointed to by this iterator
		    *
		    * @returns ResGadget pointed to
		    */
		   ResGadget operator*() const {return _object->gadget_at_offset(_offset);}
	   };
	   friend class gadget_iterator;

	   /**
	    * Get iterator to first gadget
	    *
	    * @returns iterator for first gadget
	    */
	   gadget_iterator gadget_begin() {return gadget_iterator(this, first_gadget_offset());}
	   /**
	    * Get iterator to the end of the gadgets
	    *
	    * @returns iterator to the position after the last gadget
	    */
	   gadget_iterator gadget_end() {return gadget_iterator(this, end_gadget_offset());}
	   gadget_iterator find_gadget(int component_id);

	   ResGadget gadget(ComponentId component_id) const;
	   bool contains_gadget(ComponentId component_id) const;
	   void add_gadget(const ResGadget &gadget);
	   void replace_gadget(const ResGadget &gadget);
	   void erase_gadget(ComponentId component_id);

	   gadget_iterator insert_gadget(gadget_iterator pos, const ResGadget &gadget);
	   gadget_iterator replace_gadget(gadget_iterator pos, const ResGadget &gadget);
	   gadget_iterator erase_gadget(gadget_iterator pos);

	protected:
	   /**
	    * Get pointer to position of first gadget in memory
	    *
	    * @returns pointer to first gadget in memory or 0 if none
	    */
		char *gadget_start() const {return (char *)int_value(44);}
		int first_gadget_offset() const;
		int end_gadget_offset() const;
	    ResGadget gadget_at_offset(int item_offset) const;
		void next_gadget(int &item_offset) const;

		/**
		 * Get pointer to first short cut in memory
		 *
		 * @return pointer to first shortcut in memory or 0 if none
		 */
		char *shortcut_start() const {return (char *)int_value(36);}
		int first_shortcut_offset() const;
		int end_shortcut_offset() const;
	    ResShortcut shortcut_at_offset(int item_offset) const;

};

}

}
#endif // TBX_RES_Window_H
