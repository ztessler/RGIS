UNIX=$(shell uname)
ifndef GHAASDIR
export GHAASDIR=/usr/local/share/ghaas
endif

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

export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -pthread -fsigned-char -D_GNU_SOURCE $(CUSTOM_INC)
export UNIXLIBS=$(CUSTOM_LIB) ${PTHREAD_OPS} -lnetcdf -ludunits2 -lm
export UNIXMAKE=make
