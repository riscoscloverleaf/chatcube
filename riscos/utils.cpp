#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <list>
#include <cstdio>
#include <cerrno>
#include <tbx/sprite.h>
#include <tbx/application.h>
#include <tbx/uri.h>
#include <tbx/path.h>
#include "oslib/os.h"
#include "oslib/squash.h"
#include "oslib/toolbox.h"
#include "oslib/window.h"
#include "oslib/wimp.h"
#include "oslib/Wimptypes.h"
#include "oslib/Event.h"
#include "oslib/messagetrans.h"
#include "oslib/osfile.h"
#include "oslib/jpeg.h"
#include "oslib/osspriteop.h"
#include "oslib/wimpspriteop.h"
#include "swis.h"

#include "utils.h"
#include "global.h"
#include "cloverleaf/CLException.h"
#include "cloverleaf/Logger.h"


time_t local_tzoffset = 0x7fffffff;


static unsigned int contents_of(char *a)
{
   unsigned char *ad = (unsigned char *) a;
   unsigned int v = (((*(ad+3)) <<24) +
          ((*(ad+2)) <<16) +
          ((*(ad+1)) <<8 ) + *ad);

   return v;
}

#define SQUASH  0x48535153                /* SQSH */

/* to save space we allow compressed draw files */


bool decompress(char **file, int *size)
{
   if (contents_of(*file) == SQUASH) {
      /* it's squashed */

      char *wkspc=0;
      int ss,q;
      char *s;
      q = contents_of((*file)+4);        /* size */
      s = (char *)malloc(q);
      if (!s) return false;

      if (_swix(Squash_Decompress,_IN(0) | _IN(1)  | _OUT(0),
              8,(*size)-20,&ss)) return false;

      wkspc = (char *)malloc(ss);
      if ((!wkspc) || (_swix(Squash_Decompress,_IN(0) | _IN(1) |_IN(2) |
                            _IN(3) |_IN(4) |_IN(5),
                            4,wkspc,(*file)+20,(*size)-20,s,q))) {
          free(s);
          if (wkspc) free (wkspc);
          return false;
      }
      free(wkspc);
      free(*file);

      *file =s;
      *size = q;

   }
   return true;
}

int file_size(char *name)
{
  _kernel_swi_regs regs;
  regs.r[0] = 17;
  regs.r[1] = (int) name;

  if(_kernel_swi(OS_File,&regs,&regs)) regs.r[4] = 0;

  return regs.r[4];
}


_kernel_oserror * file_load(char *name, char *add)
{
  _kernel_swi_regs regs;
  regs.r[0] = 16;
  regs.r[1] = (int) name;
  regs.r[2] = (int) add;
  regs.r[3] = 0;

  return (_kernel_swi(OS_File,&regs,&regs));
}

void CLG(void)
{
   _swix(0x110,0);
}

void set_file_type(const char *file_name, int file_type)
{
   _swix(OS_File, _INR(0,2), 18, file_name, file_type);
}


static char obj__name[16];

char *object_name(ObjectId id)
{
    if(xtoolbox_get_template_name(0,id,obj__name,16,0)) return NULL;
    return obj__name;
}

int is_object(ObjectId id,char *name)
{
    if(xtoolbox_get_template_name(0,id,obj__name,16,0)) return 1;
    return ((int) strcmp(obj__name,name));

}

std::string lookup_token(const char *tag)
{
    return tbx::Application::instance()->messages().message(tag);
}

static wimp_error_box_selection error_wimp_os_report(const char* message, wimp_error_box_flags type,
                                                     wimp_error_box_flags buttons, char *custom_buttons)
{
    os_error e;
    e.errnum = 255;
    strncpy(e.errmess, message, os_ERROR_LIMIT);
    e.errmess[os_ERROR_LIMIT - 1] = '\0';
    wimp_error_box_selection        click;
    wimp_error_box_flags            flags;

    if (custom_buttons != NULL && *custom_buttons != '\0') {
        flags = wimp_ERROR_BOX_GIVEN_CATEGORY |
                (type << wimp_ERROR_BOX_CATEGORY_SHIFT);
        click = wimp_report_error_by_category(&e, flags, "ChatCube", "",
                                              NULL, custom_buttons);
    } else {
        flags = wimp_ERROR_BOX_GIVEN_CATEGORY | buttons |
                (type << wimp_ERROR_BOX_CATEGORY_SHIFT);
        click = wimp_report_error_by_category(&e, flags, "ChatCube", "",
                                              NULL, NULL);
    }

    return click;
}

void show_int_alert(int num)
{
  char numstr[10];
  sprintf(numstr, "%d", num);
    show_alert_error(numstr);
}

void show_alert_error(const char *message)
{
   os_error e;

   strcpy(e.errmess, message);
   wimp_report_error(&e,0,0);
}

void show_alert_info(const char *message)
{
    error_wimp_os_report(message, wimp_ERROR_BOX_CATEGORY_INFO, wimp_ERROR_BOX_OK_ICON, NULL);
}

int show_question(const char *message, char* buttons)
{
    return error_wimp_os_report(message, wimp_ERROR_BOX_CATEGORY_QUESTION, wimp_ERROR_BOX_OK_ICON | wimp_ERROR_BOX_CANCEL_ICON, buttons);
}

std::string to_string(int num)
{
  char numstr[10];
  sprintf(numstr, "%d", num);
  return numstr;
}

std::string to_string(long num)
{
    char numstr[10];
    sprintf(numstr, "%ld", num);
    return numstr;
}

std::string to_string(int64_t num)
{
    char numstr[22];
    sprintf(numstr, "%lld", num);
    return numstr;
}

std::string to_string_2digit_num(int num)
{
  char numstr[2];
  sprintf(numstr, "%02d", num);
  return numstr;
}

int string_to_int(std::string s)
{
    int res = 0;
    sscanf(s.c_str(), "%d", &res);    
    return res;
}

size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find( ch );
    size_t initialPos = 0;
    strs.clear();
 
    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;
 
        pos = txt.find( ch, initialPos );
    }
 
    // Add the last one
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );
 
    return strs.size();
}

std::string str_join(const std::vector<std::string>& vec, const char *delim) {
    int i = 0;
    std::string result;
    for(const auto &item : vec) {
        if (i != 0) {
            result.append(delim);
        }
        result.append(item);
        i++;
    }
    return result;
}

time_t str_to_timet(const char* str)
{
    int tm_ms, tz_h, tz_m, items, tm_gmtoff = 0;
    char tz_sign;
    tm _t;
//    Logger::debug("str_to_local_time %s", str);
    items = sscanf(str, "%4d-%2d-%2dT%2d:%2d:%2d.%dZ", &_t.tm_year, &_t.tm_mon, &_t.tm_mday, &_t.tm_hour, &_t.tm_min, &_t.tm_sec,
                   &tm_ms);
//    Logger::debug("str_to_local_time1 %s items=%d", str, items);
    if (items != 7) {
        items = sscanf(str, "%4d-%2d-%2dT%2d:%2d:%2d.%d%c%2d:%2d", &_t.tm_year, &_t.tm_mon, &_t.tm_mday, &_t.tm_hour, &_t.tm_min, &_t.tm_sec,
                       &tm_ms, &tz_sign, &tz_h, &tz_m);
//        Logger::debug("str_to_local_time2 %s items=%d", str, items);
        if (items != 10) {
            Logger::error("str_to_local_time() incorrect time: %s", str);
            return 0;
        }
        tm_gmtoff = 3600*tz_h + 60*tz_m;
        if (tz_sign == '-') {
            tm_gmtoff = -tm_gmtoff;
        }
    }
    _t.tm_mon -= 1;
    _t.tm_year -= 1900;
    _t.tm_gmtoff = 0;
    if (local_tzoffset == 0x7fffffff) {
        tm _t2;
        _t2.tm_year = 70;
        _t2.tm_mon = 0;
        _t2.tm_mday = 1;
        _t2.tm_hour = 0;
        _t2.tm_min = 0;
        _t2.tm_sec = 0;
        _t2.tm_gmtoff = 0;
        local_tzoffset = mktime(&_t2);
//        tm *tm1 = localtime(&local_tzoffset);
//        Logger::debug("str_to_local_time2 localoffset %d %d %d",  tm1->tm_hour, tm1->tm_min, tm1->tm_gmtoff);
    }
    time_t result = mktime(&_t) - local_tzoffset - tm_gmtoff;
//    Logger::debug("str_to_local_time2 mktime %s res=%d localoff=%d timezone=%d gmoff=%d", str, result, local_tzoffset, timezone, tm_gmtoff);
    return result;
}

void str_to_local_time(const char* str, const char* format, char* buf, int len)
{
    time_t t = str_to_timet(str);
    tm *lt = localtime(&t);
    strftime(buf, len, format, lt);
}

void timet_to_local_time(const time_t *t, const char* format, char* buf, int len)
{
    tm *lt = localtime(t);
    strftime(buf, len, format, lt);
}

std::string convert_time_full_to_HM(const time_t t) {
    char buf[200];
    timet_to_local_time(&t, "%H:%M", buf, sizeof(buf));
//    Logger::debug("convert_time_full_to_HM=%s", buf);
    return std::string(buf);
}

std::string convert_time_full_to_DMY(const time_t t) {
    char buf[200];
    timet_to_local_time(&t, "%d.%m.%Y", buf, sizeof(buf));
    return std::string(buf);
}

std::string convert_time_full_to_string(const time_t t, const char *format) {
    char buf[200];
    timet_to_local_time(&t, format, buf, sizeof(buf));
    return std::string(buf);
}

std::string file_size_to_displayed_string(int64_t num) {
    std::string sizes[] = {"B", "KB", "MB", "GB", "TB" };
    int64_t len = num;
    int order = 0;
    while (len >= 10000 && order < sizeof(sizes)) {
        order++;
        len = len/1000;
    }

    char buffer[50];
    sprintf(buffer, "Size: %lld %s", len, sizes[order].c_str());
    return std::string(buffer);    
}

bool is_file_exist (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0); 
}

bool is_directory_exist (const std::string& name) {
  struct stat buffer;   
  int res = stat (name.c_str(), &buffer);
  return (res == 0 && (buffer.st_mode & S_IFDIR)); 
}

long get_filesize(const char *filename) {
  struct stat buffer;   
  int res = stat (filename, &buffer);
  return (res == 0 ? buffer.st_size : -1);     
}

void create_directories_for_file(const std::string& file_name) {
    std::string dirname;
    std::list<std::string> create_dirs;
    for(size_t i = file_name.size()-1; i > 0; i--) {
        if (file_name[i] == '.') {
            dirname = file_name.substr(0, i);
            if (is_directory_exist(dirname)) {
                //Logger::debug("dirname exists=%s", dirname.c_str());
                break;
            }
            create_dirs.push_back(dirname);
        }
    }
    while(!create_dirs.empty()) {
        //Logger::debug("create dir=%s", create_dirs.back().c_str());
        mkdir(create_dirs.back().c_str(), 0777);
        create_dirs.pop_back();
    }
}

struct _riscos_file_type_struct {
    int type;
    const char* ext;
};
_riscos_file_type_struct _riscos_file_types[] = {
        {0xFFF, "txt"},
        {0xFFD, "dat"},
        {0xFF0, "tiff"},
        {0xFD9, "exe"},
        {0xFD8, "com"},
        {0xFC2, "aiff"},
        {0xFB2, "avi"},
        {0xFB1, "wav"},
        {0xFAF, "html"},
        {0xF98, "psd"},
        {0xF89, "gz"},
        {0xF83, "mng"},
        {0xF81, "js"},
        {0xF80, "xml"},
        {0xF79, "css"},
        {0xF78, "jng"},
        {0xDFE, "csv"},
        {0xDF6, "iso"},
        {0xDEA, "dxf"},
        {0xDDC, "zip"},
        {0xDB0, "wk1"},
        {0xD6D, "rle"},
        {0xCFF, "cff"},
        {0xCE5, "tex"},
        {0xCE4, "dvi"},
        {0xCB6, "mod"},
        {0xC85, "jpg"},
        {0xC46, "tar"},
        {0xC32, "rtf"},
        {0xBF8, "mpg"},
        {0xBA6, "xls"},
        {0xB61, "xbm"},
        {0xB60, "png"},
        {0xB2F, "wmf"},
        {0xADF, "pdf"},
        {0xAE4, "jar"},
        {0xABF, "cab"},
        {0xAAD, "svg"},
        {0xAA7, "m3u"},
        {0xA91, "zip"},
        {0xA8F, "ac3"},
        {0xA8D, "vob"},
        {0xA7F, "xlsx"},
        {0xA7E, "docx"},
        {0x808, "eps"},
        {0x77F, "ttf"},
        {0x69E, "ppm"},
        {0x69C, "bmp"},
        {0x697, "pcx"},
        {0x695, "gif"},
        {0x1CF, "flac"},
        {0x1AD, "mp3"},
        {0x1A8, "ogg"},
        {0x071, "avi"}
};
#define _riscos_file_types_len (sizeof _riscos_file_types / sizeof _riscos_file_types[0])

const char* filetype2ext(int file_type) {
    unsigned int i;
    _riscos_file_type_struct *t;

    for(i = 0; i < _riscos_file_types_len; i++) {
        t = &_riscos_file_types[i];
        //Logger::debug("Filetype %d %d->%s", file_type, t->type, t->ext);
        if (t->type == 0) {
            return NULL;
        }
        if (t->type == file_type) {
            //Logger::debug("Filetype %d->%s", file_type, t->ext);
            return t->ext;
        }
    }
    
    return NULL;
}

std::string file_basename_append_ext(const std::string& file_full_path) {
    const char *ext;
    std::string file_name;
    bits load, exec, attr;
    int size, file_type;
    file_name = std::string(basename(
            str_replace_all(str_replace_all(str_replace_all(file_full_path, "/", "!@!"), ".", "/"), "!@!", ".").c_str()));
    osfile_read_stamped_no_path(file_full_path.c_str(), &load, &exec, &size, &attr, (bits *) &file_type);
    if (file_type) {
        ext = filetype2ext(file_type);
        if (ext) {
            std::string ext_str = "." + std::string(ext);
            if (file_name.size() >= ext_str.size() && file_name.compare(file_name.size() - ext_str.size(), ext_str.size(), ext_str) == 0) {
                Logger::error("dont append ext, filename already has it [%s]", file_name.c_str());
                return file_name;
            } else {
                file_name.append(".");
                file_name.append(ext);
                Logger::debug("append ext %s", file_name.c_str());
                return file_name;
            }
        } else {
            file_name.append(",");
            file_name.append(to_string(file_type));
            Logger::debug("append file_type %s", file_name.c_str());
            return file_name;
        }
    }
    return file_name;
}

std::string file_basename(const std::string& file_full_path) {
    return std::string(basename(
            str_replace_all(str_replace_all(str_replace_all(file_full_path, "/", "!@!"), ".", "/"), "!@!", ".").c_str()));
}

std::string get_file_contents(const char *filename)
{
    std::FILE *fp = std::fopen(filename, "rb");
    if (fp)
    {
        std::string contents;
        std::fseek(fp, 0, SEEK_END);
        contents.resize(std::ftell(fp));
        std::rewind(fp);
        std::fread(&contents[0], 1, contents.size(), fp);
        std::fclose(fp);
        return(contents);
    }
    throw_exception(std::string("Error open/create file. ") + strerror(errno));
}

void copy_file(const char *from, const char *to)
{
    std::FILE *fpfrom = std::fopen(from, "rb");
    if (!fpfrom) {
        throw_exception(std::string("Error open/create file ") + from + " " + strerror(errno));
    }
    std::FILE *fpto = std::fopen(to, "w+b");
    if (!fpto) {
        throw_exception(std::string("Error open/create file ") + to + " " + strerror(errno));
    }
    int readed;
    char buf[16384];
    if (fpfrom && fpto) {
        do {
            readed = std::fread(buf, 1, 16384, fpfrom);
            std::fwrite(buf, 1, readed, fpto);
        } while (readed == 16384);
        fclose(fpfrom);
        fclose(fpto);
    }
}

// string
char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

std::string& ltrim(std::string& str, const std::string& chars)
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

std::string& rtrim(std::string& str, const std::string& chars)
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

std::string& trim(std::string& str, const std::string& chars)
{
    return ltrim(rtrim(str, chars), chars);
}

std::string trim_prefix(const std::string& s, const std::string& prefix)
{
    return s.find(prefix) == 0 ? s.substr(prefix.length()) : s;
}

std::string str_replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

void open_browser_url(const std::string& url) {
    Logger::info("Open URL: %s", url.c_str());
    if (!tbx::URI::dispatch(url)) {
        tbx::Application *app = tbx::Application::instance();
        if (url.substr(0, 5) == "https") {
            app->start_wimp_task("URLOpen_https "+ url);
        } else {
            app->start_wimp_task("URLOpen_http "+ url);
        }
    }
}

void copy_dir_exclude(std::string src, std::string dst, char* exclude) {
    tbx::Path src_path = tbx::Path(src);
    for(auto file = src_path.begin(); file != src_path.end(); file++) {
        if (*file == exclude) {
            continue;
        }
        tbx::Path src_item = tbx::Path(src, *file);
        src_item.copy(dst + "." + *file, tbx::Path::CopyOption::COPY_RECURSE | tbx::Path::CopyOption::COPY_FORCE);
        Logger::debug("_copydir_exclude File: %s", file->c_str());
    }
}

void remove_recursive(const std::string& src) {
    tbx::Path src_path = tbx::Path(src);
    if (src_path.directory()) {
        for(auto leaf = src_path.begin(); leaf != src_path.end(); leaf++) {
            auto leaf_path = tbx::Path(src, *leaf);
            if (leaf_path.directory()) {
                Logger::debug("remove_recursive Dir: %s", leaf_path.name().c_str());
                remove_recursive(leaf_path.name());
            } else {
                leaf_path.remove();
                Logger::debug("remove_recursive File: %s", leaf_path.name().c_str());
            }
        }
    }
    src_path.remove();
}

void copy_dir(const std::string& src, const std::string& dst, const std::string& exclude) {
    tbx::Path src_path = tbx::Path(src);
    mkdir(dst.c_str(), 0777);
    for(auto file = src_path.begin(); file != src_path.end(); file++) {
        if (*file == exclude) {
            continue;
        }
        tbx::Path src_item = tbx::Path(src, *file);
        src_item.copy(dst + "." + *file, tbx::Path::CopyOption::COPY_RECURSE | tbx::Path::CopyOption::COPY_FORCE);
        Logger::debug("_copy_dir File: %s", file->c_str());
    }
}

bool can_run_file_type(int type) {
    char varname[200];
    os_var_type out;
    int used;
    sprintf(varname, "Alias$@RunType_%X", type);
    os_read_var_val_size(varname, 0, 0, &used, &out);
//    Logger::debug("can_run_file_type var=%s type=%X user=%d", varname, type, used);
    return (used != 0);
}