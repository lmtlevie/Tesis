from . import flow
from . import h5
from . import loggers
from . import objects
from py2pdevs.common import distributions

import  py2pdevs.params

from collections import OrderedDict


class RootModel(object):
    
    FLOW_NAMES_PARAM = 'PacketFlowNames'
    FLOW_IDS_PARAM = 'FlowIds' 
    
    LOGGER_PARAM = 'logger'
    LOG_LEVEL_PARAM = 'logLevel'
    DEBUG_LEVEL_PARAM = 'debugLevel'
    DEFAULT_LOG_VALUE = 100
    
    def __init__(self, interface, name):
        self.interface = interface
        self.name = name
        
        self.global_params = dict()
        
        self.submodels = OrderedDict()
        self.flows = set()
        
        self.internal_connections = set()
        self.input_connections = set()
        self.output_connections = set()
        
        self.log_vars = list()
        self.log_aliases = dict()
        
        self.h5_reader = None
        
    def get_name(self):
        return self.name
    
    def pd_model(self):
        return self.interface.top()
    
    def get_h5_reader(self):
        if self.h5_reader is None:
            self.h5_reader = h5.HDF5Reader(self)
        return self.h5_reader
        
    def run(self):
        params = self.serialize()
#        params.update(self.global_params) # uncomment if config params should override code params
        params.setdefault(self.LOG_LEVEL_PARAM, 0)
        params.setdefault(self.DEBUG_LEVEL_PARAM, 0)
        py2pdevs.params.set_options(params)
        
        self._make_connections()
        
        self.interface.run()
        
    def set_global_parameters(self, **params):
        self.global_params.update(params)
        
    def get_global_parameters(self):
        return self.global_params
    
    def new_tcp_sender(self, name, ip=None,
           bandwidth=None, propagation=None, MSS=None,
           CWND_max=None, ssthresh=None, initial_RTT=None):
        ip = ip or name
        bandwidth = bandwidth or self._check_globals('bandwidth')
        propagation = propagation or self._check_globals('propagation')
        MSS = MSS or self._check_globals('MSS')
        CWND_max = CWND_max or self._check_globals('CWND_max')
        ssthresh = ssthresh or self._check_globals('ssthresh')
        initial_RTT = initial_RTT or self._check_globals('initial_RTT')
        sender = objects.TCPSender(name, ip, bandwidth, propagation,
                                   MSS, CWND_max, ssthresh, initial_RTT)
        self.submodels[name] = sender
        return sender
    
    def new_tcp_receiver(self, name, ip=None,
           bandwidth=None, propagation=None, MSS=None):
        ip = ip or name
        bandwidth = bandwidth or self._check_globals('bandwidth')
        propagation = propagation or self._check_globals('propagation')
        MSS = MSS or self._check_globals('MSS')
        receiver = objects.TCPReceiver(name, ip, bandwidth, propagation, MSS)
        self.submodels[name] = receiver
        return receiver
    
    def new_router(self, name,
           buffer_size=None, bandwidth=None, propagation=None):
        buffer_size = buffer_size or self._check_globals('buffer_size')
        bandwidth = bandwidth or self._check_globals('bandwidth')
        propagation = propagation or self._check_globals('propagation')
        router = objects.Router(name, buffer_size, bandwidth, propagation)
        self.submodels[name] = router
        return router
    
    def new_RED_router(self, name,
           buffer_size=None, bandwidth=None, propagation=None,
           queue_thresh=None, discard_prob=None, alpha=None):
        buffer_size = buffer_size or self._check_globals('buffer_size')
        bandwidth = bandwidth or self._check_globals('bandwidth')
        propagation = propagation or self._check_globals('propagation')
        if queue_thresh is None:
            tmin = self._check_globals('tmin')
            tmax = self._check_globals('tmax')
        else:
            tmin, tmax = queue_thresh
        if discard_prob is None:
            pmin = self._check_globals('pmin')
            pmax = self._check_globals('pmax')
        else:
            pmin, pmax = discard_prob
        alpha = propagation or self._check_globals('alpha')
        params = [name, buffer_size, bandwidth, propagation, tmin, tmax, pmin, pmax, alpha]
        router = objects.REDRouter(*params)
        self.submodels[name] = router
        return router
    
    def new_flow(self, flow_id, src, dst, period=None, packet_size=None, tos=None, times=None):
        if flow_id in self.flows:
            raise RuntimeError('Trying to add flow "%s" which was already added previously!' % flow_id) 
        
        if isinstance(src, (list,tuple)):
            src, src_port = src
        else:
            src_port = 0
        if isinstance(dst, (list,tuple)):
            dst, dst_port = dst
        else:
            dst_port = 0
            
        period = period or distributions.Constant(99999999)   
        
        _flow = flow.NetworkFlow(
            name= flow_id,
            period=period,
            packet_size=packet_size,
            tos=tos,
            times=times,
            src=(src,src_port),
            dst=(dst,dst_port))
        
        src.add_flow(_flow)
        dst.add_flow(_flow)
        
        self.flows.add(_flow)
        
        return _flow
    
    def serialize(self):
        params = dict(self.global_params)
        if self.flows and params.has_key(self.FLOW_IDS_PARAM):
            # TODO: to go around this boths definitions should be joind. Now config parameters are strings (stirngs with '{' must be converted into a list)
            raise RuntimeError("Flows are defined both in config and in code (parameter %s). Only one definition can be used, either in config or in code, but not both" % self.FLOW_IDS_PARAM)
        elif self.flows:
            # set up flows
            flow_names = list()
            for flow in self.flows:
                flow_names.append(flow.get_name())
                flow_params = flow.serialize()
                params.update(flow_params)
            params[self.FLOW_NAMES_PARAM] = flow_names
            params[self.FLOW_IDS_PARAM] = flow_names # for now both params should be specified to differenciate fluid and packets flows
            
        log_params = self._serialize_log_vars()
        params.update(log_params)
        model_params = self._serialize_model_params()
        params.update(model_params)
        
        return params
    
    def connect(self, src, dst):
        # interpret parameters
        if isinstance(src, (tuple,list)): # interpret src
            connection = self.internal_connections
            param = src 
        elif isinstance(src, int):
            connection = self.input_connections
            param = (src, )            
        else:
            connection = self.internal_connections
            param = (src, 0) # default port
            
        if isinstance(dst, (tuple,list)): # interpret dst
            param += dst
        elif isinstance(dst, int):
            if connection is self.input_connections:
                raise RuntimeError('Invalid connection between %r and %r! Either src or dst should be an object or tuple.' % (src, dst))
            connection = self.output_connections
            param += (dst, )            
        else:
            param += (dst, 0) # default port
           
        # add connection and notify   
        if not param in connection:
            connection.add(param)
            
            # send notification to src and dst
            if connection is self.output_connections:
                src, src_port, out_port = param
                src.connected_to_output(src_port, out_port)
            elif connection is self.input_connections:
                in_port, dst, dst_port = param
                dst.connected_to_input(dst_port, in_port)
            else:
               src, src_port, dst, dst_port = param
               src.connected(*param)
               dst.connected(*param)    
            
    def connect_input(self, input_port, dst):
        self.connect(src=input_port, dst=dst)
        
    def connect_output(self, src, output_port):
        self.connect(src=src, dst=output_port)
        
    def log(self, vars, logger=None):
        logger = logger or loggers.DefaultLogger()
        
        if not isinstance(vars, (tuple,list)): # vars is log_vars class
            self._log_var(vars.owner(), vars, logger)
        else: # vars is list of log_vars
            for var in vars:
                self._log_var(var.owner(), var, logger)
                
    def _log_var(self, obj, var, logger):
        self.log_vars.append((obj, var, logger))
        alias = var.get_alias()
        name = var.get_name()
        if alias in self.log_aliases:
            msg = 'Variable %s has alias %s, which was already registered!' % (name, alias)
            raise RuntimeError(msg)
        self.log_aliases[alias] = name
        
    def _create_models(self):
        map(lambda obj: obj._create_model(), self.submodels.itervalues())
    
    def _make_connections(self):
        self._create_models()
        for connection in self.internal_connections:
            src_obj, src_port, dst_obj, dst_port = connection
            self.interface.connect_models(src_obj.pd_model(), src_port, dst_obj.pd_model(), dst_port)
        for connection in self.input_connections:
            src_port, dst_obj, dst_port = connection
            self.interface.connect_input_to_model(dst_obj.pd_model(), dst_port, src_port)
        for connection in self.output_connections:
            src_obj, src_port, dst_port = connection
            self.interface.connect_model_to_output(src_obj.pd_model(), src_port, dst_port)
    
    def _check_globals(self, param):
        if param in self.global_params:
            return self.global_params[param]
        raise RuntimeError('%s parameter not found!' % param)
    
    def _serialize_log_vars(self):
        params = dict()
        for (obj, var, logger) in self.log_vars:
            logger_param = '%s.%s' % (var.get_name(), self.LOGGER_PARAM)
            params[logger_param] = logger.CODE
            
            log_level_param = '%s.%s' % (var.get_name(), self.LOG_LEVEL_PARAM)
            params[log_level_param] = self.DEFAULT_LOG_VALUE
        return params
    
    def _serialize_model_params(self):
        params = dict()
        for submodel in  self.submodels.itervalues():
            get_params_func = getattr(submodel, "get_params", None)
            if callable(get_params_func):
                params.update(get_params_func())
        
        return params
    
        
        