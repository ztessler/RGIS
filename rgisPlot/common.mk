UNIX=$(shell uname)

ifeq ($(UNIX),Darwin)
ifndef ($(CUSTOM_INC)
	CUSTOM_INC=-I/sw/include -I/sw/lib/pgplot
endif
ifndef ($(CUSTOM_LIB)
	CUSTOM_LIB=-L/sw/lib -L/sw/lib/pgplot -L/sw/lib/gcc4.8/lib -L/usr/X11R6/lib -Wl,-framework -Wl,Foundation -lgfortran -lz -laquaterm
endif
endif

export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE $(CUSTOM_INC)
export UNIXLIBS=$(CUSTOM_LIB) -lcpgplot -lpgplot -lpng -lX11 -lm
export UNIXMAKE=make

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif
