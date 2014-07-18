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
SRCS		+= bbi.c
SRCS		+= bbi_parse.c
SRCS		+= bbi_token.c
SRCS		+= bbi_util.c
SRCS		+= bbi_fio.c
SRCS		+= bbi_symbt.c

OBJS		 = $(foreach src, $(SRCS), $(OBJ_DIR)/$(src:.c=.o))

all :  $(TGT)


include	$(TOP_DIR)/rules.mk
