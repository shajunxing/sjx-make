# 纯c实现的智能make程序，无需makefile脚本，全自动，适合c项目

之前发布的纯c单h文件make系统 <https://github.com/shajunxing/sjx-make/blob/main/make.h.md> ,依旧需要在项目源文件每次变更后调整编译脚本，太麻烦。于是我一鼓作气用400行代码完成了“一次编写，处处运行”，自动分析头文件依赖关系，自动增量、并行编译。

约定项目目录结构是这样的：

```
项目名
  +-- src
```

所有源代码头文件都平铺在src里，不再分子目录，当然也可以有子目录，但我比较懒，自己的项目都很简单。

执行 `smake`，会编译所有c文件，然后把所有不带main函数的合并为库文件，默认名字同项目名。所有带main函数的各自单独生成同名可执行程序。

编译后的目录如下：

```
项目名
  +-- bin 所有可执行程序和库文件
  +-- src
  +-- tmp 中间过程产生的其它文件，比如目标文件
```

编译支持调试和发布模式；支持动态（也就是生成动态库.dll .so）和静态链接；支持windows和posix兼容系统。

想玩的可以在 <https://github.com/shajunxing/sjx-make/releases> 下载 windows 可执行程序。命令行参数如下：

```
C:\>smake --help
Usage: smake [clean|debug|release|mono|static|dynamic|-h|--help]
  clean             cleanup generated binaries
  debug             with debug symbols
  release           no debug symbols, optimize for speed
  mono              generate monolithic .exe
  static            static build, link with dynamic crt
  dynamic           seperate to .dll .exe, link with dynamic crt
Default is: release + static
  -h, --help        show help
More options can be set in "makefile", see examples.
```

同时也支持简单的配置文件，为了方便编辑器语法高亮，也取名为makefile，格式如下（# 开头的是注释行）：

```
# Configuration file for SJX Smart Make System.
# Values shown below are default values.
#   Build mode, "d" is with debug symbols, "r" is no debug symbols and optimize for speed
#     build=r
#   Linking model, "m" is all static, "s" static project and dynamic crt, "d" is all dynamic
#     linkage=s
#   Pattern to match main function
#     main_pat=int main(
#   Lib and dll name, default is project folder name
#     lib_name=
#   Extra options for linker
#     ex_opts_windows=
#     ex_opts_posix=

# GetUserNameA requires advapi32.lib
ex_opts_windows=user32.lib gdi32.lib imm32.lib advapi32.lib winmm.lib
# suppress fucking stupid readline warnings "Using 'xxx' in statically linked applications requires at runtime the shared libraries ..."
ex_opts_posix=-lm -lreadline -lncurses -ltinfo -Wl,--no-warnings
```

