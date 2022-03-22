# Makefile for TBX Reverse example application

CXX=g++
CXXFLAGS=-O2 -ITBX: -mthrowback

LD=g++
LDFLAGS=-LTBX: -ltbx -static

TARGET=^.!RunImage
TARGETELF=reverse

CCSRC=main.cc reverser.cc

OBJS=$(CCSRC:.cc=.o)

all: $(TARGET)

$(TARGET):	$(TARGETELF)
	elf2aif $(TARGETELF) $(TARGET)

$(TARGETELF):	$(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(TARGETELF)

clean:
	rm -f $(OBJS) $(TARGETELF) $(TARGET)
