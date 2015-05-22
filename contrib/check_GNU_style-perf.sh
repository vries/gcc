#!/bin/bash

prepare ()
{
    tmps=""

    local f
    for f in gcc/*.c ; do
	local tmp
	tmp=$(mktemp)
	tmps="$tmps $tmp"

	cat "$f" \
	    | awk '{printf "+%s\n", $0}' \
	    > "$tmp"
    done
}

finalize ()
{
    local tmp
    for tmp in $tmps; do
	rm -f "$tmp"
    done
}

# one file at a time
method1 ()
{
    for f in $tmps; do
	./contrib/check_GNU_style.sh $f \
	    >/dev/null 2>&1 
    done
}

# one file at at time, stdin
method2 ()
{
    for f in $tmps; do
	cat "$f" \
	    | ./contrib/check_GNU_style.sh - \
	    >/dev/null 2>&1 
    done
}

# all files at once
method3 ()
{
    ./contrib/check_GNU_style.sh $tmps \
	>/dev/null 2>&1 
}

main ()
{
    prepare

    time -p method1

    time -p method2

    time -p method3

    finalize
}

main
