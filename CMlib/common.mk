UNIX=$(shell uname)

export UNIXAR=ar -ru
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -pthread
export UNIXMAKE=make
export UNIXRL=ranlib
