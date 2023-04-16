/**** file guifltk-refpersys/mainfltk.cc ******
 ****  SPDX-License-Identifier: MIT ******
 *
 * © Copyright 2023 The  Reflective Persistent System Team
 * team@refpersys.org &   http://refpersys.org/
 *
 * contributors: Basile Starynkevitch <basile@starynkevitch.net>
 *
 **********************************************/

#include "fltkrps.hh"

const char*progname;
int preferred_height, preferred_width;

static const struct option long_options[] =
{
    //// --version | -V
    {
        .name=(char*)"version", .has_arg=no_argument, .flag=(int*)nullptr,
        .val=(char)'V'
    },
    ///  --help | -h
    {
        .name=(char*)"help", .has_arg=no_argument, .flag=(int*)nullptr,
        .val=(char)'h'
    },
    ///  --geometry | -G widthxheight, e.g. --geometry 600x500
    {
        .name=(char*)"geometry", .has_arg=required_argument, .flag=(int*)nullptr,
        .val=(char)'G'
    },
    /// final sentinel
    { .name=(char*)nullptr, .has_arg=0, .flag=(int*)nullptr, .val=0 }
};

static void
show_usage(void)
{
    std::clog << progname << " usage:" << std::endl
              << "\t --version | -V                    "
              << "\t\t# show version" << std::endl
              << "\t --geometry= | -G<width>x<height>  "
              << "\t\t# preferred window geometry" << std::endl
              << "\t --help | -h                       "
              << "\t\t# give this help" << std::endl
              ;
} // end show_usage

static void
parse_program_options (int argc, char*const*argv)
{
    int op= -1;
    int ix;
    while ((ix= -1), //
            (op = getopt_long(argc, argv,
                              "Vh", //short option string
                              long_options,
                              &ix)),
            (op>=0))
        {
            switch (op)
                {
                case 'V':			// --version
                    std::clog << progname << " version:" << std::endl
                              << "\t built:" << __DATE__ "@" << __TIME__ << " on " << BUILD_HOST
                              << " git: " << SHORTGIT_ID << std::endl
                              << "\t FLTK:" << Fl::api_version() << " ABI " << FL_ABI_VERSION << std::endl;
                    break;
                case 'h': /// --help
                    show_usage();
                    break;
                case 'G': /// --geometry=<width>x<height> # e.g. --geometry 400x333
                {
                    int w= -1, h= -1;
                    if (sscanf(optarg, "%d[xX]%d", &w, &h) < 2)
                        {
                            std::cerr << progname << "bad geometry " << optarg
                                      << " expecting <width>x<height> e.g. 400x333"
                                      << std::endl;
                            exit(EXIT_FAILURE);
                        };
                    if (w<100) w=100;
                    if (h<64) h=64;
                    if (w>2048) w=2048;
                    if (h>2048) h=2048;
                    preferred_width = w;
                    preferred_height = h;
                    std::clog << progname << ": preferred window geometry: width=" << preferred_width << ", height=" << preferred_height << "." << std::endl;
                }
                break;
                };
        };
#warning incomplete parse_program_options should use getopt
} // end parse_program_options

int main(int argc, char**argv)
{
    progname = argv[0];
    parse_program_options(argc, argv);
} // end main

