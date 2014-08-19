UNIX=$(shell uname)

ifeq ($(UNIX),Darwin)
ifndef ($(CUSTOM_INC))
	CUSTOM_INC=-I/sw/include
endif
ifndef ($(CUSTOM_LIB))
	CUSTOM_LIB=-L/sw/lib
endif
else
	PTHREAD_OPS=-pthread
endif

export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE $(CUSTOM_INC)
export UNIXLIBS=$(CUSTOM_LIB) ${PTHREAD_OPS} -lnetcdf -ludunits2 -lm
export UNIXMAKE=make

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif
