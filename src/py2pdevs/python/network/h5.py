import h5py

from . import log_vars


class HDF5Reader(object):
    
    TIME_SUFFIX = 't'
    DATA_SUFFIX = 'value'
    
    def __init__(self, model):
        self.model = model
        
        filename = '%s_0.h5' % self.model.get_name()
        self.data = h5py.File(filename, 'r')
        
    def read_variables(self, obj):
        vars = dict()
        for _obj, var, _ in self.model.log_vars:
            if _obj.get_name() == obj.get_name():
                data = self.read_variable(var)
                vars[var.get_alias()] = data
        return vars
    
    def read_object_variable(self, obj, var_name):
        data = self.read_variables(obj)
        for key, val in data.iteritems():
            if var_name in key:
                return val
        
    def read_variable(self, var):
        if isinstance(var, log_vars.LogVariable):
            var_name = var.get_name()
            alias = var.get_alias()
        else:
            # Interpret var as an alias.
            alias = var
            try:
                var_name = self.model.log_aliases[alias]
            except KeyError:
                msg = 'Variable alias %s not registered!' % alias
                raise RuntimeError(msg)
        
        try:
            time_name = '%s.%s' % (var_name, self.TIME_SUFFIX)
            times = self.data[time_name]
            
            value_name = '%s.%s' % (var_name, self.DATA_SUFFIX)
            values = self.data[value_name]
        except Exception:
            msg = 'Variable with alias %s not found in HDF5 output! (actual name: %s)' % (alias, var_name)
            raise RuntimeError(msg)
        
        return times, values
    