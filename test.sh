#/bin/sh
export cc="gcc -s -O3 -Wall"
$cc -o bin/sleep src/sleep.c && $cc -o bin/test src/test.c && ./bin/test $*
