include ../Makefile.defs

####### WINDOWS ############
# TODO: broken
ifeq ($(UNAME), Windows)
OPTFLAGS = -O3
CXXFLAGS = -Wall $(OPTFLAGS) -D__WINDOWS__ -std=c++11
ATOMICS = ..\atomics
SRCENGINE = ..\engine
BUILD = ..\build
BUILDOBJ = ..\build\objs
BUILDLIB = ..\build\lib
OUTPUT = ..\output
GSLLIB = /usr/lib
GSLINC = /usr/include/gsl
MODEL = $(OUTPUT)\model.exe
RM = del
MV = copy 
OBJS = $(BUILDOBJ)\*.o
LIBS = -lsimpd -lws2_32 -lgsl -L$(GSLLIB)
INCLUDES = -I$(SRCENGINE) -I$(ATOMICS) -I$(BUILD) -L$(BUILDLIB) -I$(GSLINC)
all : $(BUILDLIB)/libsimpd.a $(MODEL)
clean:
	$(RM) /q /s  $(OBJS) $(MODEL)
endif

####### LINUX ############
ifeq ($(UNAME), Linux)

LIB_DIR = ../../build/lib

LIBENGINE = $(LIB_DIR)/libengine.so

OBJ_DIR = ../../build/objs/engine

ATOMICS = ../../atomics
ENGINE_SRC = common
UTILS_PATH = ../src/utils

LIBS = -L$(LIB_DIR) -lutils
INCLUDES = -I$(ENGINE_SRC) -I$(ATOMICS) -I$(UTILS_PATH) -I/usr/include/python2.7

SRC_FILES = $(wildcard $(ENGINE_SRC)/*.cpp)
endif

include ../Makefile.rules

all: $(LIBENGINE)

clean:
	@rm -rf $(LIBENGINE) $(DEP_FILES) $(OBJS_W_DIR)

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
sinclude $(DEP_FILES)
endif
