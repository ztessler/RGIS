UNIX=$(shell uname)

ifeq ($(UNIX),Darwin)
ifndef ($(CUSTOM_INC))
	CUSTOM_INC=-I/opt/X11/include -I/sw/include
endif
ifndef ($(CUSTOM_LIB))
	CUSTOM_LIB=-L/usr/X11R6/lib -L/sw/lib
endif
endif

export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE $(CUSTOM_INC)
export UNIXLIBS=$(CUSTOM_LIB) -lXm -lXt -lXext -lX11 -lnetcdf -ludunits2
export UNIXMAKE=make

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif
