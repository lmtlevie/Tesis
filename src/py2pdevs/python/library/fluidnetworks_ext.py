import os
import sys

import collections
from .base_model import BaseModel 
from .fluidnetworks_highlevel import  TcpSender, TcpReceiver, Router, REDRouter
from .fluidnetworks_lowlevel import  TCP, REDBufferServer, PropagationDelay, REDEgressPort
from .qss_continuous_ext import SignalLoggerExt as SignalLogger
from .hybrid import Reservoir
from .common_ext import TwoWayRouter

INPORTS_DEFAULT = 1
OUTPORTS_DEFAULT = 1
N_FLOWS_DEFAULT = 1

class TcpReceiverExt(TcpReceiver):
    # NOTE: n_sessions parameter is not used. It is there to have the same parameters as the packet model
    def __init__(self, parent, name, n_sessions=None, log = True, **kwargs):
        TcpReceiver.__init__(self, parent, name, **kwargs)
        self.log = log

class TcpSenderExt(TcpSender):
    def __init__(self, parent, name, log = True, **kwargs):
        TcpSender.__init__(self, parent, name, **kwargs)
        self.log = log
        
    def _do_create_model(self):
        self._pd_model = self.interface.create_coupled(self.parent.pd_model(), self.name)
        
        # Child models
        ___999999_0 = self.interface.create_constant_sci(self._pd_model, "999999", ["99999"])
        __toBitssec_1 = self.interface.create_qss_multiplier(self._pd_model, "toBits/sec", ["Purely static", "1e-6", "1e-3"])
        __packetSize_2 = self.interface.create_constant_sci(self._pd_model, "packetSize", [self.meanPacketSize])
        __onoffbitssec_3 = self.interface.create_qss_multiplier(self._pd_model, "on/off bits/sec", ["Purely static", "1e-6", "1e-3"])
        __onoffRTT_4 = self.interface.create_qss_switch(self._pd_model, "on/off RTT", ["0"])
        __TCP_5 = TCPExt(self, "TCP", log = self.log, bandwidth = self.bandwidth, delay = self.delay, maxWindow = self.maxWindow, initialRTT = self.initialRTT, n_sessions_times = self.n_sessions_times, n_sessions = self.n_sessions, method = self.method, dQmin = self.dQmin, dQrel = self.dQrel)._create_model() # create an instance of 'TCP_5' coupled model 
        __onoff_6 = self.interface.create_customsteps(self._pd_model, "on/off", [self.on_off_times, self.on_off, "", "", "", ""])
        __packetSize__7 = self.interface.create_constant_sci(self._pd_model, "packetSize_", [self.meanPacketSize])
        __dropsSelect_8 = self.interface.create_flowdropsselector(self._pd_model, "dropsSelect", [])
        __acumDelaySelect_9 = self.interface.create_flowacumdelayselector(self._pd_model, "acumDelaySelect", [])
        __bandwidth_11 = self.interface.create_qss_saturation(self._pd_model, "bandwidth", ["0", self.bandwidth])
        __dropRate_pkts0_13 = self.interface.create_qss_nlfunction_step(self._pd_model, "dropRate_pkts0", ["u0*(1/u1)*(1/u2)", "3.000000e+00", "Purely static", self.dQmin, self.dQrel])
        __dropCalib_14 = self.interface.create_customsteps(self._pd_model, "dropCalib", [self.drops_calib_times, self.drops_calib, "", "", "", ""])
        __N_15 = self.interface.create_customsteps(self._pd_model, "N", [self.n_sessions_times, self.n_sessions, "", "", "", ""])
        __Ncalib0_16 = self.interface.create_qss_multiplier(self._pd_model, "N*calib0", ["Purely static", "1e-6", "1e-3"])
        __flowCreator_10 = self.interface.create_fluidflowcreator(self._pd_model, "flowCreator", [self.FlowName])
        __egress_port0_12 = TcpSender.egress_port0_12(self, "egress_port0", bandwidth = self.bandwidth, delay = self.delay, method = self.method, dQmin = self.dQmin, dQrel = self.dQrel)._create_model() # create an instance of 'egress_port0_12' coupled model 
#         __signalFilter_13 = self.interface.create_qss_signal_filter(self._pd_model, "filter", ["1", "1e-2"])
        
        if self.log:
            __w_17 = SignalLogger(self, "w")._create_model() # create an instance of 'w_17' coupled model 
            __rate_18 = SignalLogger(self, "rate", log_accumulated=True)._create_model() # create an instance of 'rate_18' coupled model
            self.interface.connect_models(__TCP_5, 0, __w_17, 0)
            self.interface.connect_models(__bandwidth_11, 0, __rate_18, 0) 
        
        
        # Connections        
        self.interface.connect_models(__TCP_5, 1, __toBitssec_1, 0)
        self.interface.connect_models(___999999_0, 0, __onoffRTT_4, 2)
        self.interface.connect_models(__onoffRTT_4, 0, __TCP_5, 0)
        self.interface.connect_models(__packetSize_2, 0, __toBitssec_1, 1)
        self.interface.connect_models(__dropRate_pkts0_13, 0, __TCP_5, 1)
        self.interface.connect_models(__packetSize__7, 0, __dropRate_pkts0_13, 2)
        self.interface.connect_models(__acumDelaySelect_9, 0, __onoffRTT_4, 0)
        self.interface.connect_models(__onoffbitssec_3, 0, __bandwidth_11, 0)
        self.interface.connect_models(__toBitssec_1, 0, __onoffbitssec_3, 1)
#         self.interface.connect_models(__toBitssec_1, 0, __signalFilter_13, 0)
#         self.interface.connect_models(__signalFilter_13, 0, __onoffbitssec_3, 1)
        self.interface.connect_models(__dropsSelect_8, 0, __dropRate_pkts0_13, 0)
        self.interface.connect_models(__dropCalib_14, 0, __Ncalib0_16, 0)
        self.interface.connect_models(__N_15, 0, __Ncalib0_16, 1)
        self.interface.connect_models(__Ncalib0_16, 0, __dropRate_pkts0_13, 1)
        self.interface.connect_models(__flowCreator_10, 0, __egress_port0_12, 0)
        self.interface.connect_models(__bandwidth_11, 0, __flowCreator_10, 0)
        self.interface.connect_models(__onoff_6, 0, __onoffbitssec_3, 0)
        self.interface.connect_models(__onoff_6, 0, __onoffRTT_4, 1)
        self.interface.connect_input_to_model(__dropsSelect_8, 0, 0)
        self.interface.connect_input_to_model(__acumDelaySelect_9, 0, 0)
        self.interface.connect_model_to_output(__egress_port0_12, 0, 0) 

        return self._pd_model
        

class TCPExt(TCP):
    def __init__(self, parent, name, log = True, **kwargs):
        TCP.__init__(self, parent, name, **kwargs)
        self.log = log
        
    def _do_create_model(self):
        TCP._do_create_model(self)
        
        if self.log:     
            logger_rtt= SignalLogger(self, "RTT")._create_model() 
            logger_drops= SignalLogger(self, "Drops")._create_model()
            self.interface.connect_input_to_model(logger_rtt, 0, 0)
            self.interface.connect_input_to_model(logger_drops, 0, 1)
            
            if self.log == "debug":
                logger_w_in= SignalLogger(self, "w_in")._create_model()
                logger_w_out= SignalLogger(self, "w_out")._create_model()
                self.interface.connect_models(self.submodels["___1RTTWt2bptr_3"], 0, logger_w_in, 0)
                self.interface.connect_models(self.submodels["__W_11"], 0, logger_w_out, 0)
            
        
        return self._pd_model      
        
class REDRouterExt(TwoWayRouter, REDRouter):
    def __init__(self, parent, name, in_ports=INPORTS_DEFAULT, out_ports=OUTPORTS_DEFAULT, n_flows=N_FLOWS_DEFAULT, log=True, **kwargs):
        TwoWayRouter.__init__(self)
        REDRouter.__init__(self, parent, name, **kwargs)
        self.in_ports = in_ports
        self.out_ports = out_ports
        self.n_flows = n_flows
        self.log = log
        self.bufferSizes = [self.bufferSize] * out_ports if out_ports > 0 else [self.bufferSize]
        self.bandwidths = [self.bandwidth] * out_ports  if out_ports > 0 else [self.bandwidth]
        
   
    def _do_create_model(self):
        self._pd_model = self.interface.create_coupled(self.parent.pd_model(), self.name)
        
        # Child models
        RoutingTable = self.interface.create_flowrouter(self._pd_model, "RoutingTable", [])
        
        # out ports
        for n in range(0, self.out_ports):
            egress_port = REDEgressPortExt(self, "RED_egress_port%d" % n,
                                                       log = self.log, n_flows=self.n_flows, bufferSize = self.bufferSizes[n], bandwidth = self.bandwidths[n], delay = self.delay, weight = self.weight, samplingPeriod = self.samplingPeriod, tmin = self.tmin, tmax = self.tmax, pmax = self.pmax, method = self.method, dQmin = self.dQmin, dQrel = self.dQrel)._create_model() # create an instance of 'RED_egress_port0_1' coupled model 
            self.interface.connect_models(RoutingTable, n, egress_port, 0)
            self.interface.connect_model_to_output(egress_port, 0, n)   
        
        # in ports
        for n in range(0, self.in_ports):   
             self.interface.connect_input_to_model(RoutingTable, n, n)

        return self._pd_model   
    
class REDEgressPortExt(REDEgressPort):
    def __init__(self, parent, name, n_flows=N_FLOWS_DEFAULT, log = True, **kwargs):
        REDEgressPort.__init__(self, parent, name, **kwargs)
        self.n_flows = n_flows
        self.log = log
        
    def _do_create_model(self):        
        self._pd_model = self.interface.create_coupled(self.parent.pd_model(), self.name)
        
        # Child models
        __propagation_delay_0 = PropagationDelayExt(self, "propagation_delay",
                                                                 n_flows=self.n_flows, delay = self.delay)._create_model() # create an instance of 'propagation_delay_0' coupled model 
        __RED_buffer_server_1 = REDBufferServerExt(self, "RED_buffer_server", 
                                                                n_flows=self.n_flows, log=self.log, bufferSize = self.bufferSize, bandwidth = self.bandwidth, weight = self.weight, samplingPeriod = self.samplingPeriod, tmin = self.tmin, tmax = self.tmax, pmax = self.pmax, method = self.method, dQmin = self.dQmin, dQrel = self.dQrel)._create_model() # create an instance of 'RED_buffer_server_1' coupled model 
        
        # Connections
        self.interface.connect_models(__RED_buffer_server_1, 0, __propagation_delay_0, 0)
        self.interface.connect_input_to_model(__RED_buffer_server_1, 0, 0)
        self.interface.connect_model_to_output(__propagation_delay_0, 0, 0) 

        if self.log == "debug":
            logger_out = SignalLogger(self, "out" )._create_model() # create an instance of 'rate_18' coupled model
            self.interface.connect_models(__RED_buffer_server_1, 0, logger_out, 0)
            logger_in = SignalLogger(self, "in")._create_model() # create an instance of 'rate_18' coupled model            
            self.interface.connect_input_to_model(logger_in, 0, 0)
            logger_delay_in = SignalLogger(self, "delay_in")._create_model() # create an instance of 'rate_18' coupled model            
            self.interface.connect_models(__propagation_delay_0, 0, logger_delay_in, 0)
        
        return self._pd_model
    
class REDBufferServerExt(REDBufferServer):
    def __init__(self, parent, name, n_flows = N_FLOWS_DEFAULT, log = True,  **kwargs):
        REDBufferServer.__init__(self, parent, name, **kwargs)
        self.n_flows = n_flows        
        self.log = log

    def _do_create_model(self):
        self._pd_model = self.interface.create_coupled(self.parent.pd_model(), self.name)
        
        # Child models
        __FlowDemux_0 = self.interface.create_fluidflowdynamicdemutiplexer(self._pd_model, "FlowDemux", [self.n_flows])
        __WMovingAvg_18 = REDBufferServer.WMovingAvg_18(self, "WMovingAvg", weight = self.weight, samplingPeriod = self.samplingPeriod, method = self.method, dQmin = self.dQmin, dQrel = self.dQrel)._create_model() # create an instance of 'WMovingAvg_18' coupled model 
        __REDDropProb_19 = REDBufferServer.REDDropProb_19(self, "REDDropProb", tmax = self.tmax, tmin = self.tmin, pmax = self.pmax)._create_model() # create an instance of 'REDDropProb_19' coupled model 
        
        # connections
        self.interface.connect_input_to_model(__FlowDemux_0, 0, 0)
        self.interface.connect_models(__WMovingAvg_18, 0, __REDDropProb_19, 0)
        
        # CHECK: in the original model the reserviour is in last priority!
        models = dict() # save created models for later use
        for flow in range(0, self.n_flows):
            submodels = dict()
            rateSelect = self.interface.create_flowrateselector(self._pd_model, "rateSelect%d" % flow, [])
            submodels["rateSelect"] = rateSelect
            acumDelaySelect = self.interface.create_flowacumdelayselector(self._pd_model, "acumDelaySelect%d" % flow, [])
            submodels["acumDelaySelect"] = acumDelaySelect
            delaySum = self.interface.create_qss_wsum(self._pd_model, "delaySum%d" % flow, ["1", "1", "0", "0", "0", "0", "0", "0", "2.000000e+00"])
            submodels["delaySum"] = delaySum
            dropsSelect = self.interface.create_flowdropsselector(self._pd_model, "dropsSelect%d" % flow, [])
            submodels["dropsSelect"] = dropsSelect
            dropSum = self.interface.create_qss_wsum(self._pd_model, "dropSum%d" % flow, ["1", "1", "1", "0", "0", "0", "0", "0", "3.000000e+00"])
            submodels["dropSum"] = dropSum
            REDDropRate = self.interface.create_qss_multiplier(self._pd_model, "REDDropRate%d" % flow, ["Purely static", self.dQmin, self.dQrel])
            submodels["REDDropRate"] = REDDropRate
            aminusREDDrops = self.interface.create_qss_wsum(self._pd_model, "a%d-REDDrops%d" % (flow, flow), ["-1", "1", "0", "0", "0", "0", "0", "0", "2.000000e+00"])
            submodels["aminusREDDrops"] = aminusREDDrops
           
        
            # Connections
            self.interface.connect_models(__FlowDemux_0, flow, acumDelaySelect, 0)
            self.interface.connect_models(__FlowDemux_0, flow, rateSelect, 0)
            self.interface.connect_models(__FlowDemux_0, flow, dropsSelect, 0)      
            
            self.interface.connect_models(acumDelaySelect, 0, delaySum, 1)
            self.interface.connect_models(dropsSelect, 0, dropSum, 0)                
            self.interface.connect_models(REDDropRate, 0, dropSum, 2)
            self.interface.connect_models(__REDDropProb_19, 0, REDDropRate, 0)
            self.interface.connect_models(rateSelect, 0, REDDropRate, 1)
            self.interface.connect_models(rateSelect, 0, aminusREDDrops, 1)                
            self.interface.connect_models(REDDropRate, 0, aminusREDDrops, 0)
            
            if self.log == "debug":
                logger_drops = SignalLogger(self, "drops%d" % flow)._create_model() # create an instance of 'rate_18' coupled model
                logger_delay = SignalLogger(self, "delay%d" % flow)._create_model() # create an instance of 'rate_18' coupled model
                self.interface.connect_models(dropSum, 0, logger_drops, 0)
                self.interface.connect_models(delaySum, 0, logger_delay, 0)
            
            models[flow] = submodels
        
        # reservoir with last priority
        __reservoir_17 = ReservoirExt(self, "reservoir", log=self.log, n_flows=self.n_flows,
                                      maxSize = self.bufferSize, capacity = self.bandwidth, method = self.method, dQmin = self.dQmin, dQrel = self.dQrel)._create_model() # create an instance of 'reservoir_17' coupled model
        self.interface.connect_model_to_output(__reservoir_17, 0, 1) 
        self.interface.connect_models(__reservoir_17, 0, __WMovingAvg_18, 0) 
        for flow in range(0, self.n_flows):
            submodels = models[flow]
            self.interface.connect_models(submodels["aminusREDDrops"], 0, __reservoir_17, flow)
            self.interface.connect_models(__reservoir_17, 3+(2*flow), submodels["dropSum"], 1) # drop rate
            self.interface.connect_models(__reservoir_17, 1, submodels["delaySum"], 0)  
        
        # flow composers with last priority
        for flow in range(0, self.n_flows):
            submodels = models[flow]
            flowComposer = self.interface.create_fluidflowcomposer(self._pd_model, "flowComposer%d" % flow , [])
            self.interface.connect_model_to_output(flowComposer, 0, 0)
            self.interface.connect_models(__reservoir_17, 2+(2*flow), flowComposer, 1) # departure rate
            self.interface.connect_models(__FlowDemux_0, flow, flowComposer, 0)
            self.interface.connect_models(submodels["dropSum"], 0, flowComposer, 2)
            self.interface.connect_models(submodels["delaySum"], 0, flowComposer, 3)
            
            if self.log:
                logger_depRate = SignalLogger(self, "rate_flow%d" % flow, log_accumulated=True, log_value=False)._create_model()
                self.interface.connect_models(__reservoir_17, 2+(2*flow), logger_depRate, 0) # Log departure rate                
        
            
        if self.log:
            logger_queueSize = SignalLogger(self, "queueSize")._create_model()
            logger_qSize_avg = SignalLogger(self, "qSize_avg")._create_model()
            self.interface.connect_models(__reservoir_17, 0, logger_queueSize, 0)
            self.interface.connect_models(__WMovingAvg_18, 0, logger_qSize_avg, 0)
            
        
        return self._pd_model
        
class PropagationDelayExt(PropagationDelay):
    def __init__(self, parent, name, n_flows = N_FLOWS_DEFAULT, **kwargs):
        PropagationDelay.__init__(self, parent, name, **kwargs)
        self.n_flows = n_flows        
        
    def _do_create_model(self):
        self._pd_model = self.interface.create_coupled(self.parent.pd_model(), self.name)
        
        # Child models
        __FlowDemux_2 = self.interface.create_fluidflowdynamicdemutiplexer(self._pd_model, "FlowDemux", [self.n_flows])
        self.interface.connect_input_to_model(__FlowDemux_2, 0, 0)
        
        for n in range(0, self.n_flows):
            __flow = PropagationDelay.Flow1_0(self, "Flow%d" % n, delay = self.delay)._create_model() # create an instance of 'Flow1_0' coupled model 
            self.interface.connect_models(__FlowDemux_2, n, __flow, 0)
            self.interface.connect_model_to_output(__flow, 0, 0)
        
        return self._pd_model
    
class ReservoirExt(Reservoir):
    def __init__(self, parent, name, log=True, n_flows=N_FLOWS_DEFAULT, **kwargs):
        Reservoir.__init__(self, parent, name, **kwargs)
        self.n_flows = n_flows
        self.log = log

    def _do_create_model(self):
        self._pd_model = self.interface.create_coupled(self.parent.pd_model(), self.name)
        
        # Models which do not depend on self.n_flows
        __CapacityC_0 = self.interface.create_constant_sci(self._pd_model, "Capacity(C)", [self.capacity])
        __antichateringgain_1 = self.interface.create_qss_gain(self._pd_model, "anti-chatering gain", ["1.001"])
        __AC_1 = self.interface.create_qss_wsum(self._pd_model, "A - C", ["-1", "1", "0", "0", "0", "0", "0", "0", "2.000000e+00"])
        __DiscardRate_2 = Reservoir.DiscardRate_3(self, "DiscardRate", maxSize = self.maxSize)._create_model() # create an instance of 'DiscardRate_2' coupled model 
        ___1C_8 = self.interface.create_qss_nlfunction(self._pd_model, "1/C", ["1/u0", "1.000000e+00"])
        __qtC_9 = self.interface.create_qss_multiplier(self._pd_model, "q(t)/C", ["Purely static", self.dQmin, self.dQrel])
        self.interface.connect_models(__CapacityC_0, 0, __antichateringgain_1, 0)
        self.interface.connect_models(___1C_8, 0, __qtC_9, 0)
        self.interface.connect_models(__AC_1, 0, __DiscardRate_2, 1)
        self.interface.connect_models(__CapacityC_0, 0, ___1C_8, 0)
        self.interface.connect_models(__antichateringgain_1, 0, __AC_1, 0)
        self.interface.connect_model_to_output(__qtC_9, 0, 1)
        
#         inputParams = [self.n_flows] + [1] * self.n_flows # first param is # of inputs, after weight (Kn=1) for all inputs
#         __inputTotal_3 = self.interface.create_qss_dwsum(self._pd_model, "inputTotal", inputParams)  
        __inputTotal_3 = self.interface.create_qss_sum(self._pd_model, "inputTotal", [])
        self.interface.connect_models(__inputTotal_3, 0, __AC_1, 1)
        for n in range(0, self.n_flows): 
            self.interface.connect_input_to_model(__inputTotal_3, n, n)
        
        depRates = []
        for n in range(0, self.n_flows):
            __ratio_A1_4 = Reservoir.ratio_A1_5(self, "ratio_A%d" % n, dQmin = self.dQmin, dQrel = self.dQrel)._create_model() # create an instance of 'ratio_A1_4' coupled model 
            __propDrops_A1_5 = self.interface.create_qss_multiplier(self._pd_model, "propDrops_A%d" %n, ["Purely static", self.dQmin, self.dQrel])
            __delayedDepartureRate1_10 = Reservoir.delayedDepartureRate1_11(self, "delayedDepartureRate%d" % n, capacity = self.capacity, dQmin = self.dQmin, dQrel = self.dQrel)._create_model() # create an instance of 'delayedDepartureRate1_10' coupled model 
            depRates.append(__delayedDepartureRate1_10)
            
            self.interface.connect_models(__ratio_A1_4, 0, __propDrops_A1_5, 1)
            self.interface.connect_models(__DiscardRate_2, 0, __propDrops_A1_5, 0)
            self.interface.connect_models(__inputTotal_3, 0, __ratio_A1_4, 1)
            self.interface.connect_models(__inputTotal_3, 0, __delayedDepartureRate1_10, 1)
            self.interface.connect_models(__qtC_9, 0, __delayedDepartureRate1_10, 2)
            self.interface.connect_input_to_model(__ratio_A1_4, 0, n)
            self.interface.connect_input_to_model(__delayedDepartureRate1_10, 0, n)
            self.interface.connect_model_to_output(__delayedDepartureRate1_10, 0, 2+(2*n))
            self.interface.connect_model_to_output(__propDrops_A1_5, 0, 3+(2*n))
            
            if self.log == "debug":
                logger_drops = SignalLogger(self, "drops%d" % n)._create_model() # create an instance of 'rate_18' coupled model
                self.interface.connect_models(__propDrops_A1_5, 0, logger_drops, 0)
                
        # bounded integrator set with last priority
        __boundInt_12 = Reservoir.boundInt_13(self, "boundInt", maxSize = self.maxSize, method = self.method, dqmin = self.dQmin, dqrel = self.dQrel)._create_model() # create an instance of 'boundInt_12' coupled model
        self.interface.connect_models(__AC_1, 0, __boundInt_12, 0)
        self.interface.connect_model_to_output(__boundInt_12, 0, 0) 
        self.interface.connect_models(__boundInt_12, 0, __qtC_9, 1)
        self.interface.connect_models(__boundInt_12, 0, __DiscardRate_2, 0)
        for __delayedDepartureRate1_10 in depRates:
            self.interface.connect_models(__boundInt_12, 0, __delayedDepartureRate1_10, 3)
         
        if self.log == "debug":
            logger_q_in = SignalLogger(self, "q_in")._create_model() # create an instance of 'w_17' coupled model 
            logger_q_out = SignalLogger(self, "q_out")._create_model() # create an instance of 'rate_18' coupled model
            self.interface.connect_models(__AC_1, 0, logger_q_in, 0)
            self.interface.connect_models(__boundInt_12, 0, logger_q_out, 0)         
         
        return self._pd_model