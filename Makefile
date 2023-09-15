## file guifltk-refpersys/Makefile for GNU make
##  © Copyright 2023 Basile Starynkevitch
##
CXX= g++
ASTYLE= astyle
ASTYLEFLAGS= -v  --style=gnu
DESTDIR= /usr/local
RM= /bin/rm -vf
GIT_ID:= $(shell ./do-generate-gitid.sh)
SHORTGIT_ID:= $(shell ./do-generate-gitid.sh -s)
CXXFLAGS= -O2 -g -I /usr/local/include/ \
          $(shell pkg-config --cflags  jsoncpp) \
          $(shell fltk-config --cxxflags) \
	  -DGIT_ID=\"$(GIT_ID)\" -DSHORTGIT_ID=\"$(SHORTGIT_ID)\" \
	  -DBUILD_HOST=\"$(shell hostname -f)\"



################################################################
## the install target is installing in $DESTDIR/bin
## the homeinstall target is installing in $HOME/bin


.PHONY: all objects clean indent homeinstall install


all: guifltkrps

clean:
	$(RM) *.o *~ *.orig guifltkrps a.out

indent:
	for f in *.hh ; do  $(ASTYLE) $(ASTYLEFLAGS) $$f ; done
	for f in *.cc ; do  $(ASTYLE) $(ASTYLEFLAGS) $$f ; done

homeinstall: guifltkrps
	install  --backup --preserve-timestamps  guifltkrps $$HOME/bin/

install: guifltkrps
	sudo /usr/bin/install  --backup  --preserve-timestamps  guifltkrps $(DESTDIR)/bin/

guifltkrps: progfltk.o jsonrpsfltk.o
	$(LINK.cc) -o $@ -O2 -g mainfltk.o jsonrpsfltk.o \
	           $(shell pkg-config --cflags jsoncpp) \
                   $(shell fltk-config  --ldflags) 

progfltk.o: progfltk.cc fltkrps.hh

jsonrpsfltk.o: jsonrpsfltk.cc fltkrps.hh

#### end of guifltk-refpersys/Makefile
