/*
Copyright 2024-2026 ShaJunXing <shajunxing@hotmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef MAKE_H
#define MAKE_H

#ifdef _MSC_VER
    #pragma warning(disable : 4996) // such as "'strcpy' unsafe" or "'rmdir' deprecated"
#endif
#include <assert.h>
#include <errno.h>
#include <float.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <direct.h> // getcwd, chdir, mkdir, rmdir
    #include <windows.h>
#else
    #include <dirent.h>
    #include <libgen.h>
    #include <sys/stat.h>
    #include <sys/wait.h>
    #include <unistd.h>
    #define MAX_PATH 4096
#endif

// DON'T use enum, cannot be used in #if ==
// see https://stackoverflow.com/questions/56522037/why-cant-i-compare-macro-and-enum-using-if
// see https://gcc.gnu.org/onlinedocs/cpp/If.html#If
#define msvc 0
#define gcc 1
#define windows 0
#define posix 1

#ifdef _MSC_VER
    #define compiler msvc
    #define libext ".lib"
    #define objext ".obj"
#elif defined(__GNUC__)
    #define compiler gcc
    #define libext ".a"
    #define objext ".o"
#else
    #error Only msvc and gcc are supported
#endif

#ifdef _WIN32
    #define os windows
    #define dllext ".dll" // use define instead of const for easily string literal concatenation
    #define exeext ".exe"
    #define pathsep "\\"
#else
    #define os posix
    #define dllext ".so"
    #define exeext ""
    #define pathsep "/"
#endif

#ifndef numargs
    // https://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments
    // in msvc, works fine even with old version
    // in gcc, only works with default or -std=gnu2x, -std=c?? will treat zero parameter as 1, maybe ## is only recognized by gnu extension
    #if defined(__GNUC__) && defined(__STRICT_ANSI__)
        #error numargs() only works with gnu extension enabled
    #endif
    #define _numargs_call(__arg_0, __arg_1) __arg_0 __arg_1
    #define _numargs_select(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, __arg_0, ...) __arg_0
    #define numargs(...) _numargs_call(_numargs_select, (_, ##__VA_ARGS__, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#endif

char *basename(const char *path) {
    char *bn = strrchr(path, '/');
    if (bn) {
        return bn + 1;
    }
    bn = strrchr(path, '\\');
    if (bn) {
        return bn + 1;
    }
    return (char *)path;
}

#define _log(__arg_format, __arg_file, __arg_line, ...) \
    printf("%s:%d: " __arg_format "\n", basename(__arg_file), __arg_line, ##__VA_ARGS__)
#define _error_exit(__arg_format, __arg_file, __arg_line, ...) \
    do { \
        _log(__arg_format, __arg_file, __arg_line, ##__VA_ARGS__); \
        exit(EXIT_FAILURE); \
    } while (0)
#ifdef _WIN32
const char *_windows_error_string() {
    // https://learn.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
    // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-formatmessage
    static char es[256];
    memset(es, 0, sizeof(es));
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, GetLastError(), 1033, es, sizeof(es), NULL);
    return es;
}
    #define _windows_error_exit(__arg_file, __arg_line) \
        _error_exit("error %ld: %s", __arg_file, __arg_line, GetLastError(), _windows_error_string())
#endif
#define _posix_error_exit(__arg_file, __arg_line) \
    _error_exit("error %d: %s", __arg_file, __arg_line, errno, strerror(errno)) // also exists in windows

// do not use float, because va_arg needs double, see https://stackoverflow.com/questions/11270588/variadic-function-va-arg-doesnt-work-with-float same below
double _max(size_t nargs, ...) {
    double ret = -DBL_MAX;
    size_t i;
    va_list args;
    va_start(args, nargs);
    for (i = 0; i < nargs; i++) {
        double v = va_arg(args, double);
        if (v > ret) {
            ret = v;
        }
    }
    va_end(args);
    return ret;
}
#ifdef max // see https://stackoverflow.com/questions/4234004/is-maxa-b-defined-in-stdlib-h-or-not
    #undef max
#endif
#define max(...) _max(numargs(__VA_ARGS__), ##__VA_ARGS__)

#ifndef countof
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/countof-macro?view=msvc-170
    #define countof(__arg_0) (sizeof(__arg_0) / sizeof(__arg_0[0]))
#endif

char *_join(const char *sep, size_t nargs, ...) {
    size_t i, len;
    char *ret;
    va_list args;
    va_start(args, nargs);
    for (len = 0, i = 0; i < nargs; i++) {
        len += strlen(va_arg(args, char *));
    }
    va_end(args);
    len += (strlen(sep) * (nargs - 1));
    ret = (char *)calloc(len + 1, 1);
    va_start(args, nargs);
    for (i = 0; i < nargs; i++) {
        if (i > 0) {
            strcat(ret, sep);
        }
        strcat(ret, va_arg(args, char *));
    }
    va_end(args);
    return ret;
}
#define join(__arg_0, ...) _join(__arg_0, numargs(__VA_ARGS__), ##__VA_ARGS__)
#define concat(...) join("", ##__VA_ARGS__)

void _append(char **dest, size_t nargs, ...) {
    size_t i, len = strlen(*dest);
    va_list args;
    va_start(args, nargs);
    for (i = 0; i < nargs; i++) {
        len += strlen(va_arg(args, char *));
    }
    va_end(args);
    *dest = (char *)realloc(*dest, len + 1);
    va_start(args, nargs);
    for (i = 0; i < nargs; i++) {
        strcat(*dest, va_arg(args, char *));
    }
    va_end(args);
}
#define append(__arg_0, ...) _append(__arg_0, numargs(__VA_ARGS__), ##__VA_ARGS__)

bool _equals(const char *str, size_t nargs, ...) {
    bool ret = false;
    size_t i;
    va_list args;
    va_start(args, nargs);
    for (i = 0; i < nargs; i++) {
        const char *cmp = va_arg(args, const char *);
        if (strcmp(str, cmp) == 0) {
            ret = true;
            break;
        }
    }
    va_end(args);
    return ret;
}
#define equals(__arg_0, ...) _equals(__arg_0, numargs(__VA_ARGS__), ##__VA_ARGS__)

bool _startswith(const char *str, size_t nargs, ...) {
    size_t len = strlen(str);
    bool ret = false;
    size_t i;
    va_list args;
    va_start(args, nargs);
    for (i = 0; i < nargs; i++) {
        const char *prefix = va_arg(args, const char *);
        size_t prefixlen = strlen(prefix);
        if (prefixlen <= len && strncmp(str, prefix, prefixlen) == 0) {
            ret = true;
            break;
        }
    }
    va_end(args);
    return ret;
}
#define startswith(__arg_0, ...) _startswith(__arg_0, numargs(__VA_ARGS__), ##__VA_ARGS__)

bool _endswith(const char *str, size_t nargs, ...) {
    size_t len = strlen(str);
    bool ret = false;
    size_t i;
    va_list args;
    va_start(args, nargs);
    for (i = 0; i < nargs; i++) {
        const char *suffix = va_arg(args, const char *);
        size_t suffixlen = strlen(suffix);
        if (suffixlen <= len && strncmp(str + len - suffixlen, suffix, suffixlen) == 0) {
            ret = true;
            break;
        }
    }
    va_end(args);
    return ret;
}
#define endswith(__arg_0, ...) _endswith(__arg_0, numargs(__VA_ARGS__), ##__VA_ARGS__)

#if defined(_MSC_VER) && _MSC_VER < 1900
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/vsnprintf-vsnprintf-vsnprintf-l-vsnwprintf-vsnwprintf-l?view=msvc-170
    #error below vs2015, _vsnprintf() behavior is different with vsnprintf()
#endif
char *format(const char *fmt, ...) {
    char *buf = NULL;
    size_t buflen;
    int result;
    // https://en.cppreference.com/w/c/io/vfprintf
    va_list args, args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args);
    result = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (result < 0) {
        va_end(args_copy);
        return NULL;
    }
    buflen = (size_t)result + 1;
    buf = (char *)calloc(buflen, 1);
    result = vsnprintf(buf, buflen, fmt, args_copy);
    va_end(args_copy);
    if (result < 0) {
        free(buf);
        return NULL;
    }
    return buf;
}

double __mtime(const char *filename) {
    double ret = -DBL_MAX;
#ifdef _WIN32
    HANDLE fh;
    FILETIME mt;
    fh = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (fh != INVALID_HANDLE_VALUE) {
        if (GetFileTime(fh, NULL, NULL, &mt) != 0) {
            ret = (mt.dwHighDateTime * 4294967296.0 + mt.dwLowDateTime) / 10000000.0 - 11644473600.0;
        }
        CloseHandle(fh);
    }
#else
    struct stat sb;
    if (lstat(filename, &sb) == 0) {
        ret = (double)sb.st_mtime;
    }
#endif
    return ret;
}
double _mtime(int nargs, ...) {
    double ret = -DBL_MAX;
    int i;
    va_list args;
    va_start(args, nargs);
    for (i = 0; i < nargs; i++) {
        double t = __mtime(va_arg(args, char *));
        if (t > ret) {
            ret = t;
        }
    }
    va_end(args);
    return ret;
}
#define mtime(...) _mtime(numargs(__VA_ARGS__), ##__VA_ARGS__)

void listdir(const char *dir, void (*callback)(const char *dir, const char *base, const char *ext, void *arg), void *arg) {
    char *standardized_dir = endswith(dir, pathsep) ? concat(dir) : concat(dir, pathsep);
    bool isdir;
    char *filename;
#ifdef _WIN32
    char *pattern = concat(standardized_dir, "*");
    HANDLE sh;
    WIN32_FIND_DATAA fd;
    sh = FindFirstFileA(pattern, &fd);
    if (sh != INVALID_HANDLE_VALUE) {
        do {
            isdir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            filename = fd.cFileName;
#else
    // https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/
    struct dirent *de;
    DIR *dr = opendir(standardized_dir);
    if (dr != NULL) {
        while ((de = readdir(dr)) != NULL) {
            isdir = de->d_type == DT_DIR;
            filename = de->d_name;
#endif
            // BEGIN BLOCK
            if (isdir) {
                if (!equals(filename, ".") && !equals(filename, "..")) {
                    char *subdir = concat(standardized_dir, filename, pathsep);
                    callback(subdir, NULL, NULL, arg);
                    free(subdir);
                }
            } else {
                char *ext = strrchr(filename, '.');
                if (ext) {
                    size_t baselen = ext - filename;
                    char *base = (char *)calloc(baselen + 1, 1);
                    strncpy(base, filename, baselen);
                    callback(standardized_dir, base, ext, arg);
                    free(base);
                } else {
                    callback(standardized_dir, filename, "", arg);
                }
            }
            // END BLOCK
#ifdef _WIN32
        } while (FindNextFileA(sh, &fd) != 0);
        FindClose(sh);
    }
    free(pattern);
#else
        }
        closedir(dr);
    }
#endif
    free(standardized_dir);
}

#ifdef _WIN32
char *dirname(const char *path) {
    // https://stackoverflow.com/questions/21229214/how-to-get-the-directory-of-a-file-from-the-full-path-in-c
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/splitpath-s-wsplitpath-s?view=msvc-170&redirectedfrom=MSDN
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/makepath-s-wmakepath-s?view=msvc-170
    char drive[_MAX_DRIVE] = {0};
    char dir[_MAX_DIR] = {0};
    if (_splitpath_s(path, drive, sizeof(drive), dir, sizeof(dir), NULL, 0, NULL, 0)) {
        _posix_error_exit(__FILE__, __LINE__);
    }
    static char ret[MAX_PATH];
    memset(ret, 0, sizeof(ret));
    if (_makepath_s(ret, sizeof(ret), drive, dir, NULL, NULL)) {
        _posix_error_exit(__FILE__, __LINE__);
    }
    return ret;
}
#endif

char *cwd() {
    static char ret[MAX_PATH];
    memset(ret, 0, sizeof(ret));
    if (getcwd(ret, sizeof(ret)) == NULL) {
        _posix_error_exit(__FILE__, __LINE__);
    }
    return ret;
}

// since mkdir is different, to unify usage, better use following macros instead of origin ones
#define cd chdir
#define rd rmdir
#ifdef _WIN32
    #define md mkdir
#else
    #define md(__arg_path) mkdir(__arg_path, 0777)
#endif

// https://stackoverflow.com/questions/2180079/how-can-i-copy-a-file-on-unix-using-c
void cp(const char *to, const char *from) {
    char buf[1024];
    FILE *fp_to = fopen(to, "wb");
    FILE *fp_from = fopen(from, "rb");
    if (!fp_to || !fp_from) {
        _posix_error_exit(__FILE__, __LINE__);
    }
    size_t num_read;
    while ((num_read = fread(buf, 1, sizeof(buf), fp_from))) {
        fwrite(buf, 1, num_read, fp_to);
    }
    fclose(fp_from);
    fclose(fp_to);
}

// stdlib.c already has _sleep()
void __sleep(double secs) {
    // _log("Sleeping %g seconds", __FILE__, __LINE__, secs);
#ifdef _WIN32
    Sleep((DWORD)(secs * 1000));
#else
    usleep((int)(secs * 1000000));
#endif
}

#define run(__arg_cmd) \
    do { \
        _log("%s", __FILE__, __LINE__, __arg_cmd); \
        int ret = system(__arg_cmd); \
        if (ret != 0) { \
            _log("%s: exit code is %d.", __FILE__, __LINE__, __arg_cmd, ret); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

unsigned _parallel_num_workers = 0;
struct _parallel_worker_info {
    const char *file;
    int line;
#ifdef _WIN32
    HANDLE proc;
#else
    pid_t proc;
#endif
    char *cmd;
};
struct _parallel_worker_info *_parallel_workers = NULL;
double __parallel_poll_interval = 0.2;

void _parallel_init() {
    if (_parallel_num_workers == 0) {
#ifdef _WIN32
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        _parallel_num_workers = (unsigned)info.dwNumberOfProcessors;
#else
        // https://stackoverflow.com/questions/2693948/how-do-i-retrieve-the-number-of-processors-on-c-linux
        _parallel_num_workers = (unsigned)sysconf(_SC_NPROCESSORS_ONLN);
#endif
        if (_parallel_num_workers == 0) {
            _error_exit("Failed to get _parallel_num_workers.", __FILE__, __LINE__);
        }
        _parallel_workers = (struct _parallel_worker_info *)calloc(_parallel_num_workers, sizeof(struct _parallel_worker_info));
    }
}
void _parallel_zero_out(size_t slot) {
    _parallel_workers[slot].file = NULL;
    _parallel_workers[slot].line = 0;
    _parallel_workers[slot].proc = 0;
    free(_parallel_workers[slot].cmd);
    _parallel_workers[slot].cmd = NULL;
}

// void _parallel_kill_all() {
//     size_t slot;
//     for (slot = 0; slot < _parallel_num_workers; slot++) {
//         if (_parallel_workers[slot].proc != 0) {
//             // printf("kill: %s\n", _parallel_workers[slot].cmd);
// #ifdef _WIN32
//             if (TerminateProcess(_parallel_workers[slot].proc, EXIT_FAILURE)) {
//                 _parallel_zero_out(slot);
//             } else {
//                 _windows_error_exit(__FILE__, __LINE__);
//             }
// #else
//             if (kill(_parallel_workers[slot].proc, SIGKILL) == 0) {
//                 _parallel_zero_out(slot);
//             } else {
//                 _posix_error_exit(__FILE__, __LINE__);
//             }
// #endif
//         }
//     }
// }

// kill running process may cause generated file corruped, change to sending ctrl-c
// void _parallel_kill_all() {
// #ifdef _WIN32
//     // if running in batch, will cause anonying 'Terminate batch job (Y/N)?'
//     if (GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0) == 0) {
//         _windows_error_exit(__FILE__, __LINE__);
//     }
// #else
//     if (kill(0, SIGINT)) {
//         _posix_error_exit(__FILE__, __LINE__);
//     }
// #endif
// }

void _parallel_kill_all() {}

// in windows, using CREATE_NEW_PROCESS_GROUP to prevent if run in batch ctrl-c event will be passed to cmd.exe and display anoying 'Terminate batch job (Y/N)?'
// not works!
// and maybe it's harmful because child processes with new group will never receive user's pressing ctrl-c
// void _parallel_kill_all() {
//     for (size_t slot = 0; slot < _parallel_num_workers; slot++) {
//         if (_parallel_workers[slot].proc != 0) {
//             // printf("kill: %s\n", _parallel_workers[slot].cmd);
// #ifdef _WIN32
//             printf("%ld\n", _parallel_workers[slot].grp);
//             if (GenerateConsoleCtrlEvent(CTRL_C_EVENT, _parallel_workers[slot].grp)) {
//                 _parallel_zero_out(slot);
//             } else {
//                 _windows_error_exit(__FILE__, __LINE__);
//             }
// #else
//             if (kill(_parallel_workers[slot].proc, SIGINT) == 0) {
//                 _parallel_zero_out(slot);
//             } else {
//                 _posix_error_exit(__FILE__, __LINE__);
//             }
// #endif
//         }
//     }
// }

// void _parallel_cleanup() {
//     _parallel_num_workers = 0;
//     free(_parallel_workers);
//     _parallel_workers = NULL;
// }

enum _worker_state {
    _st_available,
    _st_running,
    _st_exit_failure
};

enum _worker_state _parallel_check(size_t slot) {
    long exit_code;
    if (_parallel_workers[slot].proc == 0) { // slot is available
        return _st_available;
    }
    {
#ifdef _WIN32
        DWORD status;
        if (WaitForSingleObject(_parallel_workers[slot].proc, 0) != WAIT_OBJECT_0) { // process is running
            return _st_running;
        }
        if (GetExitCodeProcess(_parallel_workers[slot].proc, &status) == 0) {
            _windows_error_exit(__FILE__, __LINE__);
        }
        exit_code = (long)status;
#else
        int status;
        if (waitpid(_parallel_workers[slot].proc, &status, WNOHANG) == 0) { // process is running
            return _st_running;
        }
        exit_code = WEXITSTATUS(status);
#endif
    }
    if (exit_code != 0) {
        _log("%s: exit code is %ld.", _parallel_workers[slot].file, _parallel_workers[slot].line, _parallel_workers[slot].cmd, exit_code);
        _parallel_zero_out(slot); // must zero out to prevent next kill error, must put after _log because _log must know slot contents
        return _st_exit_failure;
    } else {
        _parallel_zero_out(slot); // must zero out to prevent next kill error
        return _st_available;
    }
}

void _parallel_wait_all_and_exit(const char *file, int line) {
    _log("Wait all and exit...", file, line);
    for (size_t slot = 0; slot < _parallel_num_workers; slot++) {
        while (_parallel_check(slot) == _st_running) {
            __sleep(__parallel_poll_interval);
        }
    }
    exit(EXIT_FAILURE);
}

void _parallel_run(const char *file, int line, const char *cmd) {
    _parallel_init();
    size_t slot_available;
    for (;;) {
        // wait one of the _parallel_workers to finish
        bool any_available = false;
        for (size_t slot = 0; slot < _parallel_num_workers; slot++) {
            // must loop whole workers to handle possible error exit
            enum _worker_state state = _parallel_check(slot);
            if (state == _st_available) {
                any_available = true;
                slot_available = slot;
            } else if (state == _st_exit_failure) {
                _parallel_wait_all_and_exit(file, line);
            }
        }
        if (any_available) {
            break;
        }
        __sleep(__parallel_poll_interval);
    }
    _log("%s", file, line, cmd);
    {
#ifdef _WIN32
        STARTUPINFOA si = {sizeof(STARTUPINFOA)};
        PROCESS_INFORMATION pi;
        if (CreateProcessA(NULL, (LPSTR)cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            _parallel_workers[slot_available].file = file;
            _parallel_workers[slot_available].line = line;
            _parallel_workers[slot_available].proc = pi.hProcess;
            _parallel_workers[slot_available].cmd = (char *)calloc(strlen(cmd) + 1, 1);
            strcpy(_parallel_workers[slot_available].cmd, cmd);
        } else {
            _windows_error_exit(file, line);
        }
#else
        pid_t pid = fork(); // posix_spawn's parameters are hard to use
        switch (pid) {
        case -1:
            _posix_error_exit(file, line);
            break;
        case 0: {
                // from chatgpt
    #define MAX_ARGS 64
            char *argv[MAX_ARGS];
            char *cmd_copy = strdup(cmd);
            char *token;
            int i = 0;
            if (!cmd_copy) {
                _posix_error_exit(file, line);
            }
            token = strtok(cmd_copy, " ");
            while (token != NULL && i < MAX_ARGS - 1) {
                argv[i++] = token;
                token = strtok(NULL, " ");
            }
            argv[i] = NULL;
            if (i == 0) {
                free(cmd_copy);
                _error_exit("Empty command\n", file, line);
            }
            execvp(argv[0], argv);
            // Only reached if execvp fails
            perror("execvp failed");
            free(cmd_copy);
            _posix_error_exit(file, line);
        } break;
        default:
            _parallel_workers[slot_available].file = file;
            _parallel_workers[slot_available].line = line;
            _parallel_workers[slot_available].proc = pid;
            _parallel_workers[slot_available].cmd = (char *)calloc(strlen(cmd) + 1, 1);
            strcpy(_parallel_workers[slot_available].cmd, cmd);
            break;
        }
#endif
    }
}

#define async(__arg_0) _parallel_run(__FILE__, __LINE__, __arg_0)

void _parallel_wait_all(const char *file, int line) {
    if (_parallel_num_workers == 0) {
        return;
    }
    for (;;) {
        bool all_finished = true;
        for (size_t slot = 0; slot < _parallel_num_workers; slot++) {
            // must loop whole workers to handle possible error exit
            enum _worker_state state = _parallel_check(slot);
            if (state == _st_running) {
                all_finished = false;
            } else if (state == _st_exit_failure) {
                _parallel_wait_all_and_exit(file, line);
            }
        }
        if (all_finished) {
            break;
        }
        __sleep(__parallel_poll_interval);
    }
}
#define await() _parallel_wait_all(__FILE__, __LINE__)

const char *build = "r";
const char *linkage = "s";

#define bin_dir "bin" pathsep
#define tmp_dir "tmp" pathsep
#define src_dir "src" pathsep

#define d(__arg_base) bin_dir #__arg_base dllext
#define e(__arg_base) bin_dir #__arg_base exeext
#define l(__arg_base) bin_dir #__arg_base libext
#define o(__arg_base) tmp_dir #__arg_base objext
#define c(__arg_base) src_dir #__arg_base ".c"
#define h(__arg_base) src_dir #__arg_base ".h"

// DON'T use global pack option such as '/Zp', see banana-ui's make.c for explanation
// "/Zp" means #pragma pack(1), DON'T use it
// msvc link default incremental is yes, sometimes will 'xxx not found or not built by the last incremental link; performing full link'
// "-fpack-struct" also means #pragma pack(1), but will cause lot's of "taking address of packed member may result in an unaligned pointer value" warning
// DON'T use '__in', it is msvc's keyword
// in gcc, .a must before .o, or too many undefined reference error
// -shared can be used in mingw exe build, although exes cannot be executed, dependency walker can be used to check whether executable functions wrongly exported because forgot adding 'static'
// mingw cannot static link against ucrt
// in gcc, add some common used such as math.h required lib "-lm"

const char *_command_format_table[][2] = {
    // msvc
    {"msvc-cc-l-d-m", "cl /nologo /c /W3 /utf-8 /std:clatest /Z7 /DDEBUG /MTd /Fo%s %s"},
    {"msvc-cc-l-d-s", "cl /nologo /c /W3 /utf-8 /std:clatest /Z7 /DDEBUG /MDd /Fo%s %s"},
    {"msvc-cc-l-d-d", "cl /nologo /c /W3 /utf-8 /std:clatest /Z7 /DDEBUG /MDd /DDLL /DEXPORT /Fo%s %s"},
    {"msvc-cc-l-r-m", "cl /nologo /c /W3 /utf-8 /std:clatest /O2 /MT /Fo%s %s"},
    {"msvc-cc-l-r-s", "cl /nologo /c /W3 /utf-8 /std:clatest /O2 /MD /Fo%s %s"},
    {"msvc-cc-l-r-d", "cl /nologo /c /W3 /utf-8 /std:clatest /O2 /MD /DDLL /DEXPORT /Fo%s %s"},
    {"msvc-cc-x-d-m", "cl /nologo /c /W3 /utf-8 /std:clatest /Z7 /DDEBUG /MTd /Fo%s %s"},
    {"msvc-cc-x-d-s", "cl /nologo /c /W3 /utf-8 /std:clatest /Z7 /DDEBUG /MDd /Fo%s %s"},
    {"msvc-cc-x-d-d", "cl /nologo /c /W3 /utf-8 /std:clatest /Z7 /DDEBUG /MDd /DDLL /Fo%s %s"},
    {"msvc-cc-x-r-m", "cl /nologo /c /W3 /utf-8 /std:clatest /O2 /MT /Fo%s %s"},
    {"msvc-cc-x-r-s", "cl /nologo /c /W3 /utf-8 /std:clatest /O2 /MD /Fo%s %s"},
    {"msvc-cc-x-r-d", "cl /nologo /c /W3 /utf-8 /std:clatest /O2 /MD /DDLL /Fo%s %s"},
    {"msvc-ld-l-d-m", "lib /nologo /out:%s %s"},
    {"msvc-ld-l-d-s", "lib /nologo /out:%s %s"},
    {"msvc-ld-l-d-d", "link /nologo /incremental:no /debug /dll /out:%s %s"},
    {"msvc-ld-l-r-m", "lib /nologo /out:%s %s"},
    {"msvc-ld-l-r-s", "lib /nologo /out:%s %s"},
    {"msvc-ld-l-r-d", "link /nologo /incremental:no /dll /out:%s %s"},
    {"msvc-ld-x-d-m", "link /nologo /incremental:no /debug /out:%s %s"},
    {"msvc-ld-x-d-s", "link /nologo /incremental:no /debug /out:%s %s"},
    {"msvc-ld-x-d-d", "link /nologo /incremental:no /debug /out:%s %s"},
    {"msvc-ld-x-r-m", "link /nologo /incremental:no /out:%s %s"},
    {"msvc-ld-x-r-s", "link /nologo /incremental:no /out:%s %s"},
    {"msvc-ld-x-r-d", "link /nologo /incremental:no /out:%s %s"},
    // mingw
    {"mingw-cc-l-d-s", "gcc -c -Wall -DDEBUG -o %s %s"},
    {"mingw-cc-l-d-d", "gcc -c -Wall -DDEBUG -DDLL -DEXPORT -o %s %s"},
    {"mingw-cc-l-r-s", "gcc -c -Wall -O3 -o %s %s"},
    {"mingw-cc-l-r-d", "gcc -c -Wall -O3 -DDLL -DEXPORT -o %s %s"},
    {"mingw-cc-x-d-s", "gcc -c -Wall -DDEBUG -o %s %s"},
    {"mingw-cc-x-d-d", "gcc -c -Wall -DDEBUG -DDLL -o %s %s"},
    {"mingw-cc-x-r-s", "gcc -c -Wall -O3 -o %s %s"},
    {"mingw-cc-x-r-d", "gcc -c -Wall -O3 -DDLL -o %s %s"},
    {"mingw-ld-l-d-s", "ar rcs %s %s"},
    {"mingw-ld-l-d-d", "gcc -fvisibility=hidden -fvisibility-inlines-hidden -shared -o %s %s"},
    {"mingw-ld-l-r-s", "ar rcs %s %s"},
    {"mingw-ld-l-r-d", "gcc -fvisibility=hidden -fvisibility-inlines-hidden -s -Wl,--exclude-all-symbols -shared -o %s %s"},
    {"mingw-ld-x-d-s", "gcc -fvisibility=hidden -fvisibility-inlines-hidden -static -static-libgcc -o %s %s"},
    {"mingw-ld-x-d-d", "gcc -fvisibility=hidden -fvisibility-inlines-hidden -o %s %s"},
    {"mingw-ld-x-r-s", "gcc -fvisibility=hidden -fvisibility-inlines-hidden -static -static-libgcc -s -Wl,--exclude-all-symbols -o %s %s"},
    {"mingw-ld-x-r-d", "gcc -fvisibility=hidden -fvisibility-inlines-hidden -s -Wl,--exclude-all-symbols -o %s %s"},
    // gcc
    {"gcc-cc-l-d-s", "gcc -c -Wall -fPIC -DDEBUG -o %s %s"},
    {"gcc-cc-l-d-d", "gcc -c -Wall -fPIC -DDEBUG -DDLL -DEXPORT -o %s %s"},
    {"gcc-cc-l-r-s", "gcc -c -Wall -fPIC -O3 -o %s %s"},
    {"gcc-cc-l-r-d", "gcc -c -Wall -fPIC -O3 -DDLL -DEXPORT -o %s %s"},
    {"gcc-cc-x-d-s", "gcc -c -Wall -fPIC -DDEBUG -o %s %s"},
    {"gcc-cc-x-d-d", "gcc -c -Wall -fPIC -DDEBUG -DDLL -o %s %s"},
    {"gcc-cc-x-r-s", "gcc -c -Wall -fPIC -O3 -o %s %s"},
    {"gcc-cc-x-r-d", "gcc -c -Wall -fPIC -O3 -DDLL -o %s %s"},
    {"gcc-ld-l-d-s", "ar rcs %s %s"},
    {"gcc-ld-l-d-d", "gcc -fvisibility=hidden -fvisibility-inlines-hidden -shared -o %s %s"},
    {"gcc-ld-l-r-s", "ar rcs %s %s"},
    {"gcc-ld-l-r-d", "gcc -fvisibility=hidden -fvisibility-inlines-hidden -s -shared -o %s %s"},
    {"gcc-ld-x-d-s", "gcc -fvisibility=hidden -fvisibility-inlines-hidden -static -static-libgcc -o %s %s"},
    {"gcc-ld-x-d-d", "gcc -fvisibility=hidden -fvisibility-inlines-hidden -o %s %s"},
    {"gcc-ld-x-r-s", "gcc -fvisibility=hidden -fvisibility-inlines-hidden -static -static-libgcc -s -o %s %s"},
    {"gcc-ld-x-r-d", "gcc -fvisibility=hidden -fvisibility-inlines-hidden -s -o %s %s"},
};

const char *_get_command_format(const char *tool, const char *target, const char *out, const char *opts) {
    char idx[32] = {0};
    strcat(idx, os == windows ? (compiler == msvc ? "msvc" : "mingw") : "gcc");
    strcat(idx, "-");
    strcat(idx, tool);
    strcat(idx, "-");
    strcat(idx, target);
    strcat(idx, "-");
    strcat(idx, build);
    strcat(idx, "-");
    strcat(idx, linkage);
    const char *fmt = NULL;
    for (size_t i = 0; i < countof(_command_format_table); i++) {
        if (equals(idx, _command_format_table[i][0])) {
            fmt = _command_format_table[i][1];
            break;
        }
    }
    return fmt;
}

#define _async_command(__arg_tool, __arg_target, __arg_out, __arg_opts) \
    do { \
        const char *__fmt = _get_command_format(__arg_tool, __arg_target, __arg_out, __arg_opts); \
        if (!__fmt) { \
            _error_exit("Command not found", __FILE__, __LINE__); \
        } \
        char *__cmd = format(__fmt, __arg_out, __arg_opts); \
        async(__cmd); \
        free(__cmd); \
    } while (0)

#define cc_lib(__arg_out, __arg_opts) _async_command("cc", "l", __arg_out, __arg_opts)
#define ld_lib(__arg_out, __arg_opts) _async_command("ld", "l", __arg_out, __arg_opts)
#define cc_exe(__arg_out, __arg_opts) _async_command("cc", "x", __arg_out, __arg_opts)
#define ld_exe(__arg_out, __arg_opts) _async_command("ld", "x", __arg_out, __arg_opts)

#endif