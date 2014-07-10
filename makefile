#######################################################################
# Directories
#######################################################################
TOP_DIR		 = .

include	$(TOP_DIR)/incs.mk

#######################################################################
# Target
#######################################################################
TGT			 = bbi$(EXE)

SRCS		 =
SRCS		+= bbi.cpp
SRCS		+= bbi_code.cpp
SRCS		+= bbi_misc.cpp
SRCS		+= bbi_pars.cpp
SRCS		+= bbi_tbl.cpp
SRCS		+= bbi_tkn.cpp

OBJS		 = $(foreach src, $(SRCS), $(OBJ_DIR)/$(src:.cpp=.o))

all :  $(TGT)


include	$(TOP_DIR)/rules.mk
