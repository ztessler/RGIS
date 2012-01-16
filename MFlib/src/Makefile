include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
LIB    = ../lib
CC     = $(UNIXCC) $(UNIXCCOPS) -I$(INC) -I../../CMlib/include
LINK   = $(UNIXCC)
TARGET = $(LIB)/libMF10.a

IPREF    = MF
INCLUDES = $(INC)/MF.h

OBJS = $(OBJ)/MFConfig.o\
       $(OBJ)/MFDate.o\
       $(OBJ)/MFOptions.o\
       $(OBJ)/MFVariables.o\
       $(OBJ)/MFDataStream.o\
       $(OBJ)/MFDefinition.o\
       $(OBJ)/MFDomain.o\
       $(OBJ)/MFModel.o\
       $(OBJ)/MFSwap.o\
       $(OBJ)/MFRungeKutta.o\

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