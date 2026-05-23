include ../src/Makefile.defs
include Makefile.user

LIB_PATH = lib
LIBENGINE = $(LIB_PATH)/libengine.so
LIBMODELS = $(LIB_PATH)/libmodels.so

ATOMICS = ../atomics
SRC_PATH = ../src
ENGINE_PATH = $(SRC_PATH)/engine/common
MODEL_PATH = $(SRC_PATH)/model
UTILS_PATH = $(SRC_PATH)/utils
INCLUDES = -I$(SRC_PATH) -I$(ENGINE_PATH) -I$(ATOMICS) -I$(UTILS_PATH) -Iinclude -I. -I/usr/include/python2.7/

MODEL_SRC = $(wildcard $(MODEL_PATH)/*.cpp)

#ifeq ($(shell lsb_release -si),Ubuntu)
ifeq ($(shell grep -oP '^ID=\K.+' /etc/os-release),ubuntu)
    LIBS = -lengine -lmodels -lutils -lhdf5_serial -lboost_program_options -lboost_system -lboost_python -lpython2.7 -lgsl -lgslcblas -Wl,-rpath=$(shell pwd)/$(LIB_PATH)
#else ifeq ($(shell lsb_release -si),Debian)
else ifeq ($(shell grep -oP '^ID=\K.+' /etc/os-release),debian)
    LIBS = -lengine -lmodels -lutils -lhdf5_serial -lboost_program_options -lboost_system -lboost_python -lpython2.7 -lgsl -lgslcblas -Wl,-rpath=$(shell pwd)/$(LIB_PATH)
else
    LIBS = -lengine -lmodels -lutils -lhdf5 -lboost_program_options -lboost_system -lboost_python -lpython2.7 -lgsl -lgslcblas -Wl,-rpath=$(shell pwd)/$(LIB_PATH)
endif

MODEL = ../output/model

MODEL_H_EXISTS := $(wildcard model.h)

# Only compile if model.h exists
ifeq ($(MODEL_H_EXISTS),model.h)
$(MODEL): $(MODEL_SRC) model.h $(LIBENGINE) $(LIBMODELS)
	$(CXX) $(CXXFLAGS) $(USERCXXFLAGS) $(INCLUDES) $(USERINCLUDES) $(MODEL_SRC) -L$(LIB_PATH) $(LIBS) $(USERLIBS) -o $(MODEL)
else
    $(MODEL):
		@echo "Skipping the compilation of a specific model. 'output/model' executable will not be produced because 'model.h' was not found inside the 'build' directory."
endif

clean:
	rm -rf $(MODEL)
