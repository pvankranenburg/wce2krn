#!/bin/sh
PLATFORM=`uname`

if [ "$PLATFORM" = "Darwin" ]; then
	make -f Makefile.osx $@
else
	make -f Makefile.linux $@
fi

