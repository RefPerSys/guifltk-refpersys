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

/// FLTK headers
#include <FL/Fl.H>
#include <Fl/platform.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

extern "C" const char*progname;
extern "C" char myhostname[];

extern "C" Fl_Window* main_window;

/* preferred dimensions for windows */
extern "C" int preferred_height, preferred_width;


/* preferred screen scale factor */
extern "C" float screen_scale;


/* Return true if plugin was loaded successfully; A plugin foo/bar
   dlopen foo/bar.so and calls its function bool fltkrps_bar_start()
   for initialization, which should return true on success */

extern "C" bool load_plugin(const char*plugname);

#endif /* FLTKRPS_INCLUDED */
