from . import log_vars

from collections import OrderedDict


class NetworkObject(object):
    
    def __init__(self, name):
        self.name = name
        self._model = None
        
    def get_name(self):
        return self.name
    
    def model(self):
        return self._model
    
    def _create_model(self, interface, parent):
        if self._model is None:
            self._model = self._do_create_model(interface, parent)
        return self._model
    
    def _do_create_model(self, interface, parent):
        pass
        
        
class TCPSender(NetworkObject):
    
    SESSION_NAME_TEMPLATE = 'TcpSession%d'
    
    def __init__(self, name, ip,
         bandwidth, propagation, MSS,
         CWND_max, ssthresh, initial_RTT):
        NetworkObject.__init__(self, name)
        self.bandwidth = bandwidth
        self.propagation = propagation
        self.MSS = MSS
        self.CWND_max = CWND_max
        self.ssthresh = ssthresh
        self.initial_RTT = initial_RTT
        self.ip = ip
        self.flows = OrderedDict()
        self.src_port_idx = 0
        self.session_names = list()
        
    ### Log variables
    def CWND_var_for(self, flow, alias=None):
        return log_vars.CWND(self, flow, alias)    
        
    def get_ip(self):
        return self.ip        
        
    def add_flow(self, flow):
        port = flow.get_src_port()
        if port in self.flows:
            src_port_idx = self.flows[port][1] 
            self.flows[port] = (flow, src_port_idx)
        else:
            self.flows[port] = (flow, self.src_port_idx)
            session_name = self.SESSION_NAME_TEMPLATE % self.src_port_idx
            self.session_names.append(session_name)
            self.src_port_idx += 1
        
    def get_port_index(self, port):
        if port not in self.flows:
            raise RuntimeError('Port %d not found!' % port)
        return self.flows[port][1]
    
    def get_session_name_for(self, flow):
        for (_flow, src_port_idx) in self.flows.itervalues():
            if _flow.get_src_port() == flow.get_src_port():
                return self.session_names[src_port_idx]
        
    def set_receiver(self, receiver):
        self.receiver = receiver
        
    def _do_create_model(self, interface, parent):
        sender = interface.create_coupled(parent, self.name)
        
        egress_port = EgressPort('egressPort0',
                                 -1,
                                 self.bandwidth,
                                 self.propagation)._create_model(interface, sender)
                                 
        port_demux = interface.create_tcpportdemux(sender, 'portDemux', [len(self.flows), 1])

        for src_port in self.flows:
            flow, src_port_idx = self.flows[src_port]  
            session_name = self.session_names[src_port_idx]
            flow_name = flow.get_name()
            dst = flow.get_dst()
            dst_port = flow.get_dst_port()
            dst_port_idx = dst.get_port_index(dst_port)
            
            session = TCPSession(session_name,
                                 self.ip,
                                 dst.get_ip(),
                                 # NOTE: src/dst ports are masked for proper DEVS
                                 # connections.
                                 src_port_idx,
                                 dst_port_idx,
                                 self.CWND_max,
                                 self.ssthresh,
                                 -1,
                                 self.bandwidth,
                                 self.propagation,
                                 self.MSS,
                                 self.initial_RTT,
                                 flow_name)._create_model(interface, sender)

            interface.connect_models(port_demux, src_port_idx, session, 0)
            interface.connect_models(session, 0, egress_port, 0)
            
        interface.connect_input_to_model(port_demux, 0, 0)
        interface.connect_model_to_output(egress_port, 0, 0)
        
        return sender
        
        
class TCPReceiver(NetworkObject):
    
    def __init__(self, name, ip, bandwidth, propagation, MSS):
        NetworkObject.__init__(self, name)
        self.bandwidth = bandwidth
        self.propagation = propagation
        self.MSS = MSS
        self.ip = ip
        self.senders = OrderedDict()
        self.dst_port_idx = 0
        
    def get_ip(self):
        return self.ip
        
    def add_flow(self, flow):
        src = flow.get_src()
        src_port = flow.get_src_port()
        dst_port = flow.get_dst_port()
        if dst_port in self.senders:
            dst_port_idx = self.senders[dst_port][2]
            self.senders[dst_port] = (src, src_port, dst_port_idx)
        else:
            self.senders[dst_port] = (src, src_port, self.dst_port_idx)
            self.dst_port_idx += 1
            
    def get_port_index(self, port):
        if port not in self.senders:
            raise RuntimeError('Port %d not found!' % port)
        return self.senders[port][2]
        
    def set_sender(self, sender):
        self.sender = sender
        
    def _do_create_model(self, interface, parent):
        receiver = interface.create_coupled(parent, self.name)
        
        egress_port_0 = EgressPort('egressPort0',
                                   -1,
                                   self.bandwidth,
                                   self.propagation)._create_model(interface, receiver)
        
        for idx, dst_port in enumerate(self.senders):
            rcv_name = 'TCP_RCV_%d' % idx
            src, src_port, dst_port_idx = self.senders[dst_port]
            src_port_idx = src.get_port_index(src_port)
            sender_ip = src.get_ip()
            
            params = [self.MSS, sender_ip, self.ip, src_port_idx, dst_port_idx]
            rcv = interface.create_packettcprcv(receiver, rcv_name, params)
            
            interface.connect_input_to_model(rcv, 0, 0)
            interface.connect_models(rcv, 0, egress_port_0, 0)
            
        interface.connect_model_to_output(egress_port_0, 0, 0)
        
        return receiver
    
        
class Router(NetworkObject):
    
    def __init__(self, name, buffer_size, bandwidth, propagation):
        NetworkObject.__init__(self, name)
        self.buffer_size = buffer_size
        self.bandwidth = bandwidth
        self.propagation = propagation
        
    ### Log variables
    def queue_size_var_for(self, idx, alias=None):
        return log_vars.QueueSize(self, idx, alias)
    
    def tail_drops_var_for(self, idx, alias=None):
        return log_vars.QueueDrops(self, idx, alias)

    def _create_egress_port(self, name):
        return EgressPort(name,
                          self.buffer_size,
                          self.bandwidth,
                          self.propagation)

    def _do_create_model(self, interface, parent):
        router = interface.create_coupled(parent, self.name)
    
        egress_port_0 = self._create_egress_port('egressPort0')._create_model(interface, router)
        routing_table = interface.create_flowrouter(router, 'RoutingTable', [])
        egress_port_1 = self._create_egress_port('egressPort1')._create_model(interface, router)
        egress_port_2 = self._create_egress_port('egressPort2')._create_model(interface, router)
        egress_port_3 = self._create_egress_port('egressPort3')._create_model(interface, router)
    
        interface.connect_models(routing_table, 0, egress_port_0, 0)
        interface.connect_models(routing_table, 1, egress_port_1, 0)
        interface.connect_models(routing_table, 2, egress_port_2, 0)
        interface.connect_models(routing_table, 3, egress_port_3, 0)
        interface.connect_input_to_model(routing_table, 0, 0)
        interface.connect_input_to_model(routing_table, 1, 1)
        interface.connect_input_to_model(routing_table, 2, 2)
        interface.connect_input_to_model(routing_table, 3, 3)
        interface.connect_model_to_output(egress_port_0, 0, 0)
        interface.connect_model_to_output(egress_port_3, 0, 3)
        interface.connect_model_to_output(egress_port_2, 0, 2)
        interface.connect_model_to_output(egress_port_1, 0, 1)
    
        return router
    
    
class REDRouter(Router):
    
    def __init__(self,
                 name, buffer_size, bandwidth, propagation,
                 tmin, tmax, pmin, pmax, alpha):
        Router.__init__(self, name, buffer_size, bandwidth, propagation)
        self.tmin = tmin
        self.tmax = tmax
        self.pmin = pmin
        self.pmax = pmax
        self.alpha = alpha
        
    ### Log variables
    def RED_drops_var_for(self, idx, alias=None):
        return log_vars.REDDrops(self, idx, alias)
    
    def _create_egress_port(self, name):
        return REDEgressPort(name,
                             self.buffer_size,
                             self.bandwidth,
                             self.propagation,
                             self.tmin, self.tmax,
                             self.pmin, self.pmax,
                             self.alpha)


class EgressPort(NetworkObject):
    
    def __init__(self, name, buffer_size, bandwidth, propagation):
        NetworkObject.__init__(self, name)
        self.buffer_size = buffer_size
        self.bandwidth = bandwidth
        self.propagation = propagation
        
    def _create_atomic_models(self, interface, parent):
        self.queue = interface.create_packetqueue(parent, 'queue', [self.buffer_size, -1])
        self.bwdelay = interface.create_packetlinkbandwidth(parent, 'BWDelay', [self.bandwidth])
        self.propagation_delay = interface.create_packetdelay(parent, 'PropagationDelay', [self.propagation])
        
    def _connect_models(self, interface):
        interface.connect_models(self.queue, 0, self.bwdelay, 0)
        interface.connect_models(self.bwdelay, 0, self.queue, 1)
        interface.connect_models(self.bwdelay, 0, self.propagation_delay, 0)
        interface.connect_input_to_model(self.queue, 0, 0)
        interface.connect_model_to_output(self.propagation_delay, 0, 0)
        
    def _do_create_model(self, interface, parent):
        egress_port = interface.create_coupled(parent, self.name)
        
        self._create_atomic_models(interface, egress_port)
        self._connect_models(interface)
    
        return egress_port
    
    
class REDEgressPort(EgressPort):
    
    def __init__(self, name, buffer_size, bandwidth, propagation,
                 tmin, tmax, pmin, pmax, alpha):
        EgressPort.__init__(self, name, buffer_size, bandwidth, propagation)
        self.tmin = tmin
        self.tmax = tmax
        self.pmin = pmin
        self.pmax = pmax
        self.alpha = alpha
        
    def _create_atomic_models(self, interface, parent):
        EgressPort._create_atomic_models(self, interface, parent)
        red_params = [self.tmin, self.pmin, self.tmax, self.pmax, self.alpha]
        self.red = interface.create_packetred_aqm(parent, 'RED_AQM', red_params)
        
    def _connect_models(self, interface):
        interface.connect_models(self.red, 0, self.queue, 0)
        interface.connect_models(self.queue, 0, self.bwdelay, 0)
        interface.connect_models(self.queue, 1, self.red, 1)
        interface.connect_models(self.bwdelay, 0, self.queue, 1)
        interface.connect_models(self.bwdelay, 0, self.propagation_delay, 0)
        interface.connect_input_to_model(self.red, 0, 0)
        interface.connect_model_to_output(self.propagation_delay, 0, 0)
        
    
class TCPSession(NetworkObject):
    
    def __init__(self,
                 name,
                 sender_ip,
                 receiver_ip,
                 sender_port,
                 receiver_port,
                 CWND_max,
                 ssthresh,
                 buffer_size,
                 bandwidth,
                 propagation,
                 MSS,
                 initial_RTT,
                 flow_name):
        NetworkObject.__init__(self, name)
        
        self.sender_ip = sender_ip
        self.receiver_ip = receiver_ip
        self.sender_port = sender_port
        self.receiver_port = receiver_port
        self.CWND_max = CWND_max
        self.ssthresh = ssthresh
        self.buffer_size = buffer_size
        self.bandwidth = bandwidth
        self.propagation = propagation
        self.MSS = MSS
        self.initial_RTT = initial_RTT
        self.flow_name = flow_name
        
    def _do_create_model(self, interface, parent):
        session = interface.create_coupled(parent, self.name)
        
        pf_generator = interface.create_packetflowgenerator(session, 'flowGenerator', [self.flow_name])
        custom_steps = interface.create_customsteps(session, 'starter', ['{0}','{0}','','',''])
        app_queue = interface.create_packetqueue(session, 'AppQueue', [-1,-1])
        
        RTT_alpha = 0.125
        dup_ACK_limit = 3
        forced_RTO = -1
        interpacket_send_time = 0
        inter_req_time = 1e-7
        params = [self.MSS,
                  self.sender_ip,
                  self.receiver_ip,
                  self.sender_port,
                  self.receiver_port,
                  self.CWND_max,
                  self.ssthresh,
                  RTT_alpha,
                  self.initial_RTT,
                  dup_ACK_limit,
                  forced_RTO,
                  interpacket_send_time,
                  inter_req_time]
        tcpsnd = interface.create_packettcpsnd(session, 'TCP_SND', params)
    
        interface.connect_models(tcpsnd, 1, app_queue, 1)
        interface.connect_models(app_queue, 0, tcpsnd, 0)
        interface.connect_models(custom_steps, 0, pf_generator, 0)
        interface.connect_models(tcpsnd, 1, pf_generator, 0)
        interface.connect_models(pf_generator, 0, app_queue, 0)
        interface.connect_input_to_model(tcpsnd, 1, 0)
        interface.connect_model_to_output(tcpsnd, 0, 0)
    
        return session