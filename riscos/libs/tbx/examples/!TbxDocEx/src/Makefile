# Makefile for TBX Doc Ex

# If you have fix deps uncomment following line for dependency scanning
# include FixDeps:Rules/make

CXXFLAGS = -O2 -Wall -mpoke-function-name -mthrowback -ITBX:

LD = g++
LDFLAGS = -LTBX: -ltbx -lstdc++ -static

CCSRC = $(wildcard *.cc)
OBJECTS = $(CCSRC:.cc=.o)

^.!RunImage:	!RunImage,e1f
	elf2aif !RunImage,e1f ^.!RunImage
!RunImage,e1f:	$(OBJECTS)
	$(LD) $(LDFLAGS) -o !RunImage,e1f $(OBJECTS)

clean:
	rm -f $(OBJECTS) ^.!RunImage !RunImage,e1f

# IF you have fix deps uncomment following line
# #include $(CCSRC:.cc=.d)
