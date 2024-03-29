/****** file guifltk-refpersys/fltkrps.hh ******
 * SPDX-License-Identifier: MIT
 * © Copyright 2023 Basile Starynkevitch
 ***************************************************/

#ifndef  FLTKRPS_INCLUDED
#define FLTKRPS_INCLUDED 1

#ifndef GIT_ID
#error GIT_ID should be defined by compilation command
#endif

#ifndef SHORTGIT_ID
#error SHORTGIT_ID should be defined by compilation command
#endif

#if __cplusplus < 201412L
#error expecting C++17 standard
#endif

/// C++ standard headers
#include <map>
#include <vector>
#include <set>
#include <iostream>

/// POSIX headers
#include <getopt.h>
#include <libgen.h>
#include <dlfcn.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>
#include <elf.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>


/// from GNU libunistring
#include <unitypes.h>
#include <unistr.h>

/// FLTK headers
#include <FL/Fl.H>
#include <Fl/platform.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/names.h>

/// standard C++
#include <functional>
#include <string>

extern "C" const char*progname;
extern "C" char myhostname[];

extern "C" Fl_Window* main_window;

/* preferred dimensions for windows */
extern "C" int preferred_height, preferred_width;


/* preferred screen scale factor */
extern "C" float screen_scale;


/// handler for FIFOs to RefPerSys - in file
extern "C" void out_fd_handler(int fd, void*data);
extern "C" void cmd_fd_handler(int fd, void*data);

constexpr unsigned frps_buffer_size = 2048;

/* Return true if plugin was loaded successfully; A plugin foo/bar
   dlopen foo/bar.so and calls its function bool fltkrps_bar_start()
   for initialization, which should return true on success */

extern "C" bool load_plugin(const char*plugname);


/* Return true if the given string is unique valid RefPerSys path */
extern "C" bool set_refpersys_path(const char*path);


/** An important function from RefPerSys code file scalar_rps.cc in
 *  mid-september 2023.
 *
 * compute a long hash in ht[0] and ht[1]. Return the number of UTF-8
 * character or else 0 if cstr with len bytes is not proper UTF-8.
 *
 * This is an important function, whose source code is shared in
 * guifltk-refpersys.  It uses GNU libunistring library.
 */
extern "C" int rps_compute_cstr_two_64bits_hash(int64_t ht[2],
        const char*cstr,
        int len= -1);


#endif /* FLTKRPS_INCLUDED */
