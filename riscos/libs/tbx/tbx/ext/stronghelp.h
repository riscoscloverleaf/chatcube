/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2014 Alan Buckley   All Rights Reserved.
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

#ifndef TBX_EXT_STRONGHELP_H_
#define TBX_EXT_STRONGHELP_H_

#include <string>

namespace tbx {
namespace ext {

class StrongHelpNotFoundHandler;

  /**
  * class to show help from StrongHelp help files
  *
  * For the help files to work the !StrongHlp application must have
  * been seen by the filer
  */
  class StrongHelp
  {
    std::string _path;
    static std::string _default_path;

  public:
    StrongHelp();
    StrongHelp(const std::string &path);
    StrongHelp(const StrongHelp &other);

    static void default_path(const std::string &path);
    static void standard_default_path();
    /**
     * Get the default path set for the application
     */
    static const std::string &default_path() {return _default_path;}
    /**
     * Get the path used by this instance
     */
    const std::string &path() const {return _path;}

    static bool run_stronghelp();
    bool run_path();

    void lookup(const std::string &page, StrongHelpNotFoundHandler *handler= 0);
    void search(const std::string &text, StrongHelpNotFoundHandler *handler= 0);

  };

  /**
  * interface to customise handling of StrongHelp or lookup not
  * found message.
  */
  class StrongHelpNotFoundHandler
  {
    public:
    virtual ~StrongHelpNotFoundHandler() {};
    virtual bool stronghelp_not_found(StrongHelp &help, const std::string &page_or_text);
    virtual void stronghelp_not_running(StrongHelp &help, const std::string &page_or_text);
  };
}
}

#endif /* TBX_EXT_STRONGHELP_H_ */
