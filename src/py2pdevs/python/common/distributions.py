
class Distribution(object):
    def __init__(self):
        pass
    
    # instanciate distribution according to parameters
    @staticmethod
    def from_config(param_name, params):
        global code2distr
        config_code = float(params[param_name])
        type = code2distr[config_code]
        if type.CODE != config_code:
            print("Error: mis-configued code2distr in python")
            exit

        return type.from_config(param_name, params)

    def serialize_for(self, argument):
        params = {argument : self.CODE}
        for param, value in self.params().iteritems():
            params['%s_%s' % (argument, param)] = value
        return params
        
    def params(self):
        raise NotImplementedError

    def get_mean(self):
        raise NotImplementedError

    def get_std(self):
        raise NotImplementedError


class Constant(Distribution):
    
    CODE = 0
    
    VALUE = 'value'

    @staticmethod
    def from_config(param_name, params):
        return Constant(float(params[param_name + "_" + Constant.VALUE]))
    
    def __init__(self, value):
        self.value = value
        
    def params(self):
        return {
            self.VALUE : self.value
        }
    
    def get_mean(self):
        return self.value

    def get_std(self):
        return 0

        
class Exponential(Distribution):
    
    CODE = 1
    
    MU = 'mu'
    
    @staticmethod
    def from_config(param_name, params):
        return Exponential(float(params[param_name + "_" + Exponential.MU]))
    
    def __init__(self, mu):
        self.mu = mu
        
    def params(self):
        return {
            self.MU : self.mu
        }
    
    def get_mean(self):
        return self.mu
    
    def get_std(self):
        return self.mu
        
class Pareto(Distribution):
    
    CODE = 2
    
    SHAPE = 'shape'
    SCALE = 'scale'

    @staticmethod
    def from_config(param_name, params):
        return Pareto(float(params[param_name + "_" + Pareto.SHAPE]), float(params[param_name + "_" + Pareto.SCALE]))
    
    def __init__(self, shape, scale):
        self.shape = shape
        self.scale = scale
        
    def params(self):
        return {
            self.SHAPE : self.shape,
            self.SCALE : self.scale
        }
        
class Split(Distribution):
    
    CODE = 3
    
    VAL1 = 'val1'
    VAL2 = 'val2'
    MEAN = 'mean'

    @staticmethod
    def from_config(param_name, params):
        raise NotImplementedError
    
    def __init__(self, val1, val2, mean):
        self.val1 = val1
        self.val2 = val2
        self.mean = mean
        
    def params(self):
        return {
            self.VAL1 : self.val1,
            self.VAL2 : self.val2,
            self.MEAN : self.mean
        }
        
class Normal(Distribution):
    
    CODE = 4
    
    MU = 'mu'
    SIGMA = 'sigma'

    @staticmethod
    def from_config(param_name, params):
        return Normal(float(params[param_name + "_" + Normal.MU]), float(params[param_name + "_" + Normal.SIGMA] ))
    
    def __init__(self, mu=None, sigma=None):
        self.mu = mu or 0
        self.sigma = sigma or 1
        
    def params(self):
        return {
            self.MU : self.mu,
            self.SIGMA : self.sigma
        }

    def get_mean(self):
        return self.mu
    
    def get_std(self):
        return self.sigma
        
class Poisson(Distribution):    
    CODE = 6    
    RATE = 'rate'
    
    @staticmethod
    def from_config(param_name, params):
        raise NotImplementedError

    def __init__(self, rate):
        self.rate = rate        
        
    def params(self):
        return {
            self.RATE : self.rate,            
        }  
        
class Gamma(Distribution):    
    CODE = 8    
    SHAPE = 'shape'
    SCALE = 'scale'
    
    @staticmethod
    def from_config(param_name, params):
        raise NotImplementedError

    def __init__(self, shape, scale):
        self.shape = shape
        self.scale = scale                
        
    def params(self):
        return {
            self.SHAPE : self.shape,            
            self.SCALE : self.scale,
        }                

code2distr = {
        0: Constant,
        1: Exponential,
        2: Pareto,
        3: Split,
        4: Normal,
        # 5 ?
        6: Poisson,
        # 7 ?
        8: Gamma,
    }