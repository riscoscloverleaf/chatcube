/*
 * utils.h
 *
 * The utils.c file defines various functions that might otherwise
 * have been found in a library like RISC OS Lib, ie. small functions
 * based on SWI calls to RISC OS.
 * Functions extended by CloverLeaf Team
 */
 
#ifndef __utils_h
#define __utils_h

#include <time.h>
#include <vector>
#include <string>
#include <climits>
#include <sys/stat.h>
#include "oslib/toolbox.h"
#include "oslib/Toolboxtypes.h"
#include "oslib/Systypes.h"

#define FILE_TYPE_PNG 0xB60
#define FILE_TYPE_JPEG 0xC85
#define FILE_TYPE_GIF 0x695
#define FILE_TYPE_ZIP 0xa91
#define FILE_TYPE_HTML 0xfaf

#ifndef NOF_ELEMENTS
#define NOF_ELEMENTS(array) (sizeof(array)/sizeof(*(array)))
#endif

//void get_os_screen_values(void);

bool decompress(char **file, int *size);

int file_size(char *name);

_kernel_oserror * file_load(char *name, char *add) ;

void CLG(void);

void set_file_type(const char *file_name, int file_type);

char *object_name(ObjectId id);

int is_object(ObjectId id,char *name);

void warn_about_memory(void);

ObjectId named_object(char *name);

std::string lookup_token(const char *t);

void show_int_alert(int num);

void show_alert_error(const char *message);
void show_alert_info(const char *message);
void show_alert_info(const char *message, const char *title);
int show_question(const char *message, char* buttons);

std::string to_string(int num);
std::string to_string(long num);
std::string to_string(int64_t num);
std::string to_string_2digit_num(int num);
int string_to_int(std::string s);
time_t str_to_timet(const char* str);
void str_to_local_time(const char* str, const char* format, char* buf, int len);
void timet_to_local_time(const time_t *t, const char* format, char* buf, int len);

char *ltrim(char *s);
char *rtrim(char *s);
std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
std::string trim_prefix(const std::string& s, const std::string& prefix);
size_t split(const std::string &txt, std::vector<std::string> &strs, char ch);
std::string str_join(const std::vector<std::string>& vec, const char *delim);
std::string str_replace_all(std::string str, const std::string& from, const std::string& to);

//void set_yscroll_to_bottom(toolbox_o window_handler);
//void set_yscroll_to_pos(toolbox_o window_handler, int pos);
//int set_window_new_extent_height(toolbox_o window_handler, int height);
//void force_window_redraw(toolbox_o window_handler);

std::string convert_time_full_to_HM(const time_t t);
std::string convert_time_full_to_DMY(const time_t t);
std::string convert_time_full_to_string(const time_t t, const char *format);

std::string file_size_to_displayed_string(int64_t num);

//void attachMemberListToMainWindow();
//void makeActiveChatWindow();

bool is_file_exist (const std::string& name);
bool is_directory_exist (const std::string& name);
void create_directories_for_file(const std::string& file_name);
long get_filesize(const char *filename);
std::string get_file_contents(const char *filename);
void copy_file(const char *from, const char *to);

const char *filetype2ext(int file_type);
std::string file_basename_append_ext(const std::string& file_full_path);
std::string file_basename(const std::string& file_full_path);

void remove_recursive(const std::string& src);
void copy_dir(const std::string& src, const std::string& dst, const std::string& exclude);

//void wait_wimp_poll_event_finished(void);

void open_browser_url(const std::string& url);
bool can_run_file_type(int type);
#endif