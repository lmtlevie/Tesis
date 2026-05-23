################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/vector/hyst_vec.cpp \
../atomics/vector/index_mapper.cpp \
../atomics/vector/index_selector.cpp \
../atomics/vector/index_shift.cpp \
../atomics/vector/index_shift_d.cpp \
../atomics/vector/indpar.cpp \
../atomics/vector/matrix_gain.cpp \
../atomics/vector/normal_gen_vec.cpp \
../atomics/vector/qss_int_vec.cpp \
../atomics/vector/qss_integrator_vec.cpp \
../atomics/vector/qss_multiplier_vec.cpp \
../atomics/vector/qss_sum_vec.cpp \
../atomics/vector/scalar2vec.cpp \
../atomics/vector/vec2scalar.cpp \
../atomics/vector/vector_commsampler.cpp \
../atomics/vector/vector_cross_det.cpp \
../atomics/vector/vector_pow2.cpp \
../atomics/vector/vector_sample.cpp \
../atomics/vector/vector_sat.cpp \
../atomics/vector/vector_sum.cpp \
../atomics/vector/vector_unitdelay.cpp 

OBJS += \
./atomics/vector/hyst_vec.o \
./atomics/vector/index_mapper.o \
./atomics/vector/index_selector.o \
./atomics/vector/index_shift.o \
./atomics/vector/index_shift_d.o \
./atomics/vector/indpar.o \
./atomics/vector/matrix_gain.o \
./atomics/vector/normal_gen_vec.o \
./atomics/vector/qss_int_vec.o \
./atomics/vector/qss_integrator_vec.o \
./atomics/vector/qss_multiplier_vec.o \
./atomics/vector/qss_sum_vec.o \
./atomics/vector/scalar2vec.o \
./atomics/vector/vec2scalar.o \
./atomics/vector/vector_commsampler.o \
./atomics/vector/vector_cross_det.o \
./atomics/vector/vector_pow2.o \
./atomics/vector/vector_sample.o \
./atomics/vector/vector_sat.o \
./atomics/vector/vector_sum.o \
./atomics/vector/vector_unitdelay.o 

CPP_DEPS += \
./atomics/vector/hyst_vec.d \
./atomics/vector/index_mapper.d \
./atomics/vector/index_selector.d \
./atomics/vector/index_shift.d \
./atomics/vector/index_shift_d.d \
./atomics/vector/indpar.d \
./atomics/vector/matrix_gain.d \
./atomics/vector/normal_gen_vec.d \
./atomics/vector/qss_int_vec.d \
./atomics/vector/qss_integrator_vec.d \
./atomics/vector/qss_multiplier_vec.d \
./atomics/vector/qss_sum_vec.d \
./atomics/vector/scalar2vec.d \
./atomics/vector/vec2scalar.d \
./atomics/vector/vector_commsampler.d \
./atomics/vector/vector_cross_det.d \
./atomics/vector/vector_pow2.d \
./atomics/vector/vector_sample.d \
./atomics/vector/vector_sat.d \
./atomics/vector/vector_sum.d \
./atomics/vector/vector_unitdelay.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/vector/%.o: ../atomics/vector/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


