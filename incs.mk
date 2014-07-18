.PHONY : ccdv tags

ifeq ($(OSTYPE), cygwin)
EXE			 = .exe
else
EXE			 =
endif

UTIL_DIR	 = $(TOP_DIR)/utils
OBJ_DIR		 = $(TOP_DIR)/objs

INCS		 =
INCS		+= -I$(TOP_DIR)
INCS		+= -I$(TOP_DIR)/include
INCS		+= -I.

#######################################################################
# Build Options
#######################################################################
CXXFLAGS	 =
CXXFLAGS	+= -O0
CXXFLAGS	+= -g
CXXFLAGS	+= $(INCS)
CXXFLAGS	+= -Wall
CXXFLAGS	+= -Wno-switch
CXXFLAGS	+= -Wno-char-subscripts

include	$(TOP_DIR)/tools.mk

all: $(CCDV)

tags:
	@$(ECHO) "[MAKE TAGS]"
	@ctags -R
