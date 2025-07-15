#define cc_msvc_prefix "cl /nologo /c /W3 /utf-8 /std:clatest"
#define cc_msvc_suffix "/Fo%s %s %s"
#define cc_msvc(__arg_out, __arg_options, __arg_in) \
    do { \
        char *cmd = format( \
            debug \
                ? cc_msvc_prefix " /Z7 /DDEBUG " cc_msvc_suffix \
                : cc_msvc_prefix " /O2 " cc_msvc_suffix, \
            __arg_out, __arg_options, __arg_in); \
        async(cmd); \
        free(cmd); \
    } while (0)

#define ld_msvc_prefix "link /nologo /incremental:no /nodefaultlib"
#define ld_msvc_suffix "/out:%s %s %s msvcrt.lib libvcruntime.lib ucrt.lib kernel32.lib user32.lib"
#define ld_msvc(__arg_out, __arg_options, __arg_in) \
    do { \
        char *cmd = format( \
            debug \
                ? ld_msvc_prefix " /debug " ld_msvc_suffix \
                : ld_msvc_prefix " " ld_msvc_suffix, \
            __arg_out, __arg_options, __arg_in); \
        async(cmd); \
        free(cmd); \
    } while (0)

#define ar_msvc(__arg_out, __arg_options, __arg_in) \
    do { \
        char *cmd = format("lib /nologo /out:%s %s %s", __arg_out, __arg_options, __arg_in); \
        async(cmd); \
        free(cmd); \
    } while (0)

#if os == windows // mingw
    #define cc_gcc_prefix "gcc -c -Wall"
#else
    #define cc_gcc_prefix "gcc -c -Wall -fPIC"
#endif
#define cc_gcc_suffix "-o %s %s %s"
#define cc_gcc(__arg_out, __arg_options, __arg_in) \
    do { \
        char *cmd = format( \
            debug \
                ? cc_gcc_prefix " -DDEBUG " cc_gcc_suffix \
                : cc_gcc_prefix " -O3 " cc_gcc_suffix, \
            __arg_out, __arg_options, __arg_in); \
        async(cmd); \
        free(cmd); \
    } while (0)

#if os == windows // mingw
    #define ld_gcc_prefix "gcc -fvisibility=hidden -fvisibility-inlines-hidden -static -static-libgcc"
    #define ld_gcc_release_options " -s -Wl,--exclude-all-symbols "
    #define ld_gcc_suffix "-o %s %s %s"
#else
    #define ld_gcc_prefix "gcc -fvisibility=hidden -fvisibility-inlines-hidden -static-libgcc"
    #define ld_gcc_release_options " -s "
    #define ld_gcc_suffix "-o %s %s %s -lm" // added some common used libs such as '-lm' used by math.h
#endif
#define ld_gcc(__arg_out, __arg_options, __arg_in) \
    do { \
        char *cmd = format( \
            debug \
                ? ld_gcc_prefix " " ld_gcc_suffix \
                : ld_gcc_prefix ld_gcc_release_options ld_gcc_suffix, \
            __arg_out, __arg_options, __arg_in); \
        async(cmd); \
        free(cmd); \
    } while (0)

#define ar_gcc(__arg_out, __arg_options, __arg_in) \
    do { \
        char *cmd = format("ar rcs %s %s", __arg_out, __arg_in); \
        async(cmd); \
        free(cmd); \
    } while (0)

#if compiler == msvc
    #define cc_lib(__arg_obj, __arg_c) \
        do { \
            if (shared) { \
                cc_msvc(__arg_obj, "/DDLL /DEXPORT", __arg_c); \
            } else { \
                cc_msvc(__arg_obj, "", __arg_c); \
            } \
        } while (0)
    #define ld_lib(__arg_dll, __arg_lib, __arg_objs) \
        do { \
            if (shared) { \
                ld_msvc(__arg_dll, "/dll", __arg_objs); \
            } else { \
                ar_msvc(__arg_lib, "", __arg_objs); \
            } \
        } while (0)
    #define cc_exe(__arg_obj, __arg_c) \
        do { \
            if (shared) { \
                cc_msvc(__arg_obj, "/DDLL", __arg_c); \
            } else { \
                cc_msvc(__arg_obj, "", __arg_c); \
            } \
        } while (0)
    #define ld_exe(__arg_exe, __arg_obj, __arg_dll, __arg_lib) \
        do { \
            ld_msvc(__arg_exe, __arg_lib, __arg_obj); \
        } while (0)
#else
    #define cc_lib(__arg_obj, __arg_c) \
        do { \
            if (shared) { \
                cc_gcc(__arg_obj, "-DDLL -DEXPORT", __arg_c); \
            } else { \
                cc_gcc(__arg_obj, "", __arg_c); \
            } \
        } while (0)
    #define ld_lib(__arg_dll, __arg_lib, __arg_objs) \
        do { \
            if (shared) { \
                ld_gcc(__arg_dll, "-shared", __arg_objs); \
            } else { \
                ar_gcc(__arg_lib, "", __arg_objs); \
            } \
        } while (0)
    #define cc_exe(__arg_obj, __arg_c) \
        do { \
            if (shared) { \
                cc_gcc(__arg_obj, "-DDLL", __arg_c); \
            } else { \
                cc_gcc(__arg_obj, "", __arg_c); \
            } \
        } while (0)
    #define ld_exe(__arg_exe, __arg_obj, __arg_dll, __arg_lib) \
        do { \
            if (shared) { \
                ld_gcc(__arg_exe, __arg_obj, __arg_dll); \
            } else { \
                ld_gcc(__arg_exe, __arg_obj, __arg_lib); \
            } \
        } while (0)
#endif
