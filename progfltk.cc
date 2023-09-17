/**** file guifltk-refpersys/progfltk.cc ******
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
char myhostname[80];
std::string my_window_title="GUI-Fltk RefPerSys";
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
    ///  --dimension | -D widthxheight, e.g. --dimension=600x500
    {
        .name=(char*)"dimension", .has_arg=required_argument, .flag=(int*)nullptr,
        .val=(char)'D'
    },
    ///  --refpersys | -r dir, e.g. --refpersys=/home/john/RefPerSys
    {
        .name=(char*)"refpersys", .has_arg=required_argument, .flag=(int*)nullptr,
        .val=(char)'r'
    },
    ///  --scale | -S scale, e.g. --scale=1.6
    {
        .name=(char*)"scale", .has_arg=required_argument, .flag=(int*)nullptr,
        .val=(char)'S'
    },
    ///  --hashstr | -H string, e.g. --hashstr='abcde'
    {
        .name=(char*)"hashstr", .has_arg=required_argument, .flag=(int*)nullptr,
        .val=(char)'H'
    },
    ///  --title | -T title-string, e.g. --title='Fltk RefPerSys for John'
    {
        .name=(char*)"title", .has_arg=required_argument, .flag=(int*)nullptr,
        .val=(char)'T'
    },
    ///  --plugin | -P plugin, e.g. --plugin=foo/bar to dlopen
    ///  the plugin foo/bar.so and dlsym in it fltkrps_bar_start, a nullary
    ///  function return true on success...
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
              << "\t --dimension= | -D <width>x<height>  "
              << "\t\t# preferred window dimension" << std::endl
              << "\t --refpersys= | -r <directory> " << std::endl
              << "\t\t# RefPerSys directory, containing source, executable, persistore" << std::endl
              << "\t --scale= | -S<scale-factor>  "
              << "\t\t# preferred scale factor" << std::endl
              << "\t --plugin= | -P<plugin-file>  "
              << "\t\t# plugin (with .so suffix)" << std::endl
              << "\t --title= | -T<title>  "
              << "\t\t# title of the window" << std::endl
              << "\t --hashstr | -H<string>  "
              << "\t\t# compute and give on stdout the hash of the string"
              << std::endl
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
                              "VhD:P:S:r:T:", //short option string
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
                case 'H':
                {
                    std::clog << progname << " hashing string:" << std::endl
                              << optarg << std::endl;
                    int64_t ht[2] = {0,0};
                    if (rps_compute_cstr_two_64bits_hash(ht, optarg, -1))
                        {
                            char buf[128];
                            memset(buf, 0, sizeof(buf));
                            snprintf(buf, sizeof(buf),
                                     "h0=%ld=%#lx h1=%ld=%#lx",
                                     ht[0], ht[0], ht[1], ht[1]);
                            std::clog << buf << std::endl;
                        }
                    else
                        {
                            std::cerr << progname << " failed to rps_compute_cstr_two_64bits_hash on " << optarg << std::endl;
                            exit (EXIT_FAILURE);
                        };
                    std::string optstr(optarg);
                    std::clog << std::hash<std::string> {}(optarg) << std::endl;
                }
                break;
                case 'r': /// --refpersys <directory>
                {
                    if (!set_refpersys_path(optarg))
                        {
                            std::cerr << progname << " invalid RefPerSys path: " << optarg << std::endl;
                            exit (EXIT_FAILURE);
                        }
                }
                break;
                case 'D': /// --dimension=<width>x<height> # e.g. --geometry 400x333
                {
                    int w= -1, h= -1;
                    errno = 0;
                    int sdim= sscanf(optarg, "%d[xX]%d", &w, &h);
                    if (sdim < 2)
                        {
                            std::cerr << progname << " bad dimension " << optarg
                                      << " expecting <width>x<height> e.g. 400x333"
                                      << "(sdim=" << sdim
                                      << " " << strerror(errno) << ")" << std::endl;
                            exit(EXIT_FAILURE);
                        };
                    if (w<100) w=100;
                    if (h<64) h=64;
                    if (w>2048) w=2048;
                    if (h>2048) h=2048;
                    preferred_width = w;
                    preferred_height = h;
                    std::clog << progname << ": preferred window dimension:"
                              << std::endl << "\t width=" << preferred_width
                              << ", height=" << preferred_height
                              << " ..." << std::endl;
                }
                break;
                case 'S': //// --scale=<float> #e.g. --scale=1.5
                {
                    float s=1.0;
                    errno = 0;
                    if (sscanf(optarg, "%f", &s) < 1)
                        {
                            std::cerr << progname << "bad scale " << optarg
                                      << " " << strerror(errno) << std::endl;
                            exit(EXIT_FAILURE);
                        };
                    if (s<0.05) s = 0.05;
                    if (s>20.0) s = 20.0;
                    screen_scale = s;
                    std::clog << progname << ": preferred scale:" << screen_scale << std::endl;
                }
                break;
                case 'T': //// --title=<stringS #e.g. --title='RefPerSys for John'
                {
                    my_window_title.assign(optarg);
                };
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
    main_window->label(my_window_title.c_str());
} // end create_main_window


bool
set_refpersys_path(const char*path)
{
    /// in comments path is supposed to be ~/RefPerSys/
    static bool alreadycalled;
    if (alreadycalled)
        {
            std::cerr << progname << " cannot set RefPerSys path twice, here to " << path << std::endl;
            return false;
        };
    alreadycalled = true;
    struct stat srefp;
    memset(&srefp, 0, sizeof(srefp));
    if (stat(path, &srefp))
        {
            std::cerr << progname << " fails to stat RefPerSys path " << path << " : " << strerror(errno) << "." << std::endl;
            return false;
        };
    if (!srefp.st_mode & S_IFMT != S_IFDIR)
        {
            std::cerr << progname << " given RefPerSys path " << path << " is not a directory." << std::endl;
            return false;
        };
    std::string pathstr(path);
    /// check presence of ~/RefPerSys/refpersys executable
    {
        std::string exepath=pathstr + "/refpersys";
        memset(&srefp, 0, sizeof(srefp));
        if (stat(exepath.c_str(), &srefp))
            {
                std::cerr << progname << "  RefPerSys path " << path << " without program " << exepath << ":" << strerror(errno) << "." << std::endl;
                return false;
            };
        if (!srefp.st_mode & S_IFMT != S_IFREG)
            {
                std::cerr << progname << " given RefPerSys path " << path << " with non-file " << exepath << " ..." << std::endl;
                return false;
            };
        if (!srefp.st_mode & S_IEXEC)
            {
                std::cerr << progname << " given RefPerSys path " << path << " with non-executable " << exepath << " ..." << std::endl;
                return false;
            };
        /// executable ~/RefPerSys/refpersys should be an ELF binary
        {
            FILE* fexe = fopen(exepath.c_str(), "rb");
            if (!fexe)
                {
                    std::cerr << progname << " given RefPerSys path " << path << " has unreadable executable " << exepath  << ":" << strerror(errno) << "." << std::endl;
                    return false;
                };
            Elf64_Ehdr elfhead;
            memset (&elfhead, 0, sizeof(elfhead));
            if (fread(&elfhead, sizeof(elfhead), 1, fexe) != 1)
                {
                    std::cerr << progname << " given RefPerSys path "
                              << path << " has unreadable ELF executable "
                              << exepath  << ":" << strerror(errno) << "." << std::endl;
                    fclose(fexe);
                    return false;
                };
            if (elfhead.e_ident[EI_MAG0] != ELFMAG0
                    || elfhead.e_ident[EI_MAG1] != ELFMAG1
                    || elfhead.e_ident[EI_MAG2] != ELFMAG2
                    || elfhead.e_ident[EI_MAG3] != ELFMAG3
                    || elfhead.e_ident[EI_CLASS] != ELFCLASS64
                    || (elfhead.e_type != ET_EXEC && elfhead.e_type != ET_DYN))
                {
                    std::cerr << progname << " given RefPerSys path " << path
                              << " has bad ELF executable " << exepath
                              << "." << std::endl;
                    fclose(fexe);
                    return false;
                }
        }
    }
    /// check presence of ~/RefPerSys/refpersys.hh header
    {
        std::string headerpath=pathstr + "/refpersys.hh";
        FILE*fhead = fopen(headerpath.c_str(), "r");
        if (!fhead)
            {
                std::cerr << progname << "  RefPerSys path " << path << " without header " << headerpath << ":" << strerror(errno) << "." << std::endl;
                return false;
            };
        char linbuf[64];
        memset(linbuf, 0, sizeof(linbuf));
        if (!fgets(linbuf, sizeof(linbuf), fhead))
            {
                std::cerr << progname << "  RefPerSys path " << path << " with unreadable header " << headerpath << ":" << strerror(errno) << "." << std::endl;
                fclose(fhead);
                return false;
            };
        if (linbuf[0] != '/' || linbuf[1] != '*' || linbuf[2] != '*' || linbuf[3] != '*' || linbuf[4] != '*')
            {
                std::cerr << progname << "  RefPerSys path " << path << " with bad header first line " << linbuf << std::endl;
                fclose(fhead);
                return false;
            };
        fclose(fhead);
    }
    //// check presence of ~/RefPerSys/LICENSE file
    {
        std::string licpath=pathstr + "/LICENSE";
        FILE*flic = fopen(licpath.c_str(), "r");
        if (!flic)
            {
                std::cerr << progname << "  RefPerSys path "
                          << path << " without LICENSE file "
                          << licpath << ":"
                          << strerror(errno) << "." << std::endl;
                return false;
            };
        char linbuf[80];
        memset(linbuf, 0, sizeof(linbuf));
        int nbl=0;
        constexpr int liclinelimit=64;
        bool gplmentioned=false;
        while (nbl<liclinelimit && !feof(flic))
            {
                nbl++;
                memset(linbuf, 0, sizeof(linbuf));
                if (!fgets(linbuf, sizeof(linbuf)-4, flic))
                    {
                        if (feof(flic))
                            break;
                        std::cerr << progname << " cannot read line#" << nbl
                                  << " of license file " << licpath
                                  << " :" << strerror(errno) << std::endl;
                        fclose(flic);
                        return false;
                    };
                if (!gplmentioned)
                    gplmentioned = strstr(linbuf, "www.gnu.org/licenses");
            }
        fclose(flic), flic=nullptr;
        if (!gplmentioned)
            {
                std::cerr << progname << "  RefPerSys path "
                          << path << " with incorrect LICENSE file "
                          << licpath << "." << std::endl;
            }
    }
    //// check presence of ~/RefPerSys/rps_manifest.json file
    {
        std::string manifpath=pathstr + "/rps_manifest.json";
        FILE*manif = fopen(manifpath.c_str(), "r");
        if (!manif)
            {
                std::cerr << progname << "  RefPerSys path "
                          << path << " without manifest file "
                          << manifpath << ":"
                          << strerror(errno) << "." << std::endl;
                return false;
            };
        char linbuf[80];
        memset(linbuf, 0, sizeof(linbuf));
        if (!fgets(linbuf, sizeof(linbuf)-4, manif))
            {
                std::cerr << progname << " cannot read first line"
                          << " of manifest file " << manifpath
                          << " :" << strerror(errno) << std::endl;
                fclose(manif);
                return false;
            };
        constexpr const char firstmanif[]="//!! GENERATED file rps_manifest.json / DO NOT EDIT!";
        if (strncmp(linbuf, firstmanif, sizeof(firstmanif)-1))
            {
                std::cerr << progname << " bad first line"
                          << " of manifest file " << manifpath << ":" << std::endl
                          << linbuf << std::endl;
                fclose(manif);
                return false;
            };
    }
    //// check presence of ~/RefPerSys/persistore/ directory and that it contains some *json file.
    {
        std::string persistpath= pathstr+"/persistore";
        DIR* persidir= opendir(persistpath.c_str());
        if (!persidir)
            {
                std::cerr << progname << " cannot open RefPerSys persistent store directory " << persistpath
                          << " :" << strerror(errno) << std::endl;
                return false;
            };
        struct dirent* ent=nullptr;
        int nbjsonfiles=0;
        int nbent=0;
        while ((ent=readdir(persidir)) != nullptr)
            {
                if (ent->d_type == DT_REG /*regular file*/
                        && isalnum(ent->d_name[0]))
                    {
                        nbent++;
                        int nlen = strlen(ent->d_name);
                        if (nlen>10 && !strcmp(ent->d_name+nlen-sizeof(".json")+1, ".json"))
                            nbjsonfiles++;
                    }
            }
        closedir(persidir);
        if (nbjsonfiles==0)
            {
                std::cerr << progname
                          << " did not found JSON files in persistent store directory "
                          << persistpath << " with " << nbent << " entries." << std::endl;
                return false;
            }
    };
    std::cout << progname << " using RefPerSys from " << pathstr << " on " << myhostname << " pid " << (int)getpid() << std::endl;
    return true;
} // end set_refpersys_path



int
main(int argc, char**argv)
{
    progname = argv[0];
    memset(myhostname, 0, sizeof(myhostname));
    gethostname(myhostname, sizeof(myhostname)-4);
    parse_program_options(argc, argv);
    fl_open_display();
    create_main_window();
    main_window->show(argc, argv);
    std::cout << progname << " running pid " << (int)getpid()
              << " on " << myhostname << " FLTK:" << Fl::abi_version()
              << ", git "
              << SHORTGIT_ID << std::endl
              << ".... built " << __DATE__ "," __TIME__
              << " on " << BUILD_HOST << std::endl;
    return Fl::run();
} // end main

/// enf of file progfltk.cc
