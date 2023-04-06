## file guifltk-refpersys/Makefile
##  © Copyright 2023 Basile Starynkevitch
##
CXX= g++

GIT_ID:= $(shell ./do-generate-gitid.sh)
SHORTGIT_ID:= $(shell ./do-generate-gitid.sh -s)

CXXFLAGS= -O2 -g -I /usr/local/include/ \
          $(shell pkg-config jsoncpp) \
          $(shell fltk-config --cxxflags) \
	  -DGIT_ID=\"$(GIT_ID)\" -DSHORTGIT_ID=\"$(SHORTGIT_ID)\" 

.PHONY: all objects clean


