################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/sources/Trapezoidal.cpp \
../atomics/sources/constant.cpp \
../atomics/sources/constant_sci.cpp \
../atomics/sources/fromwav.cpp \
../atomics/sources/fromworkspace.cpp \
../atomics/sources/gen_wgn.cpp \
../atomics/sources/pulse.cpp \
../atomics/sources/pulse_sci.cpp \
../atomics/sources/pwmgen.cpp \
../atomics/sources/pwmsignal.cpp \
../atomics/sources/qss2ramp.cpp \
../atomics/sources/qss2sin.cpp \
../atomics/sources/ramp.cpp \
../atomics/sources/ramp_sci.cpp \
../atomics/sources/saw.cpp \
../atomics/sources/saw_sci.cpp \
../atomics/sources/scuaren.cpp \
../atomics/sources/sinegen.cpp \
../atomics/sources/sinegen_sci.cpp \
../atomics/sources/singen.cpp \
../atomics/sources/square.cpp \
../atomics/sources/square_sci.cpp \
../atomics/sources/step.cpp \
../atomics/sources/step13.cpp \
../atomics/sources/step_sci.cpp \
../atomics/sources/trapesoidal.cpp \
../atomics/sources/trapezoidal_sci.cpp \
../atomics/sources/triangular.cpp \
../atomics/sources/triangular_sci.cpp \
../atomics/sources/wavegenerator.cpp 

OBJS += \
./atomics/sources/Trapezoidal.o \
./atomics/sources/constant.o \
./atomics/sources/constant_sci.o \
./atomics/sources/fromwav.o \
./atomics/sources/fromworkspace.o \
./atomics/sources/gen_wgn.o \
./atomics/sources/pulse.o \
./atomics/sources/pulse_sci.o \
./atomics/sources/pwmgen.o \
./atomics/sources/pwmsignal.o \
./atomics/sources/qss2ramp.o \
./atomics/sources/qss2sin.o \
./atomics/sources/ramp.o \
./atomics/sources/ramp_sci.o \
./atomics/sources/saw.o \
./atomics/sources/saw_sci.o \
./atomics/sources/scuaren.o \
./atomics/sources/sinegen.o \
./atomics/sources/sinegen_sci.o \
./atomics/sources/singen.o \
./atomics/sources/square.o \
./atomics/sources/square_sci.o \
./atomics/sources/step.o \
./atomics/sources/step13.o \
./atomics/sources/step_sci.o \
./atomics/sources/trapesoidal.o \
./atomics/sources/trapezoidal_sci.o \
./atomics/sources/triangular.o \
./atomics/sources/triangular_sci.o \
./atomics/sources/wavegenerator.o 

CPP_DEPS += \
./atomics/sources/Trapezoidal.d \
./atomics/sources/constant.d \
./atomics/sources/constant_sci.d \
./atomics/sources/fromwav.d \
./atomics/sources/fromworkspace.d \
./atomics/sources/gen_wgn.d \
./atomics/sources/pulse.d \
./atomics/sources/pulse_sci.d \
./atomics/sources/pwmgen.d \
./atomics/sources/pwmsignal.d \
./atomics/sources/qss2ramp.d \
./atomics/sources/qss2sin.d \
./atomics/sources/ramp.d \
./atomics/sources/ramp_sci.d \
./atomics/sources/saw.d \
./atomics/sources/saw_sci.d \
./atomics/sources/scuaren.d \
./atomics/sources/sinegen.d \
./atomics/sources/sinegen_sci.d \
./atomics/sources/singen.d \
./atomics/sources/square.d \
./atomics/sources/square_sci.d \
./atomics/sources/step.d \
./atomics/sources/step13.d \
./atomics/sources/step_sci.d \
./atomics/sources/trapesoidal.d \
./atomics/sources/trapezoidal_sci.d \
./atomics/sources/triangular.d \
./atomics/sources/triangular_sci.d \
./atomics/sources/wavegenerator.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/sources/%.o: ../atomics/sources/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


