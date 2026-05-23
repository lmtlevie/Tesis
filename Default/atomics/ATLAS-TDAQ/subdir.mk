################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../atomics/ATLAS-TDAQ/DcmApp.cpp \
../atomics/ATLAS-TDAQ/DcmLoadSampler.cpp \
../atomics/ATLAS-TDAQ/EBProcessing.cpp \
../atomics/ATLAS-TDAQ/EBRequester.cpp \
../atomics/ATLAS-TDAQ/EventBuilder.cpp \
../atomics/ATLAS-TDAQ/HLTSupervisor.cpp \
../atomics/ATLAS-TDAQ/L1TConfiguration.cpp \
../atomics/ATLAS-TDAQ/L2TProcessing.cpp \
../atomics/ATLAS-TDAQ/L2TRequester.cpp \
../atomics/ATLAS-TDAQ/NetworkConfiguration.cpp \
../atomics/ATLAS-TDAQ/PacketNetworkServer.cpp \
../atomics/ATLAS-TDAQ/PacketQueue.cpp \
../atomics/ATLAS-TDAQ/PacketRetransmission.cpp \
../atomics/ATLAS-TDAQ/PacketSender.cpp \
../atomics/ATLAS-TDAQ/ProtocolHeader2IndexVector.cpp \
../atomics/ATLAS-TDAQ/RosApp.cpp \
../atomics/ATLAS-TDAQ/ScalarHltsv2DcmLoadSamplerVector.cpp \
../atomics/ATLAS-TDAQ/ScalarHltsvReq2Vector.cpp \
../atomics/ATLAS-TDAQ/ScalarIp2TcpSender.cpp \
../atomics/ATLAS-TDAQ/ScalarIpDst2IndexVector.cpp \
../atomics/ATLAS-TDAQ/ScalarIpSrc2IndexVector.cpp \
../atomics/ATLAS-TDAQ/ScalarSimulator.cpp \
../atomics/ATLAS-TDAQ/ScilabSampler.cpp \
../atomics/ATLAS-TDAQ/TcpAck.cpp \
../atomics/ATLAS-TDAQ/TcpPacketization.cpp \
../atomics/ATLAS-TDAQ/TcpReceiverBuffer.cpp \
../atomics/ATLAS-TDAQ/TcpSender.cpp \
../atomics/ATLAS-TDAQ/Vector2Scalar.cpp \
../atomics/ATLAS-TDAQ/VectorBondedLinkVector.cpp \
../atomics/ATLAS-TDAQ/VectorIndexMod2Vector.cpp \
../atomics/ATLAS-TDAQ/VectorIp2TcpSenderVector.cpp \
../atomics/ATLAS-TDAQ/VectorIpDst2IndexVector.cpp \
../atomics/ATLAS-TDAQ/VectorIpSrc2IndexVector.cpp \
../atomics/ATLAS-TDAQ/VectorPu2DcmVector.cpp \
../atomics/ATLAS-TDAQ/VectorRos2ToRIndex.cpp \
../atomics/ATLAS-TDAQ/VectorRosCoreLink2CoreQueueVector.cpp \
../atomics/ATLAS-TDAQ/VectorTcpDst2IndexVector.cpp \
../atomics/ATLAS-TDAQ/Vector_DcmApp.cpp \
../atomics/ATLAS-TDAQ/Vector_DcmLoadSampler.cpp \
../atomics/ATLAS-TDAQ/Vector_EBProcessing.cpp \
../atomics/ATLAS-TDAQ/Vector_EBRequester.cpp \
../atomics/ATLAS-TDAQ/Vector_EventBuilder.cpp \
../atomics/ATLAS-TDAQ/Vector_L2TProcessing.cpp \
../atomics/ATLAS-TDAQ/Vector_L2TRequester.cpp \
../atomics/ATLAS-TDAQ/Vector_PacketGen.cpp \
../atomics/ATLAS-TDAQ/Vector_PacketNetworkServer.cpp \
../atomics/ATLAS-TDAQ/Vector_PacketQueue.cpp \
../atomics/ATLAS-TDAQ/Vector_RosApp.cpp \
../atomics/ATLAS-TDAQ/Vector_ScilabSampler.cpp \
../atomics/ATLAS-TDAQ/Vector_TcpAck.cpp \
../atomics/ATLAS-TDAQ/Vector_TcpPacketization.cpp \
../atomics/ATLAS-TDAQ/Vector_TcpReceiverBuffer.cpp \
../atomics/ATLAS-TDAQ/Vector_TcpSender.cpp \
../atomics/ATLAS-TDAQ/Vector_packetDiscard.cpp \
../atomics/ATLAS-TDAQ/globals.cpp \
../atomics/ATLAS-TDAQ/multipleSimulationCommands.cpp \
../atomics/ATLAS-TDAQ/packetDiscard.cpp \
../atomics/ATLAS-TDAQ/packetdemux.cpp \
../atomics/ATLAS-TDAQ/packetgen.cpp \
../atomics/ATLAS-TDAQ/packetsink.cpp \
../atomics/ATLAS-TDAQ/tdaqPacketTool.cpp 

OBJS += \
./atomics/ATLAS-TDAQ/DcmApp.o \
./atomics/ATLAS-TDAQ/DcmLoadSampler.o \
./atomics/ATLAS-TDAQ/EBProcessing.o \
./atomics/ATLAS-TDAQ/EBRequester.o \
./atomics/ATLAS-TDAQ/EventBuilder.o \
./atomics/ATLAS-TDAQ/HLTSupervisor.o \
./atomics/ATLAS-TDAQ/L1TConfiguration.o \
./atomics/ATLAS-TDAQ/L2TProcessing.o \
./atomics/ATLAS-TDAQ/L2TRequester.o \
./atomics/ATLAS-TDAQ/NetworkConfiguration.o \
./atomics/ATLAS-TDAQ/PacketNetworkServer.o \
./atomics/ATLAS-TDAQ/PacketQueue.o \
./atomics/ATLAS-TDAQ/PacketRetransmission.o \
./atomics/ATLAS-TDAQ/PacketSender.o \
./atomics/ATLAS-TDAQ/ProtocolHeader2IndexVector.o \
./atomics/ATLAS-TDAQ/RosApp.o \
./atomics/ATLAS-TDAQ/ScalarHltsv2DcmLoadSamplerVector.o \
./atomics/ATLAS-TDAQ/ScalarHltsvReq2Vector.o \
./atomics/ATLAS-TDAQ/ScalarIp2TcpSender.o \
./atomics/ATLAS-TDAQ/ScalarIpDst2IndexVector.o \
./atomics/ATLAS-TDAQ/ScalarIpSrc2IndexVector.o \
./atomics/ATLAS-TDAQ/ScalarSimulator.o \
./atomics/ATLAS-TDAQ/ScilabSampler.o \
./atomics/ATLAS-TDAQ/TcpAck.o \
./atomics/ATLAS-TDAQ/TcpPacketization.o \
./atomics/ATLAS-TDAQ/TcpReceiverBuffer.o \
./atomics/ATLAS-TDAQ/TcpSender.o \
./atomics/ATLAS-TDAQ/Vector2Scalar.o \
./atomics/ATLAS-TDAQ/VectorBondedLinkVector.o \
./atomics/ATLAS-TDAQ/VectorIndexMod2Vector.o \
./atomics/ATLAS-TDAQ/VectorIp2TcpSenderVector.o \
./atomics/ATLAS-TDAQ/VectorIpDst2IndexVector.o \
./atomics/ATLAS-TDAQ/VectorIpSrc2IndexVector.o \
./atomics/ATLAS-TDAQ/VectorPu2DcmVector.o \
./atomics/ATLAS-TDAQ/VectorRos2ToRIndex.o \
./atomics/ATLAS-TDAQ/VectorRosCoreLink2CoreQueueVector.o \
./atomics/ATLAS-TDAQ/VectorTcpDst2IndexVector.o \
./atomics/ATLAS-TDAQ/Vector_DcmApp.o \
./atomics/ATLAS-TDAQ/Vector_DcmLoadSampler.o \
./atomics/ATLAS-TDAQ/Vector_EBProcessing.o \
./atomics/ATLAS-TDAQ/Vector_EBRequester.o \
./atomics/ATLAS-TDAQ/Vector_EventBuilder.o \
./atomics/ATLAS-TDAQ/Vector_L2TProcessing.o \
./atomics/ATLAS-TDAQ/Vector_L2TRequester.o \
./atomics/ATLAS-TDAQ/Vector_PacketGen.o \
./atomics/ATLAS-TDAQ/Vector_PacketNetworkServer.o \
./atomics/ATLAS-TDAQ/Vector_PacketQueue.o \
./atomics/ATLAS-TDAQ/Vector_RosApp.o \
./atomics/ATLAS-TDAQ/Vector_ScilabSampler.o \
./atomics/ATLAS-TDAQ/Vector_TcpAck.o \
./atomics/ATLAS-TDAQ/Vector_TcpPacketization.o \
./atomics/ATLAS-TDAQ/Vector_TcpReceiverBuffer.o \
./atomics/ATLAS-TDAQ/Vector_TcpSender.o \
./atomics/ATLAS-TDAQ/Vector_packetDiscard.o \
./atomics/ATLAS-TDAQ/globals.o \
./atomics/ATLAS-TDAQ/multipleSimulationCommands.o \
./atomics/ATLAS-TDAQ/packetDiscard.o \
./atomics/ATLAS-TDAQ/packetdemux.o \
./atomics/ATLAS-TDAQ/packetgen.o \
./atomics/ATLAS-TDAQ/packetsink.o \
./atomics/ATLAS-TDAQ/tdaqPacketTool.o 

CPP_DEPS += \
./atomics/ATLAS-TDAQ/DcmApp.d \
./atomics/ATLAS-TDAQ/DcmLoadSampler.d \
./atomics/ATLAS-TDAQ/EBProcessing.d \
./atomics/ATLAS-TDAQ/EBRequester.d \
./atomics/ATLAS-TDAQ/EventBuilder.d \
./atomics/ATLAS-TDAQ/HLTSupervisor.d \
./atomics/ATLAS-TDAQ/L1TConfiguration.d \
./atomics/ATLAS-TDAQ/L2TProcessing.d \
./atomics/ATLAS-TDAQ/L2TRequester.d \
./atomics/ATLAS-TDAQ/NetworkConfiguration.d \
./atomics/ATLAS-TDAQ/PacketNetworkServer.d \
./atomics/ATLAS-TDAQ/PacketQueue.d \
./atomics/ATLAS-TDAQ/PacketRetransmission.d \
./atomics/ATLAS-TDAQ/PacketSender.d \
./atomics/ATLAS-TDAQ/ProtocolHeader2IndexVector.d \
./atomics/ATLAS-TDAQ/RosApp.d \
./atomics/ATLAS-TDAQ/ScalarHltsv2DcmLoadSamplerVector.d \
./atomics/ATLAS-TDAQ/ScalarHltsvReq2Vector.d \
./atomics/ATLAS-TDAQ/ScalarIp2TcpSender.d \
./atomics/ATLAS-TDAQ/ScalarIpDst2IndexVector.d \
./atomics/ATLAS-TDAQ/ScalarIpSrc2IndexVector.d \
./atomics/ATLAS-TDAQ/ScalarSimulator.d \
./atomics/ATLAS-TDAQ/ScilabSampler.d \
./atomics/ATLAS-TDAQ/TcpAck.d \
./atomics/ATLAS-TDAQ/TcpPacketization.d \
./atomics/ATLAS-TDAQ/TcpReceiverBuffer.d \
./atomics/ATLAS-TDAQ/TcpSender.d \
./atomics/ATLAS-TDAQ/Vector2Scalar.d \
./atomics/ATLAS-TDAQ/VectorBondedLinkVector.d \
./atomics/ATLAS-TDAQ/VectorIndexMod2Vector.d \
./atomics/ATLAS-TDAQ/VectorIp2TcpSenderVector.d \
./atomics/ATLAS-TDAQ/VectorIpDst2IndexVector.d \
./atomics/ATLAS-TDAQ/VectorIpSrc2IndexVector.d \
./atomics/ATLAS-TDAQ/VectorPu2DcmVector.d \
./atomics/ATLAS-TDAQ/VectorRos2ToRIndex.d \
./atomics/ATLAS-TDAQ/VectorRosCoreLink2CoreQueueVector.d \
./atomics/ATLAS-TDAQ/VectorTcpDst2IndexVector.d \
./atomics/ATLAS-TDAQ/Vector_DcmApp.d \
./atomics/ATLAS-TDAQ/Vector_DcmLoadSampler.d \
./atomics/ATLAS-TDAQ/Vector_EBProcessing.d \
./atomics/ATLAS-TDAQ/Vector_EBRequester.d \
./atomics/ATLAS-TDAQ/Vector_EventBuilder.d \
./atomics/ATLAS-TDAQ/Vector_L2TProcessing.d \
./atomics/ATLAS-TDAQ/Vector_L2TRequester.d \
./atomics/ATLAS-TDAQ/Vector_PacketGen.d \
./atomics/ATLAS-TDAQ/Vector_PacketNetworkServer.d \
./atomics/ATLAS-TDAQ/Vector_PacketQueue.d \
./atomics/ATLAS-TDAQ/Vector_RosApp.d \
./atomics/ATLAS-TDAQ/Vector_ScilabSampler.d \
./atomics/ATLAS-TDAQ/Vector_TcpAck.d \
./atomics/ATLAS-TDAQ/Vector_TcpPacketization.d \
./atomics/ATLAS-TDAQ/Vector_TcpReceiverBuffer.d \
./atomics/ATLAS-TDAQ/Vector_TcpSender.d \
./atomics/ATLAS-TDAQ/Vector_packetDiscard.d \
./atomics/ATLAS-TDAQ/globals.d \
./atomics/ATLAS-TDAQ/multipleSimulationCommands.d \
./atomics/ATLAS-TDAQ/packetDiscard.d \
./atomics/ATLAS-TDAQ/packetdemux.d \
./atomics/ATLAS-TDAQ/packetgen.d \
./atomics/ATLAS-TDAQ/packetsink.d \
./atomics/ATLAS-TDAQ/tdaqPacketTool.d 


# Each subdirectory must supply rules for building sources it contributes
atomics/ATLAS-TDAQ/%.o: ../atomics/ATLAS-TDAQ/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I/usr/local/include -I"/home/mbonaven/tdaq-simulation/engine" -I/usr/local/include/c++/4.8.2/bits -O2 -g -Wall -c -fmessage-length=0 std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


