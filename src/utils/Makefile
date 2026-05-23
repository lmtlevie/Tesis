include ../Makefile.defs

LIB_DIR = ../../build/lib
LIBUTILS = $(LIB_DIR)/libutils.so

OBJ_DIR = ../../build/objs/utils

SRC_FILES = $(wildcard *.cpp)

LIBS =
INCLUDES = -I..

include ../Makefile.rules

all: $(LIBUTILS)

clean:
	@rm -rf $(LIBUTILS) $(DEP_FILES) $(OBJS_W_DIR)

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
sinclude $(DEP_FILES)
endif
