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

#ifndef TBX_RES_MENU_H
#define TBX_RES_MENU_H

#include "resobject.h"
#include "resiteratorbase.h"
#include "../handles.h"

namespace tbx
{
class Menu;
namespace res
{

class ResMenu;

const int MENU_DATA_SIZE = 32;
const int MENU_ITEM_SIZE = 40;

/**
 * Menu item object
 */
class ResMenuItem : public ResBase
{
	friend class ResMenu;

	ResMenuItem(void *item_header, int size, ResData *data);

	// Allow Menu to add items
	friend class tbx::Menu;
	char *header() const {return _impl->header();}

public:
	/**
	 * Create a ResMenuItem as a copy of another
	 *
	 * @param other ResMenuItem to copy
	 */
	ResMenuItem(const ResMenuItem &other) : ResBase(other)
	{
	}

	virtual ~ResMenuItem() {}

	/**
	 * Assign this menu item to be equal to another
	 *
	 * @param other ResMenuItem to copy
	 */
	ResMenuItem &operator=(const ResMenuItem &other)
	{
		ResBase::operator=(other);
		return *this;
	}
	
	ResMenuItem();

protected:
	/**
	 * Make item writeable.
	 */
	void make_writeable()
	{
		if (!_impl->unique())
		{
			ResImpl *new_impl = new ResImpl(*_impl);
			_impl->release();
			_impl = new_impl;
		}
	}

public:
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
	   * Check if the item is ticked
	   *
	   * @returns true if the item is ticked
	   */
	  bool ticked() const {return flag(0,1);}
	  /**
	   * Set if the item is ticked
	   *
	   * @param t set to true if the item should be ticked
	   */
	  void ticked(bool t) {flag(0,1,t);}

	  /**
	   * Check if this entry is followed by a dotted line
	   *
	   * @returns true if the entry is followed by a dotted line
	   */
	  bool dotted_line() const {return flag(0,2);}
	  /**
	   * Set if this entry is followed by a dotted line
	   *
	   * @param value set to true if the entry is followed by a dotted line
	   */
	  void dotted_line(bool value) {flag(0,2,value);}

	  /**
	   * Check if entry is faded
	   *
	   * @returns true if entry is faded
	   */
	  bool faded() const {return flag(0,256);}
	  /**
	   * Set if entry is faded
	   *
	   * @param value set to true if the entry should be faded
	   */
	  void faded(bool value) {flag(0,256, value);}

	  /**
	   * Check if this entry shows a sprite.
	   *
	   * When not set this entry shows text
	   *
	   * @returns true if this is a sprite menu item
	   */
	  bool sprite() const {return flag(0,512);}

      // TODO: Changing this flag changes in which table the text is stored (string or messages)
	  // void sprite(bool value) {flag(0,512,value);}

  	  /**
	   * Check if this entry has a sub menu.
	   *
	   * ie a sub menu arrow appears next to the entry
	   *
	   * @returns true if this menu has a sub menu
	   */
	  bool has_submenu() const {return flag(0,1024);}
  	  /**
	   * Set if this entry has a sub menu.
	   *
	   * ie a sub menu arrow appears next to the entry
	   *
	   * @param value set to true if this menu has a sub menu
	   */
	  void has_submenu(bool value) {flag(0,1024, value);}
  	  /**
  	   * Check if the sub menu event will be generated.
	   *
	   * The sub menu event occurs when the user traverses this entry's
	   * sub menu arrow with the mouse pointer (if has_submenu() == true).
	   *
	   * @returns true if the sub menu event will be generated
	   */
	  bool generate_submenu_event() const {return flag(0,2048);}
  	  /**
  	   * Set if the sub menu event will be generated.
	   *
	   * The sub menu event occurs when the user traverses this entry's
	   * sub menu arrow with the mouse pointer (if has_submenu() == true).
	   *
	   * @param value set to true if the sub menu event should be generated
	   */
	  void generate_submenu_event(bool value) {flag(0,2048, value);}
  	  /**
	   * Check if shown object will be shown transiently.
	   *
	   * @returns true if object will be shown transiently
	   */
	  bool show_transient() const {return flag(0,4096);}
  	  /**
	   * Set if shown object will be shown transiently.
	   *
	   * @param value set to true if object will be shown transiently
	   */
	  void show_transient(bool value) {flag(0,4096, value);}

	/**
	 * Get component ID of menu item
	 */
	  ComponentId component_id() const {return int_value(4);}
	  /**
	   * Set component ID
	   *
	   * @param id component ID of menu item
	   */
	  void component_id(ComponentId id) {int_value(4,id);}

    /**
	 * Get text or sprite name for entry
	 *
	 * The sprite() flag determines if this is a sprite or text entry
	 *
	 * @returns pointer to zero terminated text or sprite name
	 */
	 const char *text() const {return string(8);}
	/**
	 * Set text or sprite name for entry
	 *
	 * The sprite() flag determines if this is a sprite or text entry
	 *
	 * @param value pointer to zero terminated text or sprite name
	 * @param max_length maximum length of the text or sprite name that will
	 * be used with this menu item or -1 (the default) for the length of value
	 */
	 void text(const char *value, int max_length = -1) {make_writeable(); _impl->text_with_length(8, value, max_length, sprite());}
	/**
	 * Set text or sprite name for entry
	 *
	 * The sprite() flag determines if this is a sprite or text entry
	 *
	 * @param value text or sprite name
	 * @param max_length maximum length of the text or sprite name that will
	 * be used with this menu item or -1 (the default) for the length of value
	 */
	 void text(const std::string &value, int max_length = -1) {make_writeable(); _impl->text_with_length(8, value, max_length, sprite());}
	 /**
	  * Maximum space for text or sprite name
	  */
	 int max_text() const {return int_value(12);}

	 /**
	  * Get the name of the object to show on click
	  *
	  * @returns pointer to zero terminated object name or 0 if none
	  */
	 const char *click_show() const {return string(16);}
	 /**
	  * Set the name of the object to show on click
	  *
	  * @param show_name pointer to zero terminated object name or 0 if none
	  */
	 void click_show(const char *show_name) {string(16, show_name);}
	 /**
	  * Set the name of the object to show on click
	  *
	  * @param show_name object name
	  */
	 void click_show(const std::string &show_name) {string(16, show_name);}

 	 /**
 	  * Get the name of the sub menu to show for this menu item.
 	  *
 	  * @returns pointer to zero terminated sub menu object name or 0 for none.
	  */
	 const char *submenu_show() const {return string(20);}
 	 /**
 	  * Set the name of the sub menu to show for this menu item.
 	  *
 	  * @param show_name pointer to zero terminated sub menu object name or 0 for none.
	  */
	 void submenu_show(char *show_name) {string(20, show_name);}
 	 /**
 	  * Set the name of the sub menu to show for this menu item.
 	  *
 	  * @param show_name sub menu object name
	  */
	 void submenu_show(const std::string &show_name) {string(20, show_name);}
	 /**
	  * Get the event ID generated when sub menu is shown.
	  *
	  * @returns event ID or 0 for the default
	  */
	 int submenu_event() const {return int_value(24);}
	 /**
	  * Set the event ID generated when sub menu is shown.
	  *
	  * @param event_id event ID or 0 for the default
	  */
	 void submenu_event(int event_id) {int_value(24, event_id);}
	 /**
	  * Get the event ID generated when the item is clicked.
	  *
	  * @returns event ID or 0 for the default
	  */
	 int click_event() const {return int_value(28);}
	 /**
	  * Set the event ID generated when the item is clicked.
	  *
	  * @param event_id event ID or 0 for the default
	  */
	 void click_event(int event_id) {int_value(28, event_id);}

	   /**
	    * Get the menu item help message
	    *
	    * @returns pointer to zero terminated help message or 0 for none.
		*/
	   const char *help_message() const {return message(32);}

	   /**
	    * Set the item help message
		*
		* @param value pointer to zero terminated menu help message or 0 for none.
		* @param max_length maximum length for help message or -1 to use current value.
		*                   This is alway adjusted to allow for the full length of the help message.
		*/
	   void help_message(const char *value, int max_length = -1) {message_with_length(32, value, max_length);}
	   /**
	    * Set the item help message
		*
		* @param value menu help message.
		* @param max_length maximum length for help message or -1 to use current value.
		*                   This is alway adjusted to allow for the full length of the help message.
		*/
	   void help_message(const std::string &value, int max_length = -1) {message_with_length(32, value, max_length);}
	   
	   /**
	    * Get the maximum help message length
		*/
	   int max_help_message() const {return int_value(36);}
};

/**
 * Menu object template class
 */
class ResMenu : public ResObject
{
   public:
      enum { CLASS_ID = 0x828c0};

      /**
      * Constructor and empty ResMenu
      * @param name template name of the object
      */
      ResMenu(std::string name) : 
          ResObject(name, CLASS_ID, 102, 36 + 32)
      {
		init_message(4, 0); // Title bar
		init_message(12, 0); // Help message
		about_to_be_shown_event(-1);
		has_been_hidden_event(-1);
      }

      /**
      * Copy constructor
      *
      * @param other object to create copy of
      */
      ResMenu(const ResMenu &other) : ResObject(other) {}

      /**
      * Construct from a ResObject
      *
      * @param other object to create copy of
      */
      ResMenu(const ResObject &other) : ResObject(other) {check_class_id(CLASS_ID);}

      /**
      * Assignment
      *
      * @param other object to create copy of
      */
      ResMenu&operator=(const ResMenu&other)
      {
         ResObject::operator=(other);
         return *this;
      }

      /**
      * Assignment from ResObject
      *
      * @param other object to create copy of
      */
      ResMenu&operator=(const ResObject &other)
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
	   * @param value new flags value
	   */
	  void flags(unsigned int value) {uint_value(0, value);}

	  /**
	   * Check if about to be shown event will be generated
	   *
	   * @returns true if the about to be shown event will be generated
	   */
	  bool generate_about_to_be_shown() const {return flag(0, 1);}
	  /**
	   * Set if about to be shown event will be generated
	   *
	   * @param generate set to true if the about to be shown event should be generated
	   */
	  void generate_about_to_be_shown(bool generate) {flag(0, 1, generate);}

	  /**
	   * Check if event will be generated when the menu has been removed from the screen.
	   *
	   * @returns true if an event is generated when the menu is hidden
	   */
  	  bool generate_has_been_hidden() const {return flag(0, 2);}
	  /**
	   * Set if event will be generated when the menu has been removed from the screen.
	   *
	   * @param generate set to true if an event should be generated when the menu is hidden
	   */
	  void generate_has_been_hidden(bool generate) {flag(0, 2, generate);}

	    /**
	    * Get the iconbar icon title
	    *
	    * @returns pointer to zero terminated title or "" for no title bar or 0 for no title.
		*/
	   const char *title() const {return message(4);}

	   /**
	    * Set the title
		*
		* @param value menu title (0 means no title, an empty string means no titlebar)
		* @param max_length maximum length for title or -1 to use current value.
		*                   This is always adjusted to allow for the full length of the title.
		*/
	   void title(const char *value, int max_length = -1) {message_with_length(4, value, max_length);}
	   /**
	    * Set the title
		*
		* @param value menu title (an empty string means no titlebar)
		* @param max_length maximum length for title or -1 to use current value.
		*                   This is always adjusted to allow for the full length of the title.
		*/
	   void title(const std::string &value, int max_length = -1) {message_with_length(4, value, max_length);}
	   
	   /**
	    * Get the maximum title length
		*/
	   int max_title() const {return int_value(8);}

	   /**
	    * Get the menu help message
	    *
	    * @returns pointer to zero terminated help message or 0 for none
		*/
	   const char *help_message() const {return message(12);}

	   /**
	    * Set the help message
		*
		* @param value pointer to zero terminated help message or 0 for none
		* @param max_length maximum length for help message or -1 to use current value.
		*                   This is always adjusted to allow for the full length of the help message.
		*/
	   void help_message(const char *value, int max_length = -1) {message_with_length(12, value, max_length);}
	   /**
	    * Set the help message
		*
		* @param value help message
		* @param max_length maximum length for help message or -1 to use current value.
		*                   This is always adjusted to allow for the full length of the help message.
		*/
	   void help_message(const std::string &value, int max_length = -1) {message_with_length(12, value, max_length);}
	   
	   /**
	    * Get the maximum help message length
		*/
	   int max_help_message() const {return int_value(16);}

      /**
       * Get the event to generate when the menu is shown.
	   *
	   * @returns event ID or -1 for the default.
       */
	   int about_to_be_shown_event() const {return int_value(20);}

      /**
       * Set the event to generate when the menu is shown.
	   *
	   * @param event_id event ID or -1 for the default.
       */
	   void about_to_be_shown_event(int event_id) {int_value(20, event_id);}

	  /**
       * Get the event to generate when menu is hidden.
	   *
	   * @returns event ID or -1 for the default.
       */
	   int has_been_hidden_event() const {return int_value(24);}

      /**
       * Set the event to generate when the menu is hidden.
	   *
	   * @param event_id event ID or -1 for the default.
       */
	   void has_been_hidden_event(int event_id) {int_value(24, event_id);}

	  /**
	   * Get the number of menu entries
	   */
	   int item_count() const {return int_value(28);}

	   ResMenuItem item(ComponentId component_id) const;

	   ResMenuItem item_at(int index) const;
	   ResMenuItem item_at_offset(int item_offset) const;


	   /**
	    * Constant iterator for menu items
		*/
	   class const_iterator : public ResIteratorBase<ResMenu>
	   {
		   const_iterator(const ResMenu *menu, int offset) : ResIteratorBase<ResMenu>(menu, offset) {};
		   friend class ResMenu;
	   public:
		   /**
		    * Increment constant iterator
		    *
		    * @returns constant iterator after increment
		    */
			const_iterator &operator++() {_offset += MENU_ITEM_SIZE;	return *this;}
		   /**
			* Increment constant iterator
			*
			* @returns constant iterator before increment
			*/
			const_iterator operator++(int) {const_iterator temp(*this);	_offset += MENU_ITEM_SIZE;	return temp;}
			/**
			 * Get the menu item this constant iterator refers to
			 *
			 * @returns menu item resource
			 */
			ResMenuItem operator*() const {return _object->item_at_offset(_offset);}
	   };
	   friend class const_iterator;

   	   /**
	    * Iterator for menu items
		*
		* Note: You can not use *iterator=value as the iterator
		* returns a copy of the item in the Menu not the item itself.
		*/
	   class iterator : public ResIteratorBase<ResMenu>
	   {
		   iterator(const ResMenu *menu, int offset) : ResIteratorBase<ResMenu>(menu, offset) {};
		   friend class ResMenu;
	   public:
		   /**
		    * Increment iterator
		    *
		    * @returns iterator after increment
		    */
			iterator &operator++() {_offset += MENU_ITEM_SIZE;	return *this;}
		   /**
			* Increment iterator
			*
			* @returns iterator before increment
			*/
			iterator operator++(int) {iterator temp(*this);	_offset += MENU_ITEM_SIZE;	return temp;}
			/**
			 * Get the menu item this iterator refers to
			 *
			 * @returns menu item resource
			 */
			ResMenuItem operator*() const {return _object->item_at_offset(_offset);}
	   };
	   friend class iterator;

   	   const_iterator begin() const;
	   const_iterator end() const;
   	   const_iterator cbegin() const;
	   const_iterator cend() const;
	   const_iterator find(ComponentId component_id) const;

	   iterator begin();
	   iterator end();
	   iterator find(ComponentId component_id);

	   bool contains(ComponentId component_id) const;
	   void add(const ResMenuItem &item);
	   void replace(const ResMenuItem &item);
	   void erase(ComponentId component_id);

	   iterator insert(iterator pos, const ResMenuItem &item);
	   iterator replace(iterator pos, const ResMenuItem &item);
	   iterator erase(iterator pos);
};

}
}

#endif
