/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2021 Alan Buckley   All Rights Reserved.
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

#ifndef tbx_sprite_h
#define tbx_sprite_h

#include "colour.h"
#include "image.h"
#include "point.h"
#include "size.h"
#include "scalefactors.h"

#include <string>
#include <map>

namespace tbx
{
	class SpriteArea;
	class Sprite;
	class UserSprite;
	class WimpSprite;

	/**
	 * Maximum length of a sprite name including a terminating
	 * character 0.
	 */
	const int SPRITE_NAMELEN = 13;

	/**
	 * A class to hold a list of Colours for the a colour palette.
	 *
	 * Typically this is returned from a sprite that has < 32 thousand colours
	 */
	class ColourPalette
	{
	   public:
		  ColourPalette(int size = 0);
		  ColourPalette(const ColourPalette &other);
		  ~ColourPalette();

		  void resize(int new_size);

		  void desktop_palette();

		  ColourPalette &operator=(const ColourPalette &other);
		  bool operator==(const ColourPalette &other);
		  bool operator!=(const ColourPalette &other);

		  /**
		   * Get reference to colour in the palette
		   *
		   * @param index index of entry in the palette  (0 to size-1)
		   * @returns reference to the colour
		   */
		  Colour &operator[](int index) {return _palette[index];}
		  /**
		   * Get constant reference to colour in the palette
		   *
		   * @param index index of entry in the palette  (0 to size-1)
		   * @returns constant reference to the colour
		   */
		  const Colour &operator[](int index) const {return _palette[index];}

		  /**
		   * Set the specified index to the given colour
		   *
		   * @param index index of entry to set palette for (0 to size-1)
		   * @param col new colour for the entry
		   */
		  void entry(int index, const Colour &col) {_palette[index] = col;};
		  /**
		   * Return the colour for the specified index.
		   *
		   * @param index index of entry to return palette from (0 to size-1)
		   * @return Colour at the given index
		   */
		  Colour entry(int index) const {return _palette[index];};

		  /**
		   * Return the size of the palette
		   */
		  int size() const       {return _size;};
		  /**
		   * Get a pointer to the array of colours
		   */
		  const Colour *address() const {return &_palette[0];};

	   private:
		  int _size;
		  Colour *_palette;
	};


	/**
	 * Class for a sprite colour translation table.
	 *
	 * The table is used to convert the colours from a sprite to
	 * the screen or from one sprite to another
	 */
	class TranslationTable
	{
	   public:
		  /**
		   * Construct an empty translation table
		   */
		  TranslationTable() {_table = 0;_size = 0;};

		  bool create(int mode, const ColourPalette *pal = 0);
		  bool create(const UserSprite *s);
		  bool create(UserSprite *source, UserSprite *target);
		  bool create(const WimpSprite *s);

		  /**
		   * Return a pointer to the translation table data
		   */
		  unsigned char *data() const {return _table;};

	   private:
		  int initialise(int mode);

	   private:
		  unsigned char *_table;
		  int _size;
	};



	/**
	 * Standard sprite formats (colour <= 256)
	 */
	enum SpriteFormat {
		SF_Colour2dpi90x45 = 0,
		SF_Colour4dpi45 = 1,
		SF_Colour2dpi45 = 4,
		SF_Colour4dpi90x45 = 8,
		SF_Colour16dpi45 = 9,
		SF_Colour16dpi90x45 = 12,
		SF_Colour256dpi45 = 13,
		SF_Colour256dpi90x45 = 15,
		SF_Colour2dpi90 = 18,
		SF_Colour4dpi90 = 19,
		SF_Colour16dpi90 = 20,
		SF_Colour256dpi90 = 21
	};

	/**
	 * Sprite colour constant
	 */
	enum SpriteColours {
		SC_Colour2   = 1,
		SC_Colour4   = 2,
		SC_Colour16  = 3,
		SC_Colour256 = 4,
		SC_Colour32K = 5,
		SC_Colour16M = 6
	};

	/**
	 *  Return new format sprite mode.
	 *
	 *   Can only be used in Risc OS 3.5 and later.
	 * @param colours constant to specify number of colours
	 * @param horzDpi - horizontal dots per inch
	 * @param vertDpi - vertical dots per inch
	 */
	inline int sprite_mode(SpriteColours colours, int horzDpi = 90, int vertDpi = 90)
	{
		return (colours << 27) | (horzDpi << 1) | (vertDpi << 14) | 1;
	}

	/**
	 * Exception thrown for sprite method failures
	 */
	class SpriteException
	{
	   std::string _message;
	   public:
	   	   /**
	   	    * Construct exception with the given message
	   	    *
	   	    * @param m Message for error
	   	    */
	       SpriteException(const std::string &m) {_message = m;}
	       /**
	        * Return the error message for the exception
	        */
	       const std::string &what() const {return _message;}
	};

	/**
	 * Type for pointer to underlying RISC OS Sprite area.
	 */
	typedef int *OsSpriteAreaPtr;

	/**
	 * Constant representing the WIMP sprite area
	 */
	const OsSpriteAreaPtr WIMP_SPRITEAREA = (OsSpriteAreaPtr)1;

	/**
	 * Type for pointer to underlyin RISC OS sprite
	 */
	typedef int *OsSpritePtr;

	/**
	 * A SpriteArea holds zero or more user sprites.
	 *
	 * New sprites can be created within it.
	 */
	class SpriteArea
	{
	   public:
		  SpriteArea();
		  ~SpriteArea();
		  SpriteArea(OsSpriteAreaPtr data, bool ownsarea = false);
		  SpriteArea(int size);

          SpriteArea(const SpriteArea &other);
          SpriteArea &operator=(const SpriteArea &other);

          void set(OsSpriteAreaPtr data, bool ownsarea = false);

          /**
           * Returns true if area is valid.
           */
		  bool is_valid() const	{return (_area != 0);}

		  void clear();
		  bool initialise(int size);
		  bool load(const std::string &file_name);
		  bool merge(const std::string &file_name);
		  bool save(const std::string &file_name) const;

		  int size() const;
		  int free_space() const;
		  bool resize(int new_size);

		  int sprite_count() const;
		  UserSprite get_sprite(const std::string &name);
		  UserSprite create_sprite(const std::string &name, int width, int height, int mode, bool palette = false);
		  UserSprite create_sprite_pixels(const std::string &name, int width, int height, int mode, bool palette = false);

		  /**
		   * Return pointer to underlying sprite are that can be used when
		   * using low-level area manipulation.
		   */
		  OsSpriteAreaPtr pointer() const {return _area;};

		  bool rename(UserSprite &sprite, const std::string &newname);
		  bool erase(UserSprite &sprite);
		  bool erase(const std::string name);

		  static int calculate_memory(int width, int height, int mode, bool withPalette);
		  static int calculate_mask_size(int width, int height, int mode);
		  static int get_bits_per_pixel(int mode);

          class iterator;

		  iterator begin();
		  iterator end();

		  UserSprite get_sprite(OsSpritePtr sprite_ptr);

	   private:
		  OsSpriteAreaPtr _area;
		  bool _owns_area;
	};

	/**
	 * Enumeration of the plot actions for sprites.
	 */
   enum sprite_plot_action {SPA_OVERWRITE, SPA_OR, SPA_AND, SPA_XOR,
			SPA_INVERT, SPA_NONE, SPA_AND_NOT, SPA_OR_NOT, SPA_USE_MASK};

	/**
	 * Common base class for the UserSprite and WimpSprite
	 * classes.
	 */
	class Sprite : public Image
	{
	public:
		virtual ~Sprite() {};

		// Image overrides
		virtual void plot(int x, int y) const;
		virtual void plot(const Point &pos) const;

		/**
		 * Plot sprite with no scaling or colour translation
		 *
		 * @param pos OS coordinates of bottom left of where to plot the sprite
		 * @param code sprite_plot_action flags speficying how the sprite is
		 * combined with the screen (default SPA_USE_MASK)
		 */
	   virtual void plot_raw(const Point &pos, int code = SPA_USE_MASK) const = 0;
		/**
		 * Plot sprite scaled
		 *
		 * @param pos OS coordinates of bottom left of where to plot the sprite
		 * @param sf ScaleFactors used to scale the sprite
		 * @param tt Translation table for colours in plot, 0 (the default) means
		 *  don't translate the colours
		 * @param code sprite_plot_action flags speficying how the sprite is
		 * combined with the screen (default SPA_USE_MASK)
		 */
	   virtual void plot_scaled(const Point &pos, const ScaleFactors *sf, const TranslationTable *tt = 0, int code = SPA_USE_MASK) const = 0;
		/**
		 * Plot sprite to screen calculating the correct colour translation and scaling.
		 *
		 * @param pos OS coordinates of bottom left of where to plot the sprite
		 * @param code sprite_plot_action flags speficying how the sprite is
		 * combined with the screen (default SPA_USE_MASK)
		 */
	   virtual void plot_screen(const Point &pos, int code = SPA_USE_MASK) const = 0;
		/**
		 * Plot sprite with no scaling or colour translation
		 *
		 * @param x left of destination to plot the sprite in OS coordinates
		 * @param y bottom of destination to plot the sprite in OS coordinates
		 * @param code sprite_plot_action flags
		 */
	   virtual void plot_raw(int x, int y, int code = SPA_USE_MASK) const = 0;
		/**
		 * Plot sprite scaled
		 *
		 * @param x left of destination to plot the sprite in OS coordinates
		 * @param y bottom of destination to plot the sprite in OS coordinates
		 * @param sf ScaleFactors used to scale the sprite
		 * @param tt Translation table for colours in plot, 0 (the default) means
		 *  don't translate the colours
		 * @param code sprite_plot_action flags speficying how the sprite is
		 * combined with the screen (default SPA_USE_MASK)
		 */
	   virtual void plot_scaled(int x, int y, const ScaleFactors *sf, const TranslationTable *tt = 0, int code = SPA_USE_MASK) const = 0;
		/**
		 * Plot sprite to screen calculating the correct colour translation and scaling.
		 *
		 * @param x left of destination to plot the sprite in OS coordinates
		 * @param y bottom of destination to plot the sprite in OS coordinates
		 * @param code sprite_plot_action flags speficying how the sprite is
		 * combined with the screen (default SPA_USE_MASK)
		 */
	   virtual void plot_screen(int x, int y, int code = SPA_USE_MASK) const = 0;

	   /**
	    * Get the name of the sprite
	    *
	    * @returns the name of the sprite as a string
	    */
	   virtual std::string name() const = 0;
	   /**
	    * Return sprite area id used for calls that take an area pointer
	    * or a special value for WIMP/System areas.
	    */
	   virtual int area_id() const = 0;

	   /**
	    * Get information about the sprite
	    *
	    * @param pixel_size pointer to Size to be updated with size of
	    *        sprite in pixels
	    * @param mode pointer to integer to receive the sprite mode number
	    *        or 0 (the default) not to return the mode
	    * @param mask pointer to a boolean which will be set to true if the
	    *        sprite has a mask or 0 (the default) not to return the mask
	    * @returns true if successfule
	    */
	   virtual bool info(Size *pixel_size, int *mode  = NULL, bool *mask = NULL) const = 0;

	   // Functions using a virtual call to the derived class
	   /**
	    * Return the size of the sprite
	    *
	    * @returns size of the sprite in OS units
	    */
		Size size() const;
	   /**
		* Return the width of the sprite
		*
		* @returns width of the sprite in OS units
		*/
		int width() const;
	   /**
		* Return the height of the sprite
		*
		* @returns height of the sprite in OS units
		*/
		int height() const;

		/**
		 * Get size of the sprite in pixels
		 *
		 * @returns size of the sprite in pixels
		 */
		Size pixel_size() const {Size ps; info(&ps); return ps;};
		/**
		 * Get the mode of the sprite
		 *
		 * @returns the mode number the sprite is defined for
		 */
		int mode()       const {int m; info(NULL, &m); return m;};
		/**
		 * Check if the sprite has a mask
		 *
		 * @returns true if the sprite has a mask
		 */
		bool has_mask()      const {bool m; info(NULL, NULL, &m); return m;};

		/**
		 * Get the scale factors required to plot this sprite in
		 * the WIMP as its logical size.
		 *
		 * @param factor scale factors updated with the correct value
		 */
	   virtual void get_wimp_scale(ScaleFactors &factor) const = 0;
	};

	/**
	 * Sprite from a user sprite area.
	 *
	 * A sprite reference is returned using
	 * the SpriteArea::get_sprite method.
	 *
	 * A runtime error or crash will occur is you try to use a sprite
	 * after the SpriteArea it was retrieved from has been deleted.
	 */
	class UserSprite : public Sprite
	{
	   protected:
		   UserSprite(SpriteArea *area, int offset);

       public:
           UserSprite();
           UserSprite(SpriteArea *area, const std::string &name);
           virtual ~UserSprite();

           /**
            * Check if two sprite classes refer to the same underlying sprite
            */
		   bool operator==(const UserSprite &other) const {return _offset == other._offset && _area == other._area;}
           /**
            * Check if two sprite classes DO NOT refer to the same underlying sprite
            */
		   bool operator!=(const UserSprite &other) const {return _offset != other._offset || _area != other._area;}

		   /**
		    * Check this sprite object is valid.
		    */
           bool is_valid() const {return _area != 0 && _offset != 0;}
		   bool rename(const std::string &name);

		   virtual void plot_raw(const Point &pos, int code = SPA_USE_MASK) const;
		   virtual void plot_scaled(const Point &pos, const ScaleFactors *sf, const TranslationTable *tt = 0, int code = SPA_USE_MASK) const;
		   virtual void plot_screen(const Point &pos, int code = SPA_USE_MASK) const;
		   virtual void plot_raw(int x, int y, int code = SPA_USE_MASK) const;
		   virtual void plot_scaled(int x, int y, const ScaleFactors *sf, const TranslationTable *tt = 0, int code = SPA_USE_MASK) const;
		   virtual void plot_screen(int x, int y, int code = SPA_USE_MASK) const;

		   virtual std::string name() const;
		   virtual bool info(Size *pixel_size, int *mode  = NULL, bool *mask = NULL) const;

		   bool get_palette(ColourPalette &pal) const;
		   bool set_palette(ColourPalette &pal);
		   bool create_palette(bool col256 = false);
		   bool create_palette(ColourPalette &pal);
		   bool remove_palette();
		   bool create_mask();
		   bool remove_mask();

		   int pixel(int x, int y) const;
		   void pixel(int x, int y, int gcol);
		   void pixel(int x, int y, int gcol, int tint);
		   int pixel(int x, int y, int *tint) const;

		   bool mask_pixel(int x, int y) const;
		   void mask_pixel(int x, int y, bool on);

		   /**
		    * Return SpriteArea this sprite is from
		    */
		   SpriteArea *get_sprite_area()	const {return _area;};

		   /**
		    * Return offset of this sprite in the sprite area
		    */
		   int offset() const			{return _offset;}

		   /**
		    * Return pointer to underlying RISC OS sprite
		    */
		   OsSpritePtr pointer() const	{return (OsSpritePtr)((char *)(_area->pointer()) + _offset);};

		   /**
		    * Return sprite area id used for calls that take an area pointer
		    * or a special value for WIMP/System areas.
		    */
		   virtual int area_id() const {return (int)(_area->pointer());}

		   /**
		    * Returns true if this sprite has a palette
		    */
		   bool has_palette() const {return (*(pointer() + 8) > 0x2C);};

		   virtual void get_wimp_scale(ScaleFactors &factor) const;

		   friend class SpriteArea;

	   protected:
		   SpriteArea *_area; //!< Sprite area containing this sprite
		   int _offset;       //!< offset of sprite within the sprite area
	};

    /**
     * Iterator class for iterating through UserSprites in a sprite area
     */
    class SpriteArea::iterator
    {
    	  UserSprite _sprite;
    	  friend class SpriteArea;

    public:
          /**
           * Move to next sprite in the area
           * @returns *this
           */
    	  iterator operator++() {next_sprite(); return *this;}
    	  /**
    	   * Move to next sprite in the area
    	   * @returns copy of iterator before it was moved
    	   */
    	  iterator operator++(int) {iterator tmp(*this); next_sprite(); return tmp;}
    	  /**
    	   * Get the sprite the iterator is pointing at
    	   */
    	  UserSprite operator*() {return _sprite;}
    	  /**
    	   * See if iterators are equal
    	   * @param other iterator to compare to
    	   * @returns true if the iterators are equal
    	   */
    	  bool operator==(const iterator &other) const {return _sprite == other._sprite;}
    	  /**
    	   * See if iterators are not equal
    	   * @param other iterator to compare to
    	   * @returns true if the iterators are not equal
    	   */
    	  bool operator!=(const iterator &other) const {return _sprite != other._sprite;}

    private:
    	  void next_sprite();
    };

    /**
     * Class to capture screen output to a sprite
     */
    class SpriteCapture
    {
        char *_save_area;
        int _save_regs[4];
        UserSprite *_sprite;
        bool _capturing;

    public:
        SpriteCapture(UserSprite *sprite, bool start_capture = false, bool to_mask = false);
        ~SpriteCapture();

        bool capture();
        bool release();
        /**
         * Check if this capture is capturing to the sprite
         *
         * @returns true if capturing
         */
        bool is_capturing() {return _capturing;}
    };

	/**
	 * Class for handling sprites from the Wimp sprite pool.
	 *
	 * These sprites are read only so just have functions
	 * for getting information and plotting a sprite.
	 *
	 */
	class WimpSprite : public Sprite
	{
	public:
		/**
		 * Construct a WIMP sprite.
		 *
		 * There is no check on the name given here.
		 * If it is incorrect the other sprite operations
		 * will fail silently.
		 *
		 * Call the exist function to check if a sprite is
		 * currently in the WIMP sprite pool.
		 *
		 * @param sname of sprite in to reference
		 */
		WimpSprite(const std::string &sname) : _name(sname) {};
		/**
		 * Construct a WIMP sprite.
		 *
		 * There is no check on the name given here.
		 * If it is incorrect the other sprite operations
		 * will fail silently.
		 *
		 * Call the exist function to check if a sprite is
		 * currently in the WIMP sprite pool.
		 *
		 * @param sname of sprite in to reference
		 */
		WimpSprite(const char *sname) : _name(sname) {};
		WimpSprite(int file_type);
		WimpSprite(int file_type, std::string leafname);

		virtual ~WimpSprite() {};

		bool exist() const;

	   virtual void plot_raw(const Point &pos, int code = SPA_USE_MASK) const ;
	   virtual void plot_scaled(const Point &pos, const ScaleFactors *sf, const TranslationTable *tt = NULL, int code = SPA_USE_MASK) const;
	   virtual void plot_screen(const Point &pos, int code = SPA_USE_MASK) const;
	   virtual void plot_raw(int x, int y, int code = SPA_USE_MASK) const ;
	   virtual void plot_scaled(int x, int y, const ScaleFactors *sf, const TranslationTable *tt = NULL, int code = SPA_USE_MASK) const;
	   virtual void plot_screen(int x, int y, int code = SPA_USE_MASK) const;

	   virtual std::string name() const {return _name;}
	   virtual bool info(Size *pixel_size, int *mode  = NULL, bool *mask = NULL) const;

	   virtual void get_wimp_scale(ScaleFactors &factor) const;

	   /**
	    * Return sprite area id used for calls that take an area pointer
	    * or a special value for WIMP/System areas.
	    *
	    * This is always 1 for the wimp sprite area.
	    */
	   virtual int area_id() const {return 1;}

	   bool has_palette() const;


	private:
		std::string _name;
	};
};

#endif
