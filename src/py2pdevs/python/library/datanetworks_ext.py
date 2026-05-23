import os
import sys

import py2pdevs.core as core
import py2pdevs.network as network
from py2pdevs.common.distributions import Normal, Constant
from .datanetworks_highlevel import  TcpSender_single, TcpReceiver_single, Router, REDRouter, SharedBufferRouter
from .common_ext import TwoWayRouter
               
class TcpSenderExt(TcpSender_single):
    def __init__(self, parent, name, n_sessions=1, firstPort=0, logs_sample_period=0.1, set_samplers=True, **kwargs):
        if not "FlowName" in kwargs:
            kwargs["FlowName"] = name + "_flow"
        TcpSender_single.__init__(self, parent, name, **kwargs)
        self.n_sessions = n_sessions
        self.portsSender = range(firstPort, firstPort + n_sessions)
        self.portsReceiver = range(firstPort, firstPort + n_sessions)        
        self.logs_sample_period = logs_sample_period
        self.set_samplers = set_samplers
   
    def _do_create_model(self):
        self._pd_model = self.interface.create_coupled(self.parent.pd_model(), self.name)
        
        # TcpSender_single_top child models
        egressPort_0 = TcpSender_single.egressPort_0(self, "egressPort_0", bandwidth = self.bandwidth, delay = self.delay)._create_model() # create an instance of 'egressPort_0' coupled model 
        logger_sent = self.interface.create_packetlogger(self._pd_model , "sent", [])
        logger_ack = self.interface.create_packetlogger(self._pd_model , "ack", [])
        port_demux = self.interface.create_tcpportdemux(self._pd_model , 'portDemux', [self.n_sessions, 1, 1]) #params: modulo, divisor, use_dst
        
        sessions = list()
#         print "%s: sessions %d" % (self.name, self.n_sessions)
        for n in range(0, self.n_sessions): # create n instances of 'TcpSession' coupled model
            session = TcpSender_single.TcpSession_1(self, "TcpSession_%d" % n, 
                                                                        portReceiver = self.portsReceiver[n] , portSender = self.portsSender[n],
                                                                        FlowName = self.FlowName, bandwidth = self.bandwidth, delay = self.delay, ipSender = self.ipSender, ipReceiver = self.ipReceiver,  TCP_MSS = self.TCP_MSS, TCP_CWND_max = self.TCP_CWND_max, TCP_CWND_ssthresh = self.TCP_CWND_ssthresh, TCP_RTT_initial = self.TCP_RTT_initial)._create_model() # create an instance of 'TcpSession_1' coupled model 
            self.interface.connect_models(session, 0, egressPort_0, 0)
            self.interface.connect_models(port_demux, n, session, 0)
        
        # other connections    
        self.interface.connect_input_to_model(port_demux, 0, 0)
        self.interface.connect_model_to_output(egressPort_0, 0, 0) 
        self.interface.connect_models(egressPort_0, 0, logger_sent, 0)
        self.interface.connect_input_to_model(logger_ack, 0, 0)

        return self._pd_model
    
    def get_params(self):
        if not self.set_samplers:
            return []
        
        sender_name = self.name    
        params = dict()
        
        for n in range(0, self.n_sessions): # create n instances of 'TcpSession' coupled model
            params['%s.%s.AppQueue.logLevel' % (sender_name, "TcpSession_%d" % n)] = 0 # 
            params['%s.%s.flowGenerator.logLevel' % (sender_name, "TcpSession_%d" % n)] = 0 #
            params['%s.%s.TCP_SND.logLevel' % (sender_name, "TcpSession_%d" % n)] = 100 # only CWND
            params.update(self.set_sampler_logger('%s.%s.TCP_SND.cwnd' % (sender_name, "TcpSession_%d" % n), "fluid", self.logs_sample_period)) 
        
        params["%s.sent.id.logLevel" % sender_name] = 0
        params["%s.sent.overhead_bits.logLevel" % sender_name] = 0
        params["%s.sent.payload_bits.logLevel" % sender_name] = 0
        params["%s.sent.tcp_latency.logLevel" % sender_name] = 0
        params["%s.sent.probe_rate.id.logLevel" % sender_name] = 0
        params["%s.probe_rate.tcp_latency.logLevel" % sender_name] = 0
        params["%s.ack.id.logLevel" % sender_name] = 0
        params["%s.ack.size_bits.logLevel" % sender_name] = 0
        
        params.update(self.set_sampler_logger("%s.sent.size_bits" % sender_name, "packet", self.logs_sample_period))
        params.update(self.set_sampler_logger("%s.probe_rate.size_bits" % sender_name, "packet", self.logs_sample_period))
        params.update(self.set_sampler_logger("%s.ack.tcp_latency" % sender_name, "packet", self.logs_sample_period))

        return params
        

class RouterExt(TwoWayRouter, Router):
    """
    log_variables: queue variables to log. Should match with the ones logged in atomics/network/packetqueue.cpp:init
    logs_sample_period: sampling period to use for loging queue variables. If logs_sample_period=-1, no sampling is used (every event is logged, huge files and slow)
    """
    def __init__(self, parent, name, in_ports=1, out_ports=1, logs_sample_period=0.1, log_variables=["qSize_bits", "elemSize"], **kwargs):
        TwoWayRouter.__init__(self)
        Router.__init__(self, parent, name, **kwargs)
        self.in_ports = in_ports
        self.out_ports = out_ports
        self.bufferSizes = [self.bufferSize] * out_ports if out_ports > 0 else [self.bufferSize]
        self.bandwidths = [self.bandwidth] * out_ports  if out_ports > 0 else [self.bandwidth]
        self.logs_sample_period = logs_sample_period
        self.log_variables = log_variables
        
 
    def _do_create_model(self):
        self._pd_model = self.interface.create_coupled(self.parent.pd_model(), self.name)
        
        # Child models
        __RoutingTable_1 = self.interface.create_flowrouter(self._pd_model, "RoutingTable", [])
        
        # egress ports
        for n in range(0, self.out_ports):
                __egressPort = Router.egressPort0_0(self, "egressPort%d" %n,
                                                         bufferSize = self.bufferSizes[n], bandwidth = self.bandwidths[n], delay = self.delay)._create_model() # create an instance of 'egressPort1_2' coupled model 
                self.interface.connect_models(__RoutingTable_1, n, __egressPort, 0)
                self.interface.connect_model_to_output(__egressPort, 0, n) 
        
        # ingress ports
        for n in range(0, self.in_ports):                                                 
            self.interface.connect_input_to_model(__RoutingTable_1, n, n)
        
        return self._pd_model
    
    def get_params(self):
        router_name = self.name
        params = dict()
    
        for port in range(0, self.out_ports):
            if self.logs_sample_period == -1: # log everything, no sampling
                 params["%s.egressPort%d.queue.logLevel" % (router_name, port)] = 999999999 # log everything
            else:  # sample logging    
                # sample incomming packets
                params.update(self.set_sampler_logger("%s.RoutingTable.incoming_bits" % (router_name), "packet", self.logs_sample_period)) 


                params["%s.egressPort%d.queue.logLevel" % (router_name, port)] = 0 # do not log variables (except the ones explicitly sampled)
                
                for log_var in self.log_variables:
                    params.update(self.set_sampler_logger("%s.egressPort%d.queue.%s" % (router_name, port, log_var), "packet", self.logs_sample_period))
        
        return params   
            
        return params
    
class SharedBufferRouterExt(TwoWayRouter, SharedBufferRouter):
    """
    log_variables: queue variables to log. Should match with the ones logged in atomics/network/packetqueue.cpp:init / atomics/network/packetSharedQueue.cpp:init
    logs_sample_period: sampling period to use for loging queue variables. If logs_sample_period=-1, no sampling is used (every event is logged, huge files and slow)
    """
    def __init__(self, parent, name, in_ports=1, out_ports=1, logs_sample_period=0.1, log_variables=["shared_buffer_size_bits", "port0_buffer_size_bits", "discard_bits", "discard_elems", "elemSize", "port3_buffer_size_bits", "port5_buffer_size_bits", "port9_buffer_size_bits"], **kwargs):
        TwoWayRouter.__init__(self)
        SharedBufferRouter.__init__(self, parent, name, **kwargs)
        self.in_ports = in_ports
        self.out_ports = out_ports     
        self.bufferSizes = [self.per_port_capacity_bits] * out_ports if out_ports > 0 else [self.per_port_capacity_bits] # not used but required by TwoWayRouter   
        self.bandwidths = [self.bandwidth] * out_ports  if out_ports > 0 else [self.bandwidth]
        self.logs_sample_period = logs_sample_period
        self.log_variables = log_variables
        
 
    def _do_create_model(self):
        self._pd_model = self.interface.create_coupled(self.parent.pd_model(), self.name)
        
        # Child models
        __RoutingTable_1 = self.interface.create_flowrouter(self._pd_model, "RoutingTable", [])        
        __sharedBuffer_2 = self.interface.create_packetsharedqueue(self._pd_model, "sharedBuffer", [self.per_port_capacity_bits, self.shared_capacity_bits, self.buffer_cell_size_bits, self.port_max_shared_buffer_use_bits])
        self.submodels["__sharedBuffer_2"] = __sharedBuffer_2
        
        
        # egress ports
        for n in range(0, self.out_ports):
            __egressPort = SharedBufferRouter.egressPort0_0(self, "egressPort%d" %n,
                                                            bandwidth = self.bandwidths[n], delay = self.delay)._create_model() # create an instance of 'egressPort0_0' coupled model 
            self.submodels["__egressPort0_0"] = __egressPort
            
            self.interface.connect_models(__RoutingTable_1, n, __sharedBuffer_2, n*2) # shared buffer even ports for new packets
            self.interface.connect_models(__egressPort, 1, __sharedBuffer_2, (n*2)+1) # shared buffer odd ports for requests
            self.interface.connect_models(__sharedBuffer_2, n, __egressPort, 0) # shared buffer to egressport
            self.interface.connect_model_to_output(__egressPort, 0, n) # egressport to DEVS output port
            
            
        # ingress ports
        for n in range(0, self.in_ports):                                                 
            self.interface.connect_input_to_model(__RoutingTable_1, n, n)
        
        return self._pd_model   
       
    
    def get_params(self):
        router_name = self.name
        params = dict()
    
        for port in range(0, self.out_ports): # FIXME: this loop seems unnecesary
            if self.logs_sample_period == -1: # log everything, no sampling
                params["%s.sharedBuffer.logLevel" % (router_name)] = 999999999 # log everything
            else:  # sample logging    
                # sample incomming packets
                params.update(self.set_sampler_logger("%s.RoutingTable.incoming_bits" % (router_name), "packet", self.logs_sample_period)) 

                # only log variables explicitly sampled
                params["%s.sharedBuffer.logLevel" % (router_name)] = 0         # turn off logging for all variables         
                for log_var in self.log_variables:
                    params.update(self.set_sampler_logger("%s.sharedBuffer.%s" % (router_name, log_var), "packet", self.logs_sample_period))
        
        return params 

class REDRouterExt(TwoWayRouter, REDRouter):
    def __init__(self, parent, name, in_ports=1, out_ports=1, logs_sample_period=0.1, **kwargs):
        TwoWayRouter.__init__(self)
        REDRouter.__init__(self, parent, name, **kwargs)
        self.in_ports = in_ports
        self.out_ports = out_ports
        self.bufferSizes = [self.bufferSize] * out_ports if out_ports > 0 else [self.bufferSize]
        self.bandwidths = [self.bandwidth] * out_ports  if out_ports > 0 else [self.bandwidth]
        self.logs_sample_period = logs_sample_period
        
    def _do_create_model(self):
        self._pd_model = self.interface.create_coupled(self.parent.pd_model(), self.name)
        
        # Child models
        __RoutingTable_1 = self.interface.create_flowrouter(self._pd_model, "RoutingTable", [])
        
        # out ports
#         print "%s total ports: %d" %(self.name, self.out_ports)
        for n in range(0, self.out_ports):
           egressPort = REDRouter.egressPort0_0(self, "egressPort%d" % n,
                                                 bufferSize = self.bufferSizes[n], bandwidth = self.bandwidths[n], delay = self.delay, RED_tmin = self.RED_tmin, RED_pmin = self.RED_pmin, RED_tmax = self.RED_tmax, RED_pmax = self.RED_pmax, RED_alpha = self.RED_alpha)._create_model() # create an instance of 'egressPort0_0' coupled model
           logger = self.interface.create_packetlogger(self._pd_model , "sent%d" %n, [])
           self.interface.connect_models(__RoutingTable_1, n, egressPort, 0)
           self.interface.connect_models(egressPort, 0, logger, 0)
           self.interface.connect_model_to_output(egressPort, 0, n)                                        
           
        
        # in ports
        for n in range(0, self.in_ports):   
             self.interface.connect_input_to_model(__RoutingTable_1, n, n)
        
        return self._pd_model
    
    def get_params(self):
        router_name = self.name
        params = dict()
    
        for port in range(0, self.out_ports):
            # only red avg and drops (sampled)
            params["%s.egressPort%d.RED_AQM.logLevel" % (router_name, port)] = 0
            params.update(self.set_sampler_logger("%s.egressPort%d.RED_AQM.queueAvg" % (router_name, port), "fluid", self.logs_sample_period))
            params.update(self.set_sampler_logger("%s.egressPort%d.RED_AQM.drop_bits" % (router_name, port), "packet", self.logs_sample_period))
            
            params["%s.sent%d.logLevel" % (router_name, port)] = 0
            params.update(self.set_sampler_logger("%s.sent%d.size_bits" % (router_name, port), "packet", self.logs_sample_period))
            
            # only queue size (sampled)
            params["%s.egressPort%d.queue.logLevel" % (router_name, port)] = 0
            params.update(self.set_sampler_logger("%s.egressPort%d.queue.qSize_bits" % (router_name, port), "fluid", self.logs_sample_period))
            
            
        return params


class TcpReceiverExt(TcpReceiver_single):
    def __init__(self, parent, name, n_sessions=100, logs_sample_period=0.1, **kwargs):
        TcpReceiver_single.__init__(self, parent, name, **kwargs)
        self.n_sessions = n_sessions        
        self.logs_sample_period=logs_sample_period

    def _do_create_model(self):
        self._pd_model = self.interface.create_coupled(self.parent.pd_model(), self.name)
        
        # Child models
        __egressPort = TcpReceiver_single.egressPort_0(self, "egressPort", bandwidth = self.bandwidth, delay = self.delay)._create_model() # create an instance of 'egressPort_0' coupled model
        __portDemux = self.interface.create_tcpportdemux(self._pd_model, "portDemux", [self.n_sessions, "1", "1"]) #params: modulo, divisor, use_dst
        
        for n in range(0, self.n_sessions): 
            __TCP_RCV = self.interface.create_packettcprcv(self._pd_model, "TCP_RCV_%d" % n, [self.flowId, self.MSS, self.ipReceiver, n])
            self.interface.connect_models(__portDemux, n, __TCP_RCV, 0)
            self.interface.connect_models(__TCP_RCV, 0, __egressPort, 0)
        
        # Connections
        self.interface.connect_input_to_model(__portDemux, 0, 0)
        self.interface.connect_model_to_output(__egressPort, 0, 0) 

        return self._pd_model
      
    def get_params(self):
        receiver_name = self.name
        params = dict()
        
        params["%s.sent.id.logLevel" % receiver_name] = 0
        params["%s.sent.size_bits.logLevel" % receiver_name] = 99999999
        params["%s.sent.size_bits.logger" % receiver_name] = 2
        params["%s.sent.size_bits.sample_period" % receiver_name] = self.logs_sample_period
        params["%s.sent.size_bits.logger" % receiver_name] = 2
        params["%s.sent.size_bits.sample_period" % receiver_name] = self.logs_sample_period
    
        return params