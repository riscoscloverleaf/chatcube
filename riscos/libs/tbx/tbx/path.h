/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2014 Alan Buckley   All Rights Reserved.
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

#include <string>
#include <vector>
#include "kernel.h"

#ifndef TBX_PATH_H
#define TBX_PATH_H

namespace tbx
{
	/**
	 * Special file type returned for a directory
	 */
	const int FILE_TYPE_DIRECTORY = 0x1000;
	/**
	 * Special file type returned for an application directory.
	 * i.e. a directory where the first character is a '!'
	 */
	const int FILE_TYPE_APPLICATION = 0x2000;

	/**
	 *  Class to handle the 5 byte times.
	 *
	 *  The 5 byte time is used for time/date stamps on files
	 *  on RISCOS. It is the number of centi-seconds from
	 *  Midnight on 1st Jan 1900.
	 */
	class UTCTime
	{
	public:
		UTCTime();
		UTCTime(long long csecs);
		UTCTime(unsigned int loadAddress, unsigned int execAddress);
		UTCTime(const UTCTime &other);

		UTCTime &operator=(const UTCTime &other);

		static UTCTime now();

		std::string text() const;
		std::string text(const std::string &format) const;

		/**
		 * Get the low 4 bytes of the UTC time
		 *
		 * @returns the low 4 bytes as a 32 bit integer
		 */
		unsigned int low_word() const {return (unsigned int)(_centiseconds & 0xFFFFFFFF);}
		/**
		 * Get the high byte of the UTC time
		 *
		 * @return the 8 bits of the high byte
		 */
		unsigned char high_byte() const {return (unsigned char)((_centiseconds >> 32) & 0xFF);}

		/**
		 * Get the UTC time as centiseconds
		 *
		 * @returns number of centiseconds since Midnight Jan 1st 1900
		 */
		long long centiseconds() const {return _centiseconds;}

		/**
		 * Pointer to start of time in memory.
		 * This is used for calls to the OS that pass a UTC.
		 */
		unsigned char *buffer() {return (unsigned char *)&_centiseconds;}

		/**
		 * Pointer to start of time in memory
		 * This is used for calls to the OS that pass a UTC.
		 */
		unsigned char *buffer() const {return (unsigned char *)&_centiseconds;}

	protected:
		/**
		 * Number of centiseconds since Midnight Jan 1st 1900
		 */
		long long _centiseconds;
	};

	class Path;


	/**
	 * Class to hold the catalogue information for a file.
	 */
	class PathInfo
	{
	public:
		PathInfo();
		PathInfo(const PathInfo &other);

		PathInfo &operator=(const PathInfo &other);
		bool operator==(const PathInfo &other);
		bool operator!=(const PathInfo &other);

		bool read(const Path &path);
		bool read_raw(const Path &path, bool calc_file_type);


		/*! Type of an object */
		enum ObjectType
		{
			NOT_FOUND, /*!< Path does not exist on disc */
			FILE,	   /*!< Path is a file */
			DIRECTORY, /*!< Path is a directory */
			IMAGE_FILE /*!< Path is an image file */
		};

        /*! Attributes of an object. 0 or more of these will be combined using the | operation. */
        enum Attribute
        {
           OWNER_READ = 0x1,   /*! Read access for owner */
           OWNER_WRITE = 0x2,  /*! Write access for owner */
           OWNER_LOCKED = 0x8, /*! Object locked against owner deletion */
           OTHER_READ = 0x10,  /*! Read access for others */
           OTHER_WRITE = 0x20, /*! Write access for others */
           OTHER_LOCKED = 0x80 /*! Object locked against others deletion */
        };

		/**
		 * Get the leaf name of the object the information if for
		 *
		 * @returns leaf name of the object
		 */
		const std::string &name() const	{return _name;}

		// Object type
		ObjectType object_type() const;
		/*! Returns true if object exists on the file system */
		bool exists() const			{return (_object_type != NOT_FOUND);}
		/*! Returns true if object is a file on the file system */
		bool file() const			{return (_object_type == FILE);}
		/*! Returns true if object is a directory on the file system */
		bool directory() const	{return (_object_type == DIRECTORY);}
		/*! Returns true if object is am image file on the file system */
		bool image_file() const	{return (_object_type == IMAGE_FILE);}

		// File type format
		bool has_file_type() const;
		int file_type() const;
		int raw_file_type() const;
		UTCTime modified_time() const;

		// Load/Executable format
		bool has_load_address() const;
		unsigned int load_address() const;
		unsigned int exec_address() const;

		// All formats
		int length() const;
		int attributes() const;

		/**
		 * Iterator used to iterate through a directory
		 *
		 * use the tbx::PathInfo::begin and tbx::PathInfo::end methods
		 * to return this iterator
		 */
		class Iterator
		{
		protected:
			Iterator(const std::string &dirName, const char *wildCard);
			friend class PathInfo;

		public:
			Iterator();
			~Iterator();

			Iterator(const Iterator &other);
			Iterator &operator=(const Iterator &other);
			bool operator==(const Iterator &other);
			bool operator!=(const Iterator &other);

			Iterator &operator++();
			Iterator operator++(int);

			PathInfo &operator*();
			PathInfo *operator->();

			void next();

		// Variables
		protected:
			PathInfo *_info; /*!< Information on item iterator is pointing to */

			/**
			 * Low level class to deal with the file iteration kernel calls
			 */
			class IterBlock
			{
			public:
				IterBlock(const std::string &dirName, const char *wildCard);
				~IterBlock()	{delete _dirName; delete _wildCard;}

				bool next();
				/**
				 * Return next record from iteration block
				 */
				const char *next_record() const	{return _nextRecord;}
				bool info(PathInfo &info);

				/**
				 * Increase reference count on this block
				 */
				void add_ref() {_ref++;}
				/**
				 * Decrease reference count on this block.
				 *
				 * If the reference count reaches zero it will be deleted
				 */
				void release() {if (--_ref == 0) delete this;}

				// Variables
				int _ref; 				/*!< Reference count */
				_kernel_swi_regs _regs;	/*!< registers for swi calls */
				char *_dirName;       	/*!< Directory name */
				char *_wildCard;		/*!< Wild card for searching */
				enum {_readSize = 2048};/*!< Size of data to read with swi call */
				char _readData[_readSize];/*!< Buffer for read data */
				int _toRead;			  /*!< Bytes left to read */
				char *_nextRecord;        /*!< Next record in buffer */

			} *_iterBlock;
		};

		friend class Iterator::IterBlock;

		static PathInfo::Iterator begin(const Path &path, const std::string &wildCard);
		static PathInfo::Iterator begin(const Path &path);
		static PathInfo::Iterator end();

	protected:
		std::string _name; /*!< Name of the file system object */
		ObjectType _object_type; /*!< Object type */
		unsigned int _load_address; /*!< Load address */
		unsigned int _exec_address; /*!< Executable address */
		int _length; /*!< Length of object */
		int _attributes; /*!< object attributes */
		int _file_type;  /*!< object file type */
	};

	/**
	 * Class to manipulate RISC OS file and directory path names.
	 */
	class Path
	{
	public:
		Path();
		Path(const std::string &name);
		Path(const char *name);
		Path(const Path &other);
		Path(const Path &other, const std::string &child);
		Path(const Path &other, const char *name);

		virtual ~Path();

		// Assignment
		Path &operator=(const Path &other);
		Path &operator=(const std::string &name);
		Path &operator=(const char *name);
		Path &set(const Path &other, const std::string &child);

		// Attributes
		/**
		 * Get file name of path
		 *
		 * @returns file name
		 */
		const std::string &name() const	{return _name;}

		operator const std::string&() const;
		operator const char *() const;

		Path child(const std::string &child) const;
		Path parent() const;

		Path &down(const std::string &child);
		Path &up();
		void leaf_name(const std::string &child);
		std::string leaf_name() const;

		PathInfo::ObjectType object_type() const;
		bool path_info(PathInfo &info) const;
		bool raw_path_info(PathInfo &info, bool calc_file_type) const;

		bool exists() const;
		bool file() const;
		bool directory() const;
		bool image_file() const;

		// File information
		int file_type() const;
		bool file_type(int type);
		static int file_type(const std::string &file_name);
		static bool file_type(const std::string &file_name, int type);
		int raw_file_type() const;

		int attributes() const;
		bool attributes(int new_attributes);


		UTCTime modified_time() const;
//TODO:		bool modified_time(const UTCTime &utcTime);

		// Creation
		void create_file(int type) const;
		void create_directory() const;

		// Deletion
		void remove() const;

		// Simple renaming
		void rename(const std::string &new_name);

		/**
		 * Enumeration to options for copy method.
		 *
		 * These flags can be combined
		 */
		enum CopyOption {
			COPY_RECURSE=1, // Recursively copy
			COPY_FORCE=2,   // Overwrite destination if it already exists
            COPY_ALLOW_PRINT = 0x100u, // Allow copy to printer
			COPY_NO_ATTRIBUTES = 0x200u, // Don't copy attributes
			COPY_STAMP = 0x400u,  // Reset date stamp
			COPY_STRUCTURE = 0x800u, // Copy directory structure but not files
			COPY_NEWER = 0x1000u, // Copy if newer then destination only
			COPY_LOOK = 0x4000u   // Check destination first
		};

		void copy(const std::string &copyto, unsigned int options = 0);
		void copy(const std::string &copyto, unsigned int options, void *buffer, unsigned int size);
		void move(const std::string &copyto, unsigned int options = 0);
		void move(const std::string &copyto, unsigned int options, void *buffer, unsigned int size);

		// Whole file loading/saving
		char *load_file(int *length = 0) const;
		void save_file(const char *data, int length, int file_type) const;

		bool set_current_directory() const;

		void canonicalise();
		static std::string canonicalise(const std::string &path);
		bool canonical_equals(const tbx::Path &compare_to) const;
		bool canonical_equals(const std::string &compare_to) const;

		static Path temporary(const char *prefix = 0);
		//Operators

		/**
		 * Iterator to step through files in a folder.
		 *
		 * This is return by the tbx::Path::begin and tbx::Path::end
		 * methods.
		 */
		class Iterator
		{
		protected:
			Iterator(const std::string &dirName, const char *wildCard);
			friend class Path;

		public:
			Iterator();
			~Iterator()	{if (_iterBlock) _iterBlock->release();}

			Iterator(const Iterator &other);
			Iterator &operator=(const Iterator &other);
			bool operator==(const Iterator &other);
			bool operator!=(const Iterator &other);

			Iterator &operator++();
			Iterator operator++(int);

			/**
			 * Get file name for current iterator
			 *
			 * @returns file name
			 */
			std::string &operator*()	{return _name;};
			/**
			 * Get file name for current iterator
			 *
			 * @returns file name
			 */
			std::string *operator->()	{return &_name;};

			void next();

		// Variables
			/**
			 * Variable for current file name
			 */
			std::string _name;

			/**
			 * Low level class to deal with the file iteration kernel calls
			 */
			class IterBlock
			{
			public:
				IterBlock(const std::string &dirName, const char *wildCard);
				~IterBlock()	{delete _dirName; delete _wildCard;}

				bool next();

				/**
				 * Get next name
				 *
				 * @returns next name found
				 */
				const char *next_name() const	{return _nextName;}

				/**
				 * Increase reference count on this block
				 */
				void add_ref() {_ref++;}
				/**
				 * Decrease reference count on this block.
				 *
				 * If the reference count reaches zero it will be deleted
				 */
				void release() {if (--_ref == 0) delete this;}

				// Variables
				int _ref; 				/*!< Reference count */
				_kernel_swi_regs _regs;	/*!< registers for swi calls */
				char *_dirName;       	/*!< Directory name */
				char *_wildCard;		/*!< Wild card for searching */
				enum {_readSize = 2048};/*!< Size of data to read with swi call */
				char _readData[_readSize];/*!< Buffer for read data */
				int _toRead;			  /*!< Bytes left to read */
				char *_nextName;        /*!< Next name in the buffer */
			} *_iterBlock;
		};

		Path::Iterator begin(const std::string &wildCard);
		Path::Iterator begin();
		Path::Iterator end();

	protected:
		/**
		 * File name this path refers to
		 */
		std::string _name;
	};

};

#endif // TBX_PATH_H
