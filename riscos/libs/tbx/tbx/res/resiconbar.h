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

#ifndef TBX_RES_IconBar_H
#define TBX_RES_IconBar_H

#include "resobject.h"

namespace tbx
{
namespace res
{

/**
 * Class for Iconbar in memory template
 */
class ResIconbar : public ResObject
{
   public:
      enum { CLASS_ID = 0x82900};

      /**
      * Constructor and empty ResIconBar
      * @param name template name of the object
      */
      ResIconbar(std::string name) :
          ResObject(name, CLASS_ID, 100, 36 + 56)
      {
		  flags(0x60); // generate click events
		  position(-1);
		  init_string(12, 0); // Null sprite name
		  init_message(20, 0); // Null text
		  init_string(28, 0); // Null menu
		  init_string(40, 0); // Null select show
		  init_string(44, 0); // Null adjust show
		  init_message(48, 0); // Null help text
      }

      /**
      * Copy constructor
      *
      * @param other object to create copy of
      */
      ResIconbar(const ResIconbar &other) : ResObject(other) {}

      /**
      * Construct from a ResObject
      *
      * @param other object to create copy of
      */
      ResIconbar(const ResObject &other) : ResObject(other) {check_class_id(CLASS_ID);}

      /**
      * Assignment
      *
      * @param other object to create copy of
      */
      ResIconbar&operator=(const ResIconbar&other)
      {
         ResObject::operator=(other);
         return *this;
      }

      /**
      * Assignment from ResObject
      *
      * @param other object to create copy of
      */
      ResIconbar&operator=(const ResObject &other)
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
	   *
	   * @param value new value for the flags
	   */
	  void flags(unsigned int value) {uint_value(0, value);}
	  
	  /**
	   * Check if an iconbar select about to be shown event will be generated
	   * before the object* which has been associated with a select click is
	   * shown.
	   *
	   * @returns true if the event will be generated
	   */
	  bool generate_select_about_to_be_shown() const {return flag(0, 1);}
	  /**
	   * Set if an iconbar select about to be shown event will be generated
	   * before the object* which has been associated with a select click is
	   * shown.
	   *
	   * @param generate true to generate the event
	   */
	  void generate_select_about_to_be_shown(bool generate) {flag(0, 1, generate);}
	  /**
	   * Check if an iconbar adjust about to be shown event will be generated
	   * before the object* which has been associated with a adjust click is
	   * shown.
	   *
	   * @returns true if the event will be generated
	   */
	  bool generate_adjust_about_to_be_shown() const {return flag(0, 2);}
	  /**
	   * Set if an iconbar adjust about to be shown event will be generated
	   * before the object* which has been associated with a adjust click is
	   * shown.
	   *
	   * @param generate true to generate the event
	   */
	  void generate_adjust_about_to_be_shown(bool generate) {flag(0, 2, generate);}

	  /**
	   * Check if the select show object will be shown transient
       * (i.e. with the semantics of Wimp_CreateMenu)
       *
       * @returns true if the select show object will be shown transient
	   */
	  bool select_show_transient() const {return flag(0, 4);}
	  /**
	   * Set to show the select show object as a transient
       * (i.e. with the semantics of Wimp_CreateMenu)
       *
       * @param transient true to show the select object as transient
	   */
	  void select_show_transient(bool transient) {flag(0, 4, transient);}

	  /**
	   * Check if the adjust show object will be shown transient
       * (i.e. with the semantics of Wimp_CreateMenu)
       *
       * @returns true if the adjust show object will be shown transient
	   */
	  bool adjust_show_transient() const {return flag(0, 8);}
	  /**
	   * Set to show the adjust object as a transient
       * (i.e. with the semantics of Wimp_CreateMenu)
       *
       * @param transient true to show the adjust object as transient
	   */
	  void adjust_show_transient(bool transient) {flag(0, 8, transient);}

	  /**
       * Check if the iconbar clicked (or client-specified) event
	   * is generate when select is clicked
	   *
	   * @returns true if the clicked event is generated on select
	   */
	  bool generate_clicked_on_select() const {return flag(0, 32);}
	  /**
       * Set if the iconbar clicked (or client-specified) event
	   * is generate when select is clicked
	   *
	   * @param generate true if the clicked event is generated on select
	   */
	  void generate_clicked_on_select(bool generate) {flag(0, 32, generate);}

	  /**
       * Check if the iconbar clicked (or client-specified) event
	   * is generate when adjust is clicked
	   *
	   * @returns true if the clicked event is generated on adjust
	   */
	  bool generate_clicked_on_adjust() const {return flag(0, 64);}
	  /**
       * Set if the iconbar clicked (or client-specified) event
	   * is generate when adjust is clicked
	   *
	   * @param generate true if the clicked event is generated on adjust
	   */
	  void generate_clicked_on_adjust(bool generate) {flag(0, 64, generate);}
	  
      /**
       * Get a negative integer giving the position of this icon on the iconbar.
       */
       int position() const {return int_value(4);}
      /**
       * Set a negative integer giving the position of this icon on the iconbar.
       *
       * @param value position on iconbar
       */
       void position(int value) {int_value(4, value);}

      /**
       * Get The priority used to position the icon on the iconbar.
    
       */
       int priority() const {return int_value(8);}
      /**
       * Set The priority used to position the icon on the iconbar.
       */
       void priority(int value) {int_value(8, value);}

	   /**
	    * Get the sprite name
		*/
	   const char *sprite_name() const {return string(12);}

	   /**
	    * Set the spritename
		*
		* @param name name of sprite
		* @param max_length maximum length for sprite name or -1 to use current value.
		*                   This is alway adjusted to allow for the full length on name.
		*/
	   void sprite_name(const char *name, int max_length = -1) {string_with_length(12, name, max_length);}
	   /**
	    * Set the spritename
		*
		* @param name name of sprite
		* @param max_length maximum length for sprite name or -1 to use current value.
		*                   This is alway adjusted to allow for the full length on name.
		*/
	   void sprite_name(const std::string &name, int max_length = -1) {string_with_length(12, name, max_length);}
	   
	   /**
	    * Get the sprite name length
		*/
	   int max_sprite_name() const {return int_value(16);}


   	   /**
	    * Get the iconbar icon text
		*/
	   const char *text() const {return message(20);}

	   /**
	    * Set the text
		*
		* @param value iconbar icon text
		* @param max_length maximum length for text or -1 to use current value.
		*                   This is alway adjusted to allow for the full length of the text.
		*/
	   void text(const char *value, int max_length = -1) {message_with_length(20, value, max_length);}
	   /**
	    * Set the text
		*
		* @param value iconbar icon text
		* @param max_length maximum length for text or -1 to use current value.
		*                   This is alway adjusted to allow for the full length of the text.
		*/
	   void text(const std::string &value, int max_length = -1) {message_with_length(20, value, max_length);}
	   
	   /**
	    * Get the maximum text length
		*/
	   int max_text() const {return int_value(24);}

	   /**
	    * Get name of menu to show
		*/
	   const char *menu() const {return string(28);}

	   /**
	    * Set name of menu to show
	    *
	    * @param name name of menu to show
		*/
	   void menu(const char *name) {string(28, name);}
	   /**
	    * Set name of menu to show
	    *
	    * @param name name of menu to show
		*/
	   void menu(std::string name) {string(28, name);}

      /**
       * Get The event to generate when select is clicked or 0 for the default.
       */
       unsigned int select_event() const {return uint_value(32);}
      /**
       * Set The event to generate when select is clicked or 0 for the default.
       *
       * @param value number of event to generate
       */
       void select_event(unsigned int value) {uint_value(32, value);}

      /**
       * Get The event to generate when adjust is clicked or 0 for the default.
       */
       unsigned int adjust_event() const {return uint_value(36);}
      /**
       * Set The event to generate when adjust is clicked or 0 for the default.
       *
       * @param value number of event to generate
       */
       void adjust_event(unsigned int value) {uint_value(36, value);}

   	   /**
	    * Get name of object to show on select
	    *
	    * @returns pointer to zero terminated name of object to show or 0 if none.
		*/
	   const char *select_show() const {return string(40);}

	   /**
	    * Set name of  object to show on select
	    *
	    * @param name name of object to show on select or 0 for none
		*/
	   void select_show(const char *name) {string(40, name);}
	   /**
	    * Set name of  object to show on select
	    *
	    * @param name name of object to show on select
		*/
	   void select_show(std::string name) {string(40, name);}

   	   /**
	    * Get name of object to show on adjust
	    *
	    * @returns pointer to zero terminated name of object to show or 0 if none.
		*/
	   const char *adjust_show() const {return string(44);}

	   /**
	    * Set name of  object to show on adjust
	    *
	    * @param name name of object to show on adjust or 0 for none
		*/
	   void adjust_show(const char *name) {string(44, name);}
	   /**
	    * Set name of  object to show on adjust
	    *
	    * @param name name of object to show on adjust
		*/
	   void adjust_show(std::string name) {string(44, name);}

   	   /**
	    * Get the iconbar icon help message
		*/
	   const char *help_message() const {return message(48);}

	   /**
	    * Set the help message
		*
		* @param value iconbar icon help message or 0 for none
		* @param max_length maximum length for help message or -1 to use current value.
		*                   This is alway adjusted to allow for the full length of the help message.
		*/
	   void help_message(const char *value, int max_length = -1) {message_with_length(48, value, max_length);}
	   /**
	    * Set the help message
		*
		* @param value iconbar icon help message
		* @param max_length maximum length for help message or -1 to use current value.
		*                   This is alway adjusted to allow for the full length of the help message.
		*/
	   void help_message(const std::string &value, int max_length = -1) {message_with_length(48, value, max_length);}
	   
	   /**
	    * Get the maximum help message length
		*/
	   int max_help_message() const {return int_value(52);}

};

}

}
#endif // TBX_RES_IconBar_H
