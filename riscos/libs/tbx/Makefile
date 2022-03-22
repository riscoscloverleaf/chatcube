# Makefile for TBX Library

# include FixDeps:Rules/make

CXX=g++
CXXFLAGS=-O2 -Wall -mthrowback
CPPFLAGS=-MMD -MP
AR=ar

TARGET=libtbx.a

CCSRC = $(wildcard tbx/*.cc) $(wildcard tbx/view/*.cc) $(wildcard tbx/res/*.cc) $(wildcard tbx/doc/*.cc) $(wildcard tbx/ext/*.cc)


bin:	$(TARGET)

doc:
	doxygen tbx.doxyfile

all:	bin doc

$(TARGET): $(CCSRC:.cc=.o)
	$(AR) -rcs $(TARGET) $(CCSRC:.cc=.o)

clean:
	rm -f $(CCSRC:.cc=.o) $(CCSRC:.cc=.d) $(TARGET)

copytoapp: all
	mkdir -p !TBX/tbx/h
	copy tbx.h.* !TBX.tbx.h.* ~CF
	copy libtbx/a !TBX.libtbx/a ~CF
	mkdir -p !TBX/html
	copy docs.html.* !TBX.html.* ~CF

cleanapp:
	rm -rf !TBX/tbx
	rm -rf !TBX/html
	IfThere !TBX.libtbx/a Then Delete !TBX.libtbx/a

install:
	./install-cross


-include $(CCSRC:.cc=.d)

