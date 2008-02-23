UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated
export UNIXMAKE=make
export UNIXLIBS=-ludunits -lnetcdf -lXm -lXt -lXext -lX11 -lm
endif
ifeq ($(UNIX),Darwin)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated -I/sw/include
export UNIXMAKE=make
export UNIXLIBS=-L/usr/X11R6/lib -L/sw/lib -lXm -lXt -lXext -lX11 -ludunits -lnetcdf
endif
ifeq ($(UNIX),SunOS)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -Wno-deprecated
export UNIXMAKE=make
export UNIXLIBS=-L/usr/local/netcdf/lib -L/usr/local/udunits/lib -lXm -lXt -lXext -lX11 -ludunits -lnetcdf
endif

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif
