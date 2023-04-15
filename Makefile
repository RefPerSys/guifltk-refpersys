## file guifltk-refpersys/Makefile
##  © Copyright 2023 Basile Starynkevitch
##
CXX= g++
ASTYLE= astyle
ASTYLEFLAGS= -v  --style=gnu 
RM= /bin/rm -vf
GIT_ID:= $(shell ./do-generate-gitid.sh)
SHORTGIT_ID:= $(shell ./do-generate-gitid.sh -s)

CXXFLAGS= -O2 -g -I /usr/local/include/ \
          $(shell pkg-config --cflags  jsoncpp) \
          $(shell fltk-config --cxxflags) \
	  -DGIT_ID=\"$(GIT_ID)\" -DSHORTGIT_ID=\"$(SHORTGIT_ID)\" 


.PHONY: all objects clean indent


all: guifltkrps

clean:
	$(RM) *.o *~ *.orig guifltkrps a.out

indent:
	for f in *.hh ; do  $(ASTYLE) $(ASTYLEFLAGS) $$f ; done
	for f in *.cc ; do  $(ASTYLE) $(ASTYLEFLAGS) $$f ; done

guifltkrps: mainfltk.o jsonrpsfltk.o
	$(LINK.cc) -o $@ -O2 -g mainfltk.o jsonrpsfltk.o \
	           $(shell pkg-config --cflags jsoncpp) \
                   $(shell fltk-config  --ldflags) 
