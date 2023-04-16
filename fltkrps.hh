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
/// FLTK headers
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

extern "C" const char*progname;
#endif /* FLTKRPS_INCLUDED */
