#!/bin/sh

# Run this script from the build dir, like so:
# ...
# $ pwd
# build/libbacktrace
# $ ../../src/libbacktrace/check-deps.sh
# $
# ...

scriptdir=$(cd $(dirname $0); pwd -P)

deps=$scriptdir/Makefile.deps

los=$(echo *.lo)
os=$(echo *.o)

unlisted=""

for lo in $los; do
    cnt=$(grep -c "^$lo:" $deps)
    if [ $cnt -eq 0 ]; then
	unlisted="$unlisted $lo"
    fi
done

for o in $os; do
    lo=${o%.o}.lo
    if [ -f $lo ]; then
	continue
    fi

    cnt=$(grep -c "^$o:" $deps)
    if [ $cnt -eq 0 ]; then
	unlisted="$unlisted $o"
    fi
done

echo "Unlisted files:$unlisted"
