#!/bin/sh

evaluate_makefile ()
{
    # Resolve line continuations
    sed -e :a -e '/\\$/N; s/\\\n//; ta' Makefile.am > Makefile.cont

    # Filter out simple assignments
    grep " = "  Makefile.cont > Makefile.tmp
    rm -f Makefile.cont

    # Get variable names
    local vars
    vars=$(awk '{print $1}' Makefile.tmp)

    # Evaluate variables
    for var in $vars; do
	cp Makefile.tmp Makefile.print
	echo -e "all:\n\t@echo \$($var)" >> Makefile.print
	val=$(make -f Makefile.print)
	rm -f Makefile.print
	echo "$var = $val"
    done > Makefile.evaluated

    rm -f Makefile.tmp
}

get_los ()
{
    local files
    files=$(egrep 'la_SOURCES|_LDADD|la_LIBADD' Makefile.evaluated \
		   | sed 's/.* = //' \
		   | sed 's/ /\n/g' \
		   | sort -u)

    los=""

    local f
    for f in $files; do
	case $f in
	    *.lo)
		los="$los $f"
		;;
	    *.c)
		lo=${f%.c}.lo
		los="$los $lo"
		;;
	    *.h)
		continue
		;;
	    *.la)
		continue
		;;
	    *)
		echo "unknown: $f"
		exit 1
	esac
    done

    los=$(echo $los \
		 | sed 's/ /\n/g' \
		 | sort -u)
}

get_tests ()
{
    tests=$(grep _SOURCES Makefile.evaluated \
		   | grep -v la_SOURCES \
		   | sed 's/_SOURCES.*//')
}

handle_include ()
{
    local i
    i="$1"

    if [ -f $i ]; then
	echo "$i"
	return
    fi

    case $i in
	unwind.h)
	# Skip, see comment in Makefile.am
	;;
	config.h|backtrace-supported.h)
	    # Generated files
	    echo "$i"
	    ;;
	filenames.h)
	    # External files
	    echo '$(INCDIR)/filenames.h'
	    ;;
	dwarf2.h)
	    echo '$(INCDIR)/dwarf2.h $(INCDIR)/dwarf2.def'
	    ;;
	*)
	    echo "unhandled: $i"
	    exit 1
    esac
}

gen_deps_for_object ()
{
    local f
    f="$1"
    local object
    object="$2"

    local includes
    includes=$(grep '#include "' $f \
		      | sed 's/#include "//;s/"//' \
		      | sort)

    local deps
    deps=$(for i in $includes; do
	       handle_include "$i"
	   done \
	       | sort)

    local line
    line="$object:"

    local prefix
    prefix=""

    local limit
    limit=78

    for d in $deps; do
	if [ $(echo "$line $d" | wc -c) -gt $limit ]; then
	    echo "$prefix$line \\"
	    prefix="	"
	    line="$d"
	    limit=$(($limit - 8))
	else
	    line="$line $d"
	fi
    done

    if [ "$line" != "" ]; then
	echo "$prefix$line"
    fi	  
}

gen_deps_for_lo ()
{
    local object
    object="$1"

    local f
    case $object in
	xcoff_32.lo)
	    f=xcoff.c
	    ;;
	xcoff_64.lo)
	    f=xcoff.c
	    ;;
	*)
	    f=${object%.lo}.c
	    ;;
    esac

    if [ ! -f $f ]; then
	echo "$f not found"
	exit 1
    fi

    gen_deps_for_object $f $object
}

gen_deps_for_los ()
{
    for object in $los; do
	gen_deps_for_lo $object
    done
}

gen_deps_for_test_with_flags ()
{
    local test
    test="$1"

    local test_sources
    test_sources=$(grep "^${test}_SOURCES" Makefile.evaluated \
			  | sed 's/.* = //')

    local source
    for source in $test_sources; do
	local obj1
	obj1=$test-${source%.c}.o

	local obj2
	obj2=$test-${source%.c}.obj

	gen_deps_for_object $source $obj1
	gen_deps_for_object $source $obj2
    done
}

gen_deps_for_tests_with_flags ()
{
    local test
    for test in $tests; do
	if grep -q "^${test}_CFLAGS" Makefile.evaluated; then
	    gen_deps_for_test_with_flags $test
	fi
    done
}

gen_deps_for_tests_without_flags ()
{
    local sources
    sources=""
    
    local test
    for test in $tests; do
	if grep -q "^${test}_CFLAGS" Makefile.evaluated; then
	    continue
	fi
	local test_sources
	test_sources=$(grep "^${test}_SOURCES" Makefile.evaluated \
			      | sed 's/.* = //')
	sources="$sources $test_sources"
    done

    sources=$(echo $sources \
		 | sed 's/ /\n/g' \
		 | sort -u)

    for source in $sources; do
	local obj1
	obj1=${source%.c}.o

	local obj2
	obj2=${source%.c}.obj

	case $source in
	    edtest2_build.c)
		src=edtest2.c
		;;
	    *)
		src=$source
		;;
	esac

	gen_deps_for_object $src $obj1
	gen_deps_for_object $src $obj2
    done
}

main ()
{
    evaluate_makefile

    get_los
    gen_deps_for_los

    get_tests
    gen_deps_for_tests_without_flags
    gen_deps_for_tests_with_flags

    rm -f Makefile.evaluated
}

main
