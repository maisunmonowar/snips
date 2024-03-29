# ========================================================================================
# Makefile for the DtRecord (LINUX)
# ========================================================================================

# ========================================================================================
# Detect number of bits used for long (i.e. 32-bit or 64-bit OS)
NUMLBITS = $(shell getconf LONG_BIT)

# ========================================================================================
# Set include directories. Make sure the include path to the DTAPI is correct

IMPORT = ./Import
DTAPIINCLUDE = $(IMPORT)/DTAPI
INCLUDEDIR = /usr/local/include -I$(DTAPIINCLUDE) -I$(IMPORT)

# ========================================================================================
# Compile flags

CPPFLAGS = -g -O2 -I$(INCLUDEDIR) -fexceptions \
	   -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE

# ========================================================================================
# TARGET/Source definitions

SRCDIR = ./Source/
TARGET = DtRecord
OBJS = $(TARGET).o
SRC = $(SRCDIR)DtRecord.cpp

# ========================================================================================
# Make all option. Make sure the path to DTPAI.o is set correct

DTAPILIB32 = $(DTAPIINCLUDE)/DTAPI.o
DTAPILIB64 = $(DTAPIINCLUDE)/DTAPI64.o

# Link with 32- or 64-bit DTAPI
ifeq ($(NUMLBITS), 64)
DTAPILIB   = $(DTAPILIB64)
else
DTAPILIB   = $(DTAPILIB32)
endif

LDLIBS += $(DTAPILIB)
# NOTE: need to link with PTHREAD library
LDFLAGS += -lpthread -ldl

all: $(TARGET)
$(TARGET): $(SRC:.cpp=.o)
		$(CXX) $(CPPFLAGS) $^ $(LDLIBS) $(LDFLAGS) -o $@

# ========================================================================================
# Clean all option

clean:
	rm -f $(SRCDIR)*.o $(SRCDIR)*~ $(SRCDIR)core $(SRCDIR).depend
	rm -f *.o *~ core .depend $(TARGET)


