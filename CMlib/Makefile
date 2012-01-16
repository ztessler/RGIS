include ./common.mk
OBJ = ./obj
LIB = ./lib

all: $(OBJ) $(LIB) source

install:
	$(UNIXMAKE) -C src install

uninstall:
	$(UNIXMAKE) -C src uninstall

clean:
	$(UNIXMAKE) -C src clean

source:
	$(UNIXMAKE) -C src all

$(OBJ):
	mkdir -p $(OBJ)
$(LIB):
	mkdir -p $(LIB)
