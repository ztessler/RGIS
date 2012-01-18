include ./common.mk
OBJ = ./obj
LIB = ./lib

all: $(OBJ) $(LIB) source

install:
	$(UNIXMAKE) -C src install
	cp ./Scripts/*.sh $(INSTALLDIR)/Scripts/

uninstall:
	$(UNIXMAKE) -C src uninstall
	rm $(INSTALLDIR)/Scripts/fwFunctions*.sh

clean:
	$(UNIXMAKE) -C src clean

source:
	$(UNIXMAKE) -C src all

$(OBJ):
	mkdir -p $(OBJ)
$(LIB):
	mkdir -p $(LIB)
