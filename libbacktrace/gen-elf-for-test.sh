#!/bin/sh

sed="$1"
build_id_dir="$2"
src="$3"
dst="$4"

tmp=$dst.tmp

search='^#define SYSTEM_BUILD_ID_DIR.*$'

replace="#define SYSTEM_BUILD_ID_DIR \"$build_id_dir\""

$sed \
    "s%$search%$replace%" \
    $src \
    > $tmp

mv $tmp $dst
