#!/bin/sh

log1=printdwarftest_dwz.1.log
log2=printdwarftest_dwz.2.log

./printdwarftest 2> $log1
./printdwarftest_dwz 2> $log2
cmp $log1 $log2
