################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/pdme_v2/moc/moc_dlg_change_annotation.cpp \
../src/pdme_v2/moc/moc_dlg_change_library.cpp \
../src/pdme_v2/moc/moc_dlg_change_parameters.cpp \
../src/pdme_v2/moc/moc_dlg_change_priority.cpp \
../src/pdme_v2/moc/moc_dlg_edit_list_parameter.cpp \
../src/pdme_v2/moc/moc_dlg_edit_model.cpp \
../src/pdme_v2/moc/moc_dlg_menu_edit.cpp \
../src/pdme_v2/moc/moc_dlg_menu_setup.cpp \
../src/pdme_v2/moc/moc_dlg_set_path_tools.cpp \
../src/pdme_v2/moc/moc_gpx_atomic.cpp \
../src/pdme_v2/moc/moc_gpx_block.cpp \
../src/pdme_v2/moc/moc_gpx_coupled.cpp \
../src/pdme_v2/moc/moc_gpx_edition_scene.cpp \
../src/pdme_v2/moc/moc_gpx_edition_window.cpp \
../src/pdme_v2/moc/moc_gpx_inport.cpp \
../src/pdme_v2/moc/moc_gpx_outport.cpp \
../src/pdme_v2/moc/moc_gpx_textbox.cpp \
../src/pdme_v2/moc/moc_powergui.cpp 

OBJS += \
./src/pdme_v2/moc/moc_dlg_change_annotation.o \
./src/pdme_v2/moc/moc_dlg_change_library.o \
./src/pdme_v2/moc/moc_dlg_change_parameters.o \
./src/pdme_v2/moc/moc_dlg_change_priority.o \
./src/pdme_v2/moc/moc_dlg_edit_list_parameter.o \
./src/pdme_v2/moc/moc_dlg_edit_model.o \
./src/pdme_v2/moc/moc_dlg_menu_edit.o \
./src/pdme_v2/moc/moc_dlg_menu_setup.o \
./src/pdme_v2/moc/moc_dlg_set_path_tools.o \
./src/pdme_v2/moc/moc_gpx_atomic.o \
./src/pdme_v2/moc/moc_gpx_block.o \
./src/pdme_v2/moc/moc_gpx_coupled.o \
./src/pdme_v2/moc/moc_gpx_edition_scene.o \
./src/pdme_v2/moc/moc_gpx_edition_window.o \
./src/pdme_v2/moc/moc_gpx_inport.o \
./src/pdme_v2/moc/moc_gpx_outport.o \
./src/pdme_v2/moc/moc_gpx_textbox.o \
./src/pdme_v2/moc/moc_powergui.o 

CPP_DEPS += \
./src/pdme_v2/moc/moc_dlg_change_annotation.d \
./src/pdme_v2/moc/moc_dlg_change_library.d \
./src/pdme_v2/moc/moc_dlg_change_parameters.d \
./src/pdme_v2/moc/moc_dlg_change_priority.d \
./src/pdme_v2/moc/moc_dlg_edit_list_parameter.d \
./src/pdme_v2/moc/moc_dlg_edit_model.d \
./src/pdme_v2/moc/moc_dlg_menu_edit.d \
./src/pdme_v2/moc/moc_dlg_menu_setup.d \
./src/pdme_v2/moc/moc_dlg_set_path_tools.d \
./src/pdme_v2/moc/moc_gpx_atomic.d \
./src/pdme_v2/moc/moc_gpx_block.d \
./src/pdme_v2/moc/moc_gpx_coupled.d \
./src/pdme_v2/moc/moc_gpx_edition_scene.d \
./src/pdme_v2/moc/moc_gpx_edition_window.d \
./src/pdme_v2/moc/moc_gpx_inport.d \
./src/pdme_v2/moc/moc_gpx_outport.d \
./src/pdme_v2/moc/moc_gpx_textbox.d \
./src/pdme_v2/moc/moc_powergui.d 


# Each subdirectory must supply rules for building sources it contributes
src/pdme_v2/moc/%.o: ../src/pdme_v2/moc/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


