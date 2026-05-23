################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/qss/bqss.cpp \
../atomics/qss/command_sampler.cpp \
../atomics/qss/compare.cpp \
../atomics/qss/cqss.cpp \
../atomics/qss/cross_detect.cpp \
../atomics/qss/dqssv2o3.cpp \
../atomics/qss/hysteretic.cpp \
../atomics/qss/implicit.cpp \
../atomics/qss/inverse_function.cpp \
../atomics/qss/liqss.cpp \
../atomics/qss/liqss2.cpp \
../atomics/qss/liqss3.cpp \
../atomics/qss/liqss_integrator.cpp \
../atomics/qss/mathexpr.cpp \
../atomics/qss/pure_integrator.cpp \
../atomics/qss/qss.cpp \
../atomics/qss/qss2.cpp \
../atomics/qss/qss3.cpp \
../atomics/qss/qss4.cpp \
../atomics/qss/qss_delay.cpp \
../atomics/qss/qss_gain.cpp \
../atomics/qss/qss_integrator.cpp \
../atomics/qss/qss_multiplier.cpp \
../atomics/qss/qss_multiplier_step.cpp \
../atomics/qss/qss_nlfunction.cpp \
../atomics/qss/qss_nlfunction_step.cpp \
../atomics/qss/qss_quantizer.cpp \
../atomics/qss/qss_saturation.cpp \
../atomics/qss/qss_saturation1.cpp \
../atomics/qss/qss_switch.cpp \
../atomics/qss/qss_wsum.cpp \
../atomics/qss/qsstools.cpp \
../atomics/qss/res_qss_integrator.cpp \
../atomics/qss/samplehold.cpp \
../atomics/qss/secant_solve.cpp \
../atomics/qss/sin_function.cpp \
../atomics/qss/sin_function_step.cpp \
../atomics/qss/xpower2.cpp \
../atomics/qss/xpowern.cpp 

OBJS += \
./atomics/qss/bqss.o \
./atomics/qss/command_sampler.o \
./atomics/qss/compare.o \
./atomics/qss/cqss.o \
./atomics/qss/cross_detect.o \
./atomics/qss/dqssv2o3.o \
./atomics/qss/hysteretic.o \
./atomics/qss/implicit.o \
./atomics/qss/inverse_function.o \
./atomics/qss/liqss.o \
./atomics/qss/liqss2.o \
./atomics/qss/liqss3.o \
./atomics/qss/liqss_integrator.o \
./atomics/qss/mathexpr.o \
./atomics/qss/pure_integrator.o \
./atomics/qss/qss.o \
./atomics/qss/qss2.o \
./atomics/qss/qss3.o \
./atomics/qss/qss4.o \
./atomics/qss/qss_delay.o \
./atomics/qss/qss_gain.o \
./atomics/qss/qss_integrator.o \
./atomics/qss/qss_multiplier.o \
./atomics/qss/qss_multiplier_step.o \
./atomics/qss/qss_nlfunction.o \
./atomics/qss/qss_nlfunction_step.o \
./atomics/qss/qss_quantizer.o \
./atomics/qss/qss_saturation.o \
./atomics/qss/qss_saturation1.o \
./atomics/qss/qss_switch.o \
./atomics/qss/qss_wsum.o \
./atomics/qss/qsstools.o \
./atomics/qss/res_qss_integrator.o \
./atomics/qss/samplehold.o \
./atomics/qss/secant_solve.o \
./atomics/qss/sin_function.o \
./atomics/qss/sin_function_step.o \
./atomics/qss/xpower2.o \
./atomics/qss/xpowern.o 

CPP_DEPS += \
./atomics/qss/bqss.d \
./atomics/qss/command_sampler.d \
./atomics/qss/compare.d \
./atomics/qss/cqss.d \
./atomics/qss/cross_detect.d \
./atomics/qss/dqssv2o3.d \
./atomics/qss/hysteretic.d \
./atomics/qss/implicit.d \
./atomics/qss/inverse_function.d \
./atomics/qss/liqss.d \
./atomics/qss/liqss2.d \
./atomics/qss/liqss3.d \
./atomics/qss/liqss_integrator.d \
./atomics/qss/mathexpr.d \
./atomics/qss/pure_integrator.d \
./atomics/qss/qss.d \
./atomics/qss/qss2.d \
./atomics/qss/qss3.d \
./atomics/qss/qss4.d \
./atomics/qss/qss_delay.d \
./atomics/qss/qss_gain.d \
./atomics/qss/qss_integrator.d \
./atomics/qss/qss_multiplier.d \
./atomics/qss/qss_multiplier_step.d \
./atomics/qss/qss_nlfunction.d \
./atomics/qss/qss_nlfunction_step.d \
./atomics/qss/qss_quantizer.d \
./atomics/qss/qss_saturation.d \
./atomics/qss/qss_saturation1.d \
./atomics/qss/qss_switch.d \
./atomics/qss/qss_wsum.d \
./atomics/qss/qsstools.d \
./atomics/qss/res_qss_integrator.d \
./atomics/qss/samplehold.d \
./atomics/qss/secant_solve.d \
./atomics/qss/sin_function.d \
./atomics/qss/sin_function_step.d \
./atomics/qss/xpower2.d \
./atomics/qss/xpowern.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/qss/%.o: ../atomics/qss/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


