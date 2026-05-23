################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm

# All of the sources participating in the build are defined here
-include sources.mk
-include src/uname/subdir.mk
-include src/pdppt/subdir.mk
-include src/pdme_v2/moc/subdir.mk
-include src/pdme_v2/graphics/subdir.mk
-include src/pdme_v2/dialogs/subdir.mk
-include src/pdme_v2/subdir.mk
-include src/pdif/subdir.mk
-include src/pdae/subdir.mk
-include src/common/test_parser/subdir.mk
-include src/common/parser/subdir.mk
-include src/common/data/subdir.mk
-include src/atomics/rtview/qtsingleapplication/src/subdir.mk
-include src/atomics/rtview/qtsingleapplication/examples/trivial/subdir.mk
-include src/atomics/rtview/qtsingleapplication/examples/loader/subdir.mk
-include src/atomics/rtview/qtsingleapplication/examples/console/subdir.mk
-include src/atomics/rtview/subdir.mk
-include src/atomics/lcd/subdir.mk
-include src/atomics/knob/subdir.mk
-include engine/subdir.mk
-include build/objs/subdir.mk
-include bin/BackDoor/src/c/subdir.mk
-include bin/BackDoor/sci_gateway/c/subdir.mk
-include atomics/vector/subdir.mk
-include atomics/sources/subdir.mk
-include atomics/source/subdir.mk
-include atomics/sinks/subdir.mk
-include atomics/sink/subdir.mk
-include atomics/signal/subdir.mk
-include atomics/realtime/subdir.mk
-include atomics/random/lib/subdir.mk
-include atomics/random/subdir.mk
-include atomics/queue/subdir.mk
-include atomics/qss/subdir.mk
-include atomics/petri/subdir.mk
-include atomics/modelica/subdir.mk
-include atomics/hybrid/subdir.mk
-include atomics/discrete/subdir.mk
-include atomics/control/subdir.mk
-include atomics/continuous/subdir.mk
-include atomics/adquisicion/subdir.mk
-include atomics/Matias/subdir.mk
-include atomics/ATLAS-TDAQ/OLD/subdir.mk
-include atomics/ATLAS-TDAQ/Helpers/subdir.mk
-include atomics/ATLAS-TDAQ/subdir.mk
-include atomics/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: tdaq-simulation

# Tool invocations
tdaq-simulation: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o "tdaq-simulation" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) tdaq-simulation
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
