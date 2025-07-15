/*
Copyright 2024-2025 ShaJunXing <shajunxing@hotmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

// for test purpose

#include <signal.h>
#include "make.h"

void signal_handler(int signal) {
    _log("Received signal %d", __FILE__, __LINE__, signal);
    exit(signal);
}

int main(int argc, char *argv[]) {
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/signal?view=msvc-170
    signal(SIGABRT, signal_handler);
    signal(SIGFPE, signal_handler);
    signal(SIGILL, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGTERM, signal_handler);
    double secs;
    if (argc != 2) {
        goto usage;
    }
    secs = strtod(argv[1], NULL);
    if (secs == 0) {
        goto usage;
    }
    _log("Start sleeping %g seconds.", __FILE__, __LINE__, secs);
    __sleep(secs);
    _log("End sleeping %g seconds.", __FILE__, __LINE__, secs);
    return EXIT_SUCCESS;
usage:
    printf("Usage: %s <secs>\n", argv[0]);
    return EXIT_FAILURE;
}