#!/bin/make
SHELL=/bin/bash

#Define the virtual paths
vpath %.cpp src/
vpath %.hpp include/
vpath %.o obj/

#Set some of the compile options
CXX = g++
CXXFLAGS = -fPIC -g -Wall $(CINCLUDEDIRS)
LDLIBS = -ldl -lpthread -lGnuplotPipes
CINCLUDEDIRS = -Iinclude
c++SrcSuf = cpp

#Set the verbosity
#VERBOSE = 1
ifdef VERBOSE
CXXFLAGS += -DVERBOSE
endif

#Set the name of the program to be compiled
PROGRAM = salesman

#Define Objects
MAINO = salesman.o

#Make the object list and prefix the object directory
OBJS = $(MAINO)
OBJDIR = obj
OBJS_W_DIR = $(addprefix $(OBJDIR)/,$(OBJS))

.SUFFIXES: .$(c++SrcSuf)

all: $(OBJS_W_DIR) $(PROGRAM)

$(OBJS_W_DIR): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(PROGRAM): $(OBJS_W_DIR)
	$(CXX) $(CXXFLAGS) $(LDLIBS) $^ -o $@

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean: 
	@echo "Cleaning..."
	@rm -f $(OBJDIR)/* $(PROGRAM) *~ src/*~ include/*~
