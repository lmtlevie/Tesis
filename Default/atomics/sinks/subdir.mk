################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/sinks/ev_count.cpp \
../atomics/sinks/ev_count2.cpp \
../atomics/sinks/gnuplot.cpp \
../atomics/sinks/lcd.cpp \
../atomics/sinks/logtoscilabtool.cpp \
../atomics/sinks/qscope.cpp \
../atomics/sinks/qss2idsk.cpp \
../atomics/sinks/qss3idsk.cpp \
../atomics/sinks/scilabcommands.cpp \
../atomics/sinks/speaker.cpp \
../atomics/sinks/time_count.cpp \
../atomics/sinks/to_disk.cpp \
../atomics/sinks/to_disk3.cpp \
../atomics/sinks/to_disk_var.cpp \
../atomics/sinks/to_diskrt.cpp \
../atomics/sinks/toscilab.cpp \
../atomics/sinks/toscilab_offline.cpp \
../atomics/sinks/towav.cpp \
../atomics/sinks/writelpt.cpp \
../atomics/sinks/wxscope.cpp 

OBJS += \
./atomics/sinks/ev_count.o \
./atomics/sinks/ev_count2.o \
./atomics/sinks/gnuplot.o \
./atomics/sinks/lcd.o \
./atomics/sinks/logtoscilabtool.o \
./atomics/sinks/qscope.o \
./atomics/sinks/qss2idsk.o \
./atomics/sinks/qss3idsk.o \
./atomics/sinks/scilabcommands.o \
./atomics/sinks/speaker.o \
./atomics/sinks/time_count.o \
./atomics/sinks/to_disk.o \
./atomics/sinks/to_disk3.o \
./atomics/sinks/to_disk_var.o \
./atomics/sinks/to_diskrt.o \
./atomics/sinks/toscilab.o \
./atomics/sinks/toscilab_offline.o \
./atomics/sinks/towav.o \
./atomics/sinks/writelpt.o \
./atomics/sinks/wxscope.o 

CPP_DEPS += \
./atomics/sinks/ev_count.d \
./atomics/sinks/ev_count2.d \
./atomics/sinks/gnuplot.d \
./atomics/sinks/lcd.d \
./atomics/sinks/logtoscilabtool.d \
./atomics/sinks/qscope.d \
./atomics/sinks/qss2idsk.d \
./atomics/sinks/qss3idsk.d \
./atomics/sinks/scilabcommands.d \
./atomics/sinks/speaker.d \
./atomics/sinks/time_count.d \
./atomics/sinks/to_disk.d \
./atomics/sinks/to_disk3.d \
./atomics/sinks/to_disk_var.d \
./atomics/sinks/to_diskrt.d \
./atomics/sinks/toscilab.d \
./atomics/sinks/toscilab_offline.d \
./atomics/sinks/towav.d \
./atomics/sinks/writelpt.d \
./atomics/sinks/wxscope.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/sinks/%.o: ../atomics/sinks/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


