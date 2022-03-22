# Makefile for TBX Minimal application

CXX=g++
CXXFLAGS=-O2 -ITBX:

LD=g++
LDFLAGS=-LTBX: -ltbx -static

TARGET=^.!RunImage
TARGETELF=tbxmin

OBJS=tbxmin.o

all: $(TARGET)

$(TARGET):	$(TARGETELF)
	elf2aif $(TARGETELF) $(TARGET)

$(TARGETELF):	$(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(TARGETELF)

clean:
	rm -f $(OBJS) $(TARGETELF) $(TARGET)
