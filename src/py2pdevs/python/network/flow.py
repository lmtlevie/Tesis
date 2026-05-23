from .objects import NetworkObject, TCPSender, TCPReceiver
from py2pdevs.common.distributions import Normal


class NetworkFlow(NetworkObject):
    # NOTE: these should match with the definitions in atomics/network/RouterTable.h
    ROUTE_NAME_PARAM = 'route'
    ROUTE_INPORTS_PARAM = 'route.inPorts'
    ROUTE_OUTPORTS_PARAM = 'route.outPorts'
    WILD_CARD_PORT = -1
    
    PERIOD_PARAM = 'period'
    PACKET_SIZE_PARAM = 'packetSize'
    TOS_PARAM = 'typeOfService'
    TIMES_PARAM = 'startStopTimes'
    
    def __init__(self, name, src, dst, period=None, packet_size=None, tos=None, times=None):
        NetworkObject.__init__(self, name)
        
        self.src, self.src_port = src
        self.dst, self.dst_port = dst
        
        self.period = period or Normal()
        self.packet_size = packet_size or Normal()
        self.tos = tos or 0
        self.times = times or [0]
        
        self.hosts = list()
        self.inports = list()
        self.outports = list()
        
    def get_src(self):
        return self.src
    
    def get_dst(self):
        return self.dst
        
    def get_src_port(self):
        return self.src_port
    
    def get_dst_port(self):
        return self.dst_port
    
    def connect_route(self):
        for i in xrange(1, len(self.hosts)):
            src, src_port = (self.hosts[i-1], self.outports[i-1]) 
            dst, dst_port = (self.hosts[i], self.inports[i])
            
            if dst_port != self.WILD_CARD_PORT and src_port != self.WILD_CARD_PORT:
                src.parent.connect((src, src_port), (dst, dst_port))
                
    def set_route(self,  *args, **kwargs):
        addConnections= True if not 'addConnections' in kwargs else kwargs['addConnections'] 
        addReturnRoute= True if not 'addReturnRoute' in kwargs else kwargs['addReturnRoute']
        for i in xrange(len(args)):
            if len(args[i]) == 2:
                obj, outport = args[i]
                inport = self.WILD_CARD_PORT # default port means any incoming port
            elif len(args[i]) == 3:   
                obj, inport, outport = args[i]
            else:
                raise RuntimeError('Route elements must have 2 elements (node, outport) or 3 elements (node, inport, outport)')

            self.hosts.append(obj)
            obj.add_flow(self)
            self.inports.append(inport)
            self.outports.append(outport)
        
        if addReturnRoute:
            self.add_return_route()
        
        if addConnections:
            self.connect_route()
    
    def add_return_route(self):
        n = len(self.hosts)
        for i in xrange(n-2, -1, -1):
           self.hosts.append(self.hosts[i])
           self.inports.append(self.outports[i])
           self.outports.append(self.inports[i])
            
    def set_period_distribution(self, period):
        self.period = period
        
    def set_packet_size_distribution(self, packet_size):
        self.packet_size = packet_size
        
    def set_type_of_service(self, tos):
        self.tos = tos
        
    def set_start_stop_times(self, times):
        self.times = times
    
    def serialize(self):
        params = self._serialize_route()
        params.update(self._serialize_distributions())
        params.update(self._serialize_other())
        return params
    
    def _serialize_route(self):
        params = dict()
        
        hosts_param = '%s.%s' % (self.name, self.ROUTE_NAME_PARAM)
        inports_param = '%s.%s' % (self.name, self.ROUTE_INPORTS_PARAM)
        outports_param = '%s.%s' % (self.name, self.ROUTE_OUTPORTS_PARAM)
        
        params[hosts_param] = map(lambda host: host.get_name(), self.hosts)
        params[inports_param] = self.inports
        params[outports_param] = self.outports
        
        return params
    
    def _serialize_distributions(self):
        period_name = '%s.%s' % (self.name, self.PERIOD_PARAM)
        period_params = self.period.serialize_for(period_name)
        
        size_name = '%s.%s' % (self.name, self.PACKET_SIZE_PARAM)
        size_params = self.packet_size.serialize_for(size_name)
        
        period_params.update(size_params)
        
        return period_params
    
    def _serialize_other(self):
        tos_param = '%s.%s' % (self.name, self.TOS_PARAM)
        times_param = '%s.%s' % (self.name, self.TIMES_PARAM)
        return {
            tos_param : self.tos,
            times_param : self.times
        }