/*
Copyright 2024-2025 ShaJunXing <shajunxing@hotmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

// for test purpose

#include "make.h"
#include <wchar.h>
#include <locale.h>

#define test(__arg_0, __arg_1) printf("%-40s= " __arg_1 "\n", #__arg_0, __arg_0)

void callback(const char *dir, const char *base, const char *ext) {
    if (base == NULL) {
        printf("DIR:  %s\n", dir);
        listdir(dir, callback);
    } else {
        char *path = concat(dir, base, ext);
        printf("FILE: %s %g\n", path, mtime(path));
        free(path);
    }
}

// if there are both "sleep.exe" and "sleep" in current folder, and if use ".\sleep" instead of "sleep", CreateProcess will not auto add ".exe"
#define sleep_program "." pathsep "sleep" exeext

int main(int argc, char *argv[]) {
    cd(dirname(argv[0]));
    // char *s;
    // test(numargs(), "%d");
    // test(numargs(3.14), "%d");
    // test(numargs(3.14, "hello"), "%d");
    // test(max(), "%g");
    // test(max(1.0, 3.0, 2.0), "%g"); // DON'T use 1, 2, 3 or will get strange result (integer memory as double)
    // s = join("-");
    // test(s, "%s");
    // free(s);
    // s = join("-", "foo", "bar", "baz");
    // test(s, "%s");
    // free(s);
    // s = concat();
    // test(s, "%s");
    // free(s);
    // s = concat("foo", "bar", "baz");
    // test(s, "%s");
    // append(&s);
    // test(s, "%s");
    // append(&s, "qux", "quxx");
    // test(s, "%s");
    // free(s);
    // test(equals("foo", "fo", "foo", "fooo"), "%d");
    // test(equals("foo", "fo", "fox", "foxx"), "%d");
    // test(startswith("helo", "he", "el", "lo"), "%d");
    // test(startswith("helo", "helox", "el", "lo"), "%d");
    // test(endswith("helo", "he", "el", "lo"), "%d");
    // test(endswith("helo", "he", "el", "xhelo"), "%d");
    // s = format("%d %g %s", 1, 3.14, "hello");
    // test(s, "%s");
    // free(s);
    // setlocale(LC_ALL, "C");
    // s = format("%s", NULL);
    // test(s, "%s");
    // free(s);
    // s = format("%ls", L"こんにちは");
    // test(s, "%s");
    // free(s);
    // test(mtime(argv[0]), "%g");
    // test(mtime("non-existent"), "%g");
    // test(mtime(argv[0], "non-existent"), "%g");
    // listdir(".", callback);
    // _log("foo");
    // _log("bar %d", 1);
    // _log("baz %d %s", 2, "qux");
    // // _error_exit("foo");
    // // _error_exit("bar %d", 1);
    // // _error_exit("baz %d %s", 2, "qux");
    // _parallel_init();
    // test(_parallel_num_workers, "%u");
    // run(sleep_program " 1");
    // run(sleep_program " 3");
    // // run(sleep_program);

    async(sleep_program " 1");
    async(sleep_program " 2");
    async(sleep_program " 3");
    async(sleep_program " 4");
    async(sleep_program " 5");
    async(sleep_program " 6");
    await();
    async(sleep_program " 1");
    async(sleep_program " 2");
    async(sleep_program " 3");
    async(sleep_program " 4");
    async(sleep_program " 5");
    async(sleep_program " 6");
    async(sleep_program);
    async(sleep_program " 1");
    async(sleep_program " 2");
    async(sleep_program " 3");
    async(sleep_program " 4");
    async(sleep_program " 5");
    async(sleep_program " 6");
    await();

    // char *dir = dirname(argv[0]);
    // test(dir, "\"%s\"");
    // cd(dir);
    // test(cwd(), "%s");
    return EXIT_SUCCESS;
}
