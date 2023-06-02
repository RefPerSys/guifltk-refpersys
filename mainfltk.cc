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
int preferred_height=333, preferred_width=444;
float screen_scale= 1.0;
Fl_Window* main_window;
struct plugin_st
{
    std::string plugin_name;
    std::string plugin_base;
    void* plugin_dlh;
    int plugin_rank;
};

std::vector<plugin_st> vector_plugins;

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
    ///  --geometry | -G widthxheight, e.g. --geometry=600x500
    {
        .name=(char*)"geometry", .has_arg=required_argument, .flag=(int*)nullptr,
        .val=(char)'G'
    },
    ///  --scale | -S scale, e.g. --scale=1.6
    {
        .name=(char*)"scale", .has_arg=required_argument, .flag=(int*)nullptr,
        .val=(char)'S'
    },
    ///  --plugin | -P plugin, e.g. --plugin=foo/bar to dlopen
    ///  the plugin foo/bar.so and dlsym in it fltkrps_bar_start, a nullary
    ///  --function return true on success...
    {
        .name=(char*)"plugin", .has_arg=required_argument, .flag=(int*)nullptr,
        .val=(char)'P'
    },
    /// final sentinel
    { .name=(char*)nullptr, .has_arg=0, .flag=(int*)nullptr, .val=0 }
};


bool load_plugin(const char*plugname)
{
    char buf[256];
    memset (buf, 0, sizeof(buf));
    char basebuf[256];
    memset(basebuf, 0, sizeof(basebuf));
    if (!plugname||!plugname[0])
        return false;
    if (strlen(plugname)>=sizeof(buf)-16)
        return false;
    strcpy(buf, plugname);
    const char*plugbase = basename(buf);
    for (const char*p = plugbase; *p; p++)
        if (!isalnum(*p) && *p != '_')
            return false;
    strncpy(basebuf, plugbase, sizeof(basebuf));
    if (strlen(basebuf)==0 || strlen(basebuf) + 4 >= sizeof(basebuf)) return false;
    strcpy(buf, plugname);
    strcat(buf, ".so");
    void* dlh = dlopen(buf, RTLD_NOW | RTLD_GLOBAL);
    if (!dlh)
        {
            std::clog << progname << " failed to load plugin " << buf << ":" << dlerror() << std::endl;
            return false;
        }
    {
        typedef bool initrout_t(void);
        char inibuf[sizeof(buf)+16];
        memset (inibuf, 0, sizeof(inibuf));
        snprintf(inibuf, sizeof(inibuf), "fltkrps_%s_start", basebuf);
        void* ad = dlsym(dlh, inibuf);
        if (!ad)
            {
                std::clog << progname << " failed to dlsym " << inibuf << " in plugin " << buf << ":" << dlerror() << std::endl;
                dlclose(dlh);
                return false;
            }
        initrout_t*ini = (initrout_t*) ad;
        bool ok= (*ini)();
        if (!ok)
            {
                std::clog << progname << " failed to initialize plugin " << buf << " using " << inibuf << std::endl;
                dlclose(dlh);
                return false;
            }
    }
    plugin_st p;
    memset(&p, 0, sizeof(p));
    p.plugin_name.assign(buf) ;
    p.plugin_base.assign(basebuf);
    p.plugin_dlh = dlh;
    p.plugin_rank = vector_plugins.size();
    vector_plugins.push_back(p);
    std::clog << progname << " loaded plugin#" << p.plugin_rank << ": "<< p.plugin_name << std::endl;
    return true;
} // end load_plugin

static void
show_usage(void)
{
    std::clog << progname << " usage:" << std::endl
              << "\t --version | -V                    "
              << "\t\t# show version" << std::endl
              << "\t --geometry= | -G<width>x<height>  "
              << "\t\t# preferred window geometry" << std::endl
              << "\t --scale= | -S<scale-factor>  "
              << "\t\t# preferred scale factor" << std::endl
              << "\t --plugin= | -P<plugin-file>  "
              << "\t\t# plugin (with .so suffix)" << std::endl
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
                {
                    int sc= 0;
                    std::clog << progname << " version:" << std::endl
                              << "\t built:" << __DATE__ "@" << __TIME__ << " on " << BUILD_HOST
                              << " git: " << SHORTGIT_ID << std::endl
                              << "\t FLTK:" << Fl::api_version() << " ABI " << FL_ABI_VERSION << std::endl
                              << "\t screen count:" << Fl::screen_count();
                    sc = Fl::screen_scaling_supported();
                    if (sc==0)
                        std::clog << " no scaling";
                    else if (sc==1)
                        std::clog << " shared scaling factor";
                    else if (sc==2)
                        std::clog<< " scalable independently";
                    std::clog<<std::endl;
                };
                break;
                case 'h': /// --help
                    show_usage();
                    break;
                case 'G': /// --geometry=<width>x<height> # e.g. --geometry 400x333
                {
                    int w= -1, h= -1;
                    if (sscanf(optarg, "%d[xX]%d", &w, &h) < 2)
                        {
                            std::cerr << progname << " bad geometry " << optarg
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
                case 'S': //// --scale=<float> #e.g. --scale=1.5
                {
                    float s=1.0;
                    if (sscanf(optarg, "%f", &s) < 2)
                        {
                            std::cerr << progname << "bad scale " << optarg << std::endl;
                            exit(EXIT_FAILURE);
                        };
                    if (s<0.05) s = 0.05;
                    if (s>20.0) s = 20.0;
                    screen_scale = s;
                    std::clog << progname << ": preferred scale:" << screen_scale << std::endl;
                }
                break;
                case 'P': //// --plugin=<basepath> #e.g --plugin=$HOME/lib/myplug
                    if (!load_plugin(optarg))
                        {
                            std::clog << progname << " failed to load plugin " << optarg << std::endl;
                            exit(EXIT_FAILURE);
                        };
                    break;
                default:
                    std::clog << progname << ": with unexpected argument: " << optarg << std::endl;
                    exit(EXIT_FAILURE);
                };
        };
} // end parse_program_options

void
create_main_window(void)
{
    main_window = new Fl_Window(preferred_height, preferred_width);
    main_window->label("guifltk-refpersys");
} // end create_main_window

int main(int argc, char**argv)
{
    progname = argv[0];
    parse_program_options(argc, argv);
    fl_open_display();
    create_main_window();
    main_window->show(argc, argv);
    return Fl::run();
} // end main

