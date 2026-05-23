class LogVariable(object):
    
    def __init__(self, obj, alias):
        self.obj = obj
        self.alias = alias
        
    def get_alias(self):
        if self.alias is None:
            self.alias = self.get_name()
        return self.alias
    
    def owner(self):
        return self.obj
        
    def get_name(self):
        first = self.obj.get_name()
        mid = self._get_mid_name()
        return '%s.%s.%s' % (first, mid, self.NAME)
    
    
class CWND(LogVariable):
    
    NAME = 'cwnd'
    
    def __init__(self, obj, flow, alias=None):
        LogVariable.__init__(self, obj, alias)
        self.flow = flow
    
    def _get_mid_name(self):
        session_name = self.obj.get_session_name_for(self.flow)
        return '%s.TCP_SND' % session_name
    
    
class EgressPortVariable(LogVariable):
   
    EGRESS_PORT_TEMPLATE = 'egressPort%d'
    
    def __init__(self, obj, idx, alias=None):
        LogVariable.__init__(self, obj, alias)
        self.idx = idx
    
    def _get_mid_name(self):
        return self.EGRESS_PORT_TEMPLATE % self.idx
    
    
class QueueVariable(EgressPortVariable):
    
    QUEUE = 'queue'
    
    def _get_mid_name(self):
        egress_port_name = EgressPortVariable._get_mid_name(self)
        return '%s.%s' % (egress_port_name, self.QUEUE)
    
    
class QueueSize(QueueVariable):
    
    NAME = 'qSize_bits'

    
class QueueDrops(QueueVariable):
    
    NAME = 'discard_bits'
    
    
class REDDrops(EgressPortVariable):
    
    NAME = 'drop_bits'
    
    RED_AQM = 'RED_AQM'
    
    def _get_mid_name(self):
        egress_port_name = EgressPortVariable._get_mid_name(self)
        return '%s.%s' % (egress_port_name, self.RED_AQM)