from collections import OrderedDict

from py2pdevs.network.model import RootModel

class BaseModel(object):
    
    def __init__(self, parent, name, params = []):
        self.name = name
        self.params = params
        self._pd_model = None        
        self.interface = parent.interface
        self.parent = parent
        self.submodels = dict()
        
        parent.submodels[self.name] = self # add self to list of objects to create from the rootModel
        
                       
    def get_name(self):
        return self.name
    
    def get_full_name(self): #includes parent name recursively
        if not self.parent:
            return self.name
        else:
            return self.parent.get_full_name() + "." + self.name
    
    # override to set default parameters (like logging parameters)
    def get_params(self):
        return []
        
    def pd_model(self):
        return self._pd_model
    
    def _create_model(self):
        if self._pd_model is None:
            self._pd_model = self._do_create_model()            
        return self._pd_model
    
    def _do_create_model(self):
        pass
    
    # called when connected to other model
    def connected(self, src, src_port, dst, dst_port):
        pass
    
    # called when connected to oinport of root
    def connected_to_input(self, myPort, inPort):
        pass
    
    # called when connected output of root
    def connected_to_output(self, myPort, outPort):
        pass    
    
    def add_flow(self, flow):
        pass
    
    ### TODO: this functionality should be part of loggers.py
    def set_sampler_logger(self, var_name, mode, sampling_period):
        params = dict()
        params["%s.logLevel" % (var_name)] = 99999999
        params["%s.sample_period" % (var_name)] = sampling_period
        
        if mode == "packet":    
            params["%s.logger" % (var_name)] = 2 # DiscreteSamplerLogger (see ConfigurationLogger)
        elif mode == "fluid":    
            params["%s.logger" % (var_name)] = 1 # SamplerLogger (see ConfigurationLogger)
        else:
            raise RuntimeError("Only 'fluid' or 'packet' modes are accepted as parameter" )    
        
        return params