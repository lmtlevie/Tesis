################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/sink/ev_count.cpp \
../atomics/sink/ev_count2.cpp \
../atomics/sink/gnuplot.cpp \
../atomics/sink/lcd.cpp \
../atomics/sink/logtoscilabtool.cpp \
../atomics/sink/qscope.cpp \
../atomics/sink/qss2idsk.cpp \
../atomics/sink/qss3idsk.cpp \
../atomics/sink/scilabcommands.cpp \
../atomics/sink/speaker.cpp \
../atomics/sink/time_count.cpp \
../atomics/sink/to_disk.cpp \
../atomics/sink/to_disk3.cpp \
../atomics/sink/to_disk_var.cpp \
../atomics/sink/to_diskrt.cpp \
../atomics/sink/toscilab.cpp \
../atomics/sink/toscilab_offline.cpp \
../atomics/sink/towav.cpp \
../atomics/sink/writelpt.cpp \
../atomics/sink/wxscope.cpp 

OBJS += \
./atomics/sink/ev_count.o \
./atomics/sink/ev_count2.o \
./atomics/sink/gnuplot.o \
./atomics/sink/lcd.o \
./atomics/sink/logtoscilabtool.o \
./atomics/sink/qscope.o \
./atomics/sink/qss2idsk.o \
./atomics/sink/qss3idsk.o \
./atomics/sink/scilabcommands.o \
./atomics/sink/speaker.o \
./atomics/sink/time_count.o \
./atomics/sink/to_disk.o \
./atomics/sink/to_disk3.o \
./atomics/sink/to_disk_var.o \
./atomics/sink/to_diskrt.o \
./atomics/sink/toscilab.o \
./atomics/sink/toscilab_offline.o \
./atomics/sink/towav.o \
./atomics/sink/writelpt.o \
./atomics/sink/wxscope.o 

CPP_DEPS += \
./atomics/sink/ev_count.d \
./atomics/sink/ev_count2.d \
./atomics/sink/gnuplot.d \
./atomics/sink/lcd.d \
./atomics/sink/logtoscilabtool.d \
./atomics/sink/qscope.d \
./atomics/sink/qss2idsk.d \
./atomics/sink/qss3idsk.d \
./atomics/sink/scilabcommands.d \
./atomics/sink/speaker.d \
./atomics/sink/time_count.d \
./atomics/sink/to_disk.d \
./atomics/sink/to_disk3.d \
./atomics/sink/to_disk_var.d \
./atomics/sink/to_diskrt.d \
./atomics/sink/toscilab.d \
./atomics/sink/toscilab_offline.d \
./atomics/sink/towav.d \
./atomics/sink/writelpt.d \
./atomics/sink/wxscope.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/sink/%.o: ../atomics/sink/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


