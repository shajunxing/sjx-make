# latest

Added some predefined build commands such as `cc_xxx()` `ld_xxx()`, why using macro instead of function is because macro can correctly log position when error happens, see `make.c` in <https://github.com/shajunxing/banana-script> for example.

Now won't kill worker processes if one of them return value is not 0, but wait for them to end, to prevent possible disk file corruption when force killing compiling process.

New function `cp`

Since `mkdir` is different, to unify usage, better use `cd` `md` `rd` instead of `chdir` `mkdir` `rmdir`

Reworked makefile, now fully support msvc, mingw and linux gcc, static build will be fully static (except mingw, cannot make ucrt static), in windows, will only depend kernel32.dll, and in linux, only kernel, and on the contrary, shared will be fully dynamic.

Added default configuration for msvc, mingw and linux gcc, see <https://github.com/shajunxing/banana-script/blob/main/make.c> for example.

# 2025.05.31 2025.06.26 2025.06.29

Changed `compiler` `os` from const enum to macro definition, now they can be used in macro conditions.

# 2025.05.16

Added asynchronous mechanism, now you can run tasks concurrently equal to maximum number of CPU cores.

# 2024.12.02

First commit.