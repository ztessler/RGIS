include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
LIB    = ../lib
CC     = $(UNIXCC) $(UNIXCCOPS) -I$(INC) -I../../CMlib/include
TARGET = $(LIB)/libNC10.a

IPREFIX  = NC
INCLUDES = $(INC)/NC.h\
           $(INC)/NCcore.h\
           $(INC)/NCdsHandle.h\
           $(INC)/NCnames.h\
           $(INC)/NCtable.h\
           $(INC)/NCtime.h\
           $(INC)/NCmath.h\
           $(INC)/NCmathEqtn.h\
           $(INC)/NCstdlib.h\
           $(INC)/NCstring.h

OBJS =  $(OBJ)/NCcoordinate.o\
        $(OBJ)/NCdata.o\
        $(OBJ)/NCdsHandle.o\
        $(OBJ)/NCdsHandleGeo.o\
        $(OBJ)/NCdsHandleGLayout.o\
        $(OBJ)/NCdsHandleGrid.o\
        $(OBJ)/NCdsHandleGCont.o\
        $(OBJ)/NCdsHandleNet.o\
        $(OBJ)/NCdsHandleVector.o\
        $(OBJ)/NCdsHandleVecPnt.o\
        $(OBJ)/NCfileCreate.o\
        $(OBJ)/NCgridSampling.o\
        $(OBJ)/NCmath.o\
        $(OBJ)/NCmathEqtn.o\
        $(OBJ)/NCnames.o\
        $(OBJ)/NCreference.o\
        $(OBJ)/NCregion.o\
        $(OBJ)/NCdate.o\
        $(OBJ)/NCtable.o\
        $(OBJ)/NCfileList.o\
        $(OBJ)/NCstdlib.o\
        $(OBJ)/NCstring.o

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