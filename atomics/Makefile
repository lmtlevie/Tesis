include ../src/Makefile.defs

LIB_DIR = ../build/lib
INCLUDE_DIR = ../build/include

LIBMODELS = $(LIB_DIR)/libmodels.so

ATOMICS = $(shell find * -maxdepth 5 -type d -not -path modelica -not -path "unitTest*")
SRC_PATH = ../src
ENGINE_PATH = $(SRC_PATH)/engine/common
UTILS_PATH = $(SRC_PATH)/utils

LIBS = -L$(LIB_DIR) -lutils

# lsb_release is not available in all linux distros, instead use os-release
#ifeq ($(shell lsb_release -si),Ubuntu)
ifeq ($(shell grep -oP '^ID=\K.+' /etc/os-release),ubuntu)
    INCLUDES = $(addprefix -I, $(ATOMICS)) -I. -I$(SRC_PATH) -I$(ENGINE_PATH) -I$(UTILS_PATH) -I$(INCLUDE_DIR) -I/usr/include/hdf5/serial/ -I/usr/include/python2.7/
#else ifeq ($(shell lsb_release -si),Debian)
else ifeq ($(shell grep -oP '^ID=\K.+' /etc/os-release),debian)
    INCLUDES = $(addprefix -I, $(ATOMICS)) -I. -I$(SRC_PATH) -I$(ENGINE_PATH) -I$(UTILS_PATH) -I$(INCLUDE_DIR) -I/usr/include/hdf5/serial/ -I/usr/include/python2.7/
else
    INCLUDES = $(addprefix -I, $(ATOMICS)) -I. -I$(SRC_PATH) -I$(ENGINE_PATH) -I$(UTILS_PATH) -I$(INCLUDE_DIR) 
endif

SRC_FILES = $(foreach subdir, $(ATOMICS), $(wildcard $(subdir)/*.cpp))
OBJ_DIR = ../build/objs/atomics

include ../src/Makefile.rules

all: $(LIBMODELS)

clean:
	@rm -rf $(LIBMODELS) $(DEP_FILES) $(OBJS_W_DIR)

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
sinclude $(DEP_FILES)
endif
