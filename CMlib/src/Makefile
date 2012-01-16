include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
LIB    = ../lib
CC     = $(UNIXCC) $(UNIXCCOPS) -I$(INC)
TARGET = $(LIB)/libCM10.a

IPREF    = cm
INCLUDES = $(INC)/cm.h

OBJS = $(OBJ)/cmArgParse.o\
       $(OBJ)/cmBuffer.o\
       $(OBJ)/cmMath.o\
       $(OBJ)/cmMessage.o\
       $(OBJ)/cmString.o\
       $(OBJ)/cmThreads.o

all: $(TARGET)

install:
	cp $(INC)/$(IPREF)*.h $(INSTALLDIR)/include/
	cp $(TARGET)          $(INSTALLDIR)/lib/

uninstall:
	rm $(INSTALLDIR)/lib/$(TARGET)
	rm $(INSTALLDIR)/include/$(IPREF)*.h
clean:
	rm $(TARGET) $(OBJS)

$(TARGET): $(OBJS) $(SRC)/Makefile
	$(UNIXAR) $(TARGET) $(OBJS)
	$(UNIXRL) $(TARGET)

$(OBJ)/%.o: $(SRC)/%.c $(INCLUDES)
	$(CC) -o $(OBJ)/$*.o -c $(SRC)/$*.c