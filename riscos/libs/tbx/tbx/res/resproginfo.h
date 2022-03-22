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

#ifndef TBX_RES_ProgInfo_H
#define TBX_RES_ProgInfo_H

#include "resobject.h"

#include <stdexcept>

namespace tbx
{
namespace res
{

/**
 * Class to represent a ProgInfo resource
 *
 * TODO: New version with web url
 */
class ResProgInfo : public ResObject
{
   public:
	   enum { CLASS_ID = 0x82b40 };

      /**
      * Constructor and empty ResProgInfo
      * @param name template name of the object
	  * @param has_web_button us newer (version 1.01 item with web page button)
      */
      ResProgInfo(std::string name, bool has_web_button = false ) : 
	   ResObject(name, CLASS_ID, has_web_button ? 101 : 100, 36 + (has_web_button ? 40 : 32))
      {
		  if (has_web_button) flags(24);
		  init_message(4,  0) ; // Title - default
		  init_message(12, 0) ; // Purpose
		  init_message(16, 0) ; // Author
		  licence_type(-1);
		  init_message(24, 0) ; // Version
		  init_string(28, 0);  ; // Alternative window
		  if (has_web_button)
		  {
			  init_message(32,0);
		  }
      }

      /**
      * Copy constructor
      *
      * @param other object to create copy of
      */
      ResProgInfo(const ResProgInfo &other) : ResObject(other) {}

      /**
      * Construct from a ResObject
      *
      * @param other object to create copy of
      */
      ResProgInfo(const ResObject &other) : ResObject(other) {check_class_id(CLASS_ID);}

      /**
      * Assignment
      *
      * @param other object to create copy of
      */
      ResProgInfo&operator=(const ResProgInfo&other)
      {
         ResObject::operator=(other);
         return *this;
      }

      /**
      * Assignment from ResObject
      *
      * @param other object to create copy of
      */
      ResProgInfo&operator=(const ResObject &other)
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
	   * Check if the about to be shown event should be generated
	   *
	   * @returns true if the about to be shown event should be generated
	   */
	  bool generate_about_to_be_shown() const {return flag(0, 1<<0);}
	  /**
	   * Set if the about to be shown event should be generated
	   *
	   * @param value set to true if the about to be shown event should be generated
	  */
	  void generate_about_to_be_shown(bool value) {flag(0,1<<0,value);}
	  /**
	   * Check if the dialogue completed event should be generated
	   *
	   * @returns true if the dialogue completed event should be generated
	   */
	  bool generate_dialogue_completed() const {return flag(0, 1<<1);}
	  /**
	   * Set if the dialogue completed event should be generated
	   *
	   * @param value set to true if the dialogue completed event should be generated
	   */
	  void generate_dialogue_completed(bool value) {flag(0,1<<1,value);}

	  /**
	   * Check if licence type is included on the dialogue
	   *
	   * @returns true if licence type is included on the dialogue
	   */
	  bool include_licence_type() const {return flag(0,4);}
	  /**
	   * Set if licence type is included on the dialogue
	   *
	   * @param value set to true to include licence type on the dialogue
	   */
	  void include_licence_type(bool value) {flag(0,4,value);}

	  /**
	   * Check if the web page button is included on the dialogue
	   *
	   * This property required version 1.01 or later of the ProgInfo
	   * Toolbox object.
	   *
	   * @returns true if web page button is included on the dialogue
	   */
	  bool include_web_page_button() const {return flag(0, 8);}
	  /**
	   * Check if the web page button generates a click event
	   *
	   * This property required version 1.01 or later of the ProgInfo
	   * Toolbox object.
	   *
	   * @returns true if web page button generates a click event
	   */
	  bool generate_web_page_click() const {return flag(0, 16);}
	  
	  /**
	   * Get the title of the dialogue
	   *
	   * @returns zero terminated string with title or 0 for the default title
	   */
	  const char *title() const {return message(4);}
	  /**
	   * Set the title of the dialogue
	   *
	   * @param value zero terminated string with the title or 0 for the default
	   * @param max_length maximum length the title will be changed to.
	   * -1 (the default) to use the length of the title given.
	   */
	  void title(const char *value, int max_length = -1) {message_with_length(4, value, max_length);}
	  /**
	   * Set the title of the dialogue
	   *
	   * @param value new title
	   * @param max_length maximum length the title will be changed to.
	   * -1 (the default) to use the length of the title given.
	   */
	  void title(const std::string &value, int max_length = -1) {message_with_length(4, value, max_length);}
	  /**
	   * Get the maximum size the title can be
	   */
	  int max_title() const {return int_value(8);}

	  /**
	   * Get the purpose
	   */
	  const char *purpose() const {return message(12);}

	   /**
	    * Set the purpose of the application
	    *
	    * @param value purpose of the application
		*/
	   void purpose(const char *value) {message(12, value);}
	   /**
	    * Set the purpose of the application
	    *
	    * @param value purpose of the application
		*/
	   void purpose(const std::string &value) {message(12, value);}

	   	/**
		 * Get the author
		 */
	   const char *author() const {return message(16);}

	   /**
	    * Set the author of the application
	    *
	    * @param value author of the application
		*/
	   void author(const char *value) {message(16, value);}
	   /**
	    * Set the author of the application
	    *
	    * @param value author of the application
		*/
	   void author(const std::string &value) {message(16, value);}

	   /**
	    * Get the licence type
	    *
	    * @returns the licence type
		*/
	   int licence_type() const {return int_value(20);}

	   /**
	    * Set the licence type.
	    *
	    * @param type the new licence type
	    *
	    * Can be one of the following values:
		* 0  public domain 
		* 1  single user 
		* 2  single machine 
		* 3  site 
		* 4  network 
		* 5  authority  
		*/
	   void licence_type(int type) {int_value(20, type);}

   	  /**
	   * Get the version
 	   */
	   const char *version() const {return message(24);}

	   /**
	    * Set the version of the application
	    *
	    * @param value the new version string
		*/
	   void version(const char *value) {message(24, value);}
	   /**
	    * Set the version of the application
	    *
	    * @param value the new version string
		*/
	   void version(const std::string &value) {message(24, value);}

	  /**
	   * Get the name of the window template that provides the window for this
	   * object.
	   *
	   * @returns name of window to use or 0 if default internal window will be used
	   */
	  const char *window() const {return string(28);}
	  /**
	   * Set the name of the window template that provides the window for this
	   * object.
	   *
	   * @param value The name of window to use or 0 if default internal window will be used
	   */
	  void window(const char *value) {string(28, value);}
	  /**
	   * Set the name of the window template that provides the window for this
	   * object.
	   *
	   * @param value The name of window to use or 0 if default internal window will be used
	   */
	  void window(const std::string &value) {string(28, value);}

	  /**
	   * Get URI
	   *
	   * This method requires the Proginfo Toolbox object version 1.01 or later
	   *
	   * @returns pointer to zero terminated uri string or "" if prog info version < 1.01 or 0 if none.
	   */
	  const char *uri() const {return (object_version() >= 101) ? string(32) : "";}
	  /**
	   * Set URI
	   *
	   * This method requires the Proginfo Toolbox object version 1.01 or later
	   *
	   * @param value pointer to zero terminated uri string or 0 if none.
	   * @throws std::invalid_argument if object version is < 1.01
	   */
	  void uri(const char *value)
	  {
		  //TODO: upgrading object if necessary rather than throw an exception
		  if (object_version() < 101) throw std::invalid_argument("ProgInfo version not high enough for URI");
		  message(32, value);
	  }
	  /**
	   * Set URI
	   *
	   * This method requires the Proginfo Toolbox object version 1.01 or later
	   *
	   * @param value pointer to zero terminated uri string or 0 if none.
	   * @throws std::invalid_argument if object version is < 1.01
	   */
	  void uri(const std::string &value)
	  {
		  //TODO: set upgrading object if necessary
		  if (object_version() < 101) throw std::invalid_argument("ProgInfo version not high enough for URI");
		  message(32, value);
	  }

	  /**
	   * Get the event generated for web page click added with version 1.01
	   *
	   * @returns event ID
	   */
	  int web_event() const {return (object_version() >= 101) ? int_value(36) : -1;}
	  /**
	   * Set the event generated for web page click added with version 1.01
	   *
	   * @param event_id event ID
	   * @throws std::invalid_argument if object version is < 1.01
	   */
	  void web_event(int event_id)
	  {
		  //TODO: set upgrading object if necessary
		  if (object_version() < 101) throw std::invalid_argument("ProgInfo version not high enough for URI");
		  int_value(36, event_id);
	  }
};

}

}
#endif // TBX_RES_ProgInfo_H
