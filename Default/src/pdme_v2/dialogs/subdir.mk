################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/pdme_v2/dialogs/dlg_change_annotation.cpp \
../src/pdme_v2/dialogs/dlg_change_library.cpp \
../src/pdme_v2/dialogs/dlg_change_parameters.cpp \
../src/pdme_v2/dialogs/dlg_change_priority.cpp \
../src/pdme_v2/dialogs/dlg_edit_list_parameter.cpp \
../src/pdme_v2/dialogs/dlg_edit_model.cpp \
../src/pdme_v2/dialogs/dlg_menu_edit.cpp \
../src/pdme_v2/dialogs/dlg_menu_setup.cpp \
../src/pdme_v2/dialogs/dlg_set_path_tools.cpp 

OBJS += \
./src/pdme_v2/dialogs/dlg_change_annotation.o \
./src/pdme_v2/dialogs/dlg_change_library.o \
./src/pdme_v2/dialogs/dlg_change_parameters.o \
./src/pdme_v2/dialogs/dlg_change_priority.o \
./src/pdme_v2/dialogs/dlg_edit_list_parameter.o \
./src/pdme_v2/dialogs/dlg_edit_model.o \
./src/pdme_v2/dialogs/dlg_menu_edit.o \
./src/pdme_v2/dialogs/dlg_menu_setup.o \
./src/pdme_v2/dialogs/dlg_set_path_tools.o 

CPP_DEPS += \
./src/pdme_v2/dialogs/dlg_change_annotation.d \
./src/pdme_v2/dialogs/dlg_change_library.d \
./src/pdme_v2/dialogs/dlg_change_parameters.d \
./src/pdme_v2/dialogs/dlg_change_priority.d \
./src/pdme_v2/dialogs/dlg_edit_list_parameter.d \
./src/pdme_v2/dialogs/dlg_edit_model.d \
./src/pdme_v2/dialogs/dlg_menu_edit.d \
./src/pdme_v2/dialogs/dlg_menu_setup.d \
./src/pdme_v2/dialogs/dlg_set_path_tools.d 


# Each subdirectory must supply rules for building sources it contributes
src/pdme_v2/dialogs/%.o: ../src/pdme_v2/dialogs/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


