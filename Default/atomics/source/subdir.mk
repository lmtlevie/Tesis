################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/source/Trapezoidal.cpp \
../atomics/source/constant.cpp \
../atomics/source/constant_sci.cpp \
../atomics/source/fromwav.cpp \
../atomics/source/fromworkspace.cpp \
../atomics/source/gen_wgn.cpp \
../atomics/source/pulse.cpp \
../atomics/source/pulse_sci.cpp \
../atomics/source/pwmgen.cpp \
../atomics/source/pwmsignal.cpp \
../atomics/source/qss2ramp.cpp \
../atomics/source/qss2sin.cpp \
../atomics/source/ramp.cpp \
../atomics/source/ramp_sci.cpp \
../atomics/source/saw.cpp \
../atomics/source/saw_sci.cpp \
../atomics/source/scuaren.cpp \
../atomics/source/sinegen.cpp \
../atomics/source/sinegen_sci.cpp \
../atomics/source/singen.cpp \
../atomics/source/square.cpp \
../atomics/source/square_sci.cpp \
../atomics/source/step.cpp \
../atomics/source/step13.cpp \
../atomics/source/step_sci.cpp \
../atomics/source/trapesoidal.cpp \
../atomics/source/trapezoidal_sci.cpp \
../atomics/source/triangular.cpp \
../atomics/source/triangular_sci.cpp \
../atomics/source/wavegenerator.cpp 

OBJS += \
./atomics/source/Trapezoidal.o \
./atomics/source/constant.o \
./atomics/source/constant_sci.o \
./atomics/source/fromwav.o \
./atomics/source/fromworkspace.o \
./atomics/source/gen_wgn.o \
./atomics/source/pulse.o \
./atomics/source/pulse_sci.o \
./atomics/source/pwmgen.o \
./atomics/source/pwmsignal.o \
./atomics/source/qss2ramp.o \
./atomics/source/qss2sin.o \
./atomics/source/ramp.o \
./atomics/source/ramp_sci.o \
./atomics/source/saw.o \
./atomics/source/saw_sci.o \
./atomics/source/scuaren.o \
./atomics/source/sinegen.o \
./atomics/source/sinegen_sci.o \
./atomics/source/singen.o \
./atomics/source/square.o \
./atomics/source/square_sci.o \
./atomics/source/step.o \
./atomics/source/step13.o \
./atomics/source/step_sci.o \
./atomics/source/trapesoidal.o \
./atomics/source/trapezoidal_sci.o \
./atomics/source/triangular.o \
./atomics/source/triangular_sci.o \
./atomics/source/wavegenerator.o 

CPP_DEPS += \
./atomics/source/Trapezoidal.d \
./atomics/source/constant.d \
./atomics/source/constant_sci.d \
./atomics/source/fromwav.d \
./atomics/source/fromworkspace.d \
./atomics/source/gen_wgn.d \
./atomics/source/pulse.d \
./atomics/source/pulse_sci.d \
./atomics/source/pwmgen.d \
./atomics/source/pwmsignal.d \
./atomics/source/qss2ramp.d \
./atomics/source/qss2sin.d \
./atomics/source/ramp.d \
./atomics/source/ramp_sci.d \
./atomics/source/saw.d \
./atomics/source/saw_sci.d \
./atomics/source/scuaren.d \
./atomics/source/sinegen.d \
./atomics/source/sinegen_sci.d \
./atomics/source/singen.d \
./atomics/source/square.d \
./atomics/source/square_sci.d \
./atomics/source/step.d \
./atomics/source/step13.d \
./atomics/source/step_sci.d \
./atomics/source/trapesoidal.d \
./atomics/source/trapezoidal_sci.d \
./atomics/source/triangular.d \
./atomics/source/triangular_sci.d \
./atomics/source/wavegenerator.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/source/%.o: ../atomics/source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


