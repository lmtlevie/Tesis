import os
import sys

import collections
from .base_model import BaseModel 
from .qss_continuous import SignalLogger

class SignalLoggerExt(SignalLogger):
    def __init__(self, parent, name, log_value=True, log_samples=True, log_accumulated=False, **kwargs):
        SignalLogger.__init__(self, parent, name,  **kwargs)
        self.log_value = log_value
        self.log_samples = log_samples
        self.log_accumulated = log_accumulated

    def _do_create_model(self):
        self._pd_model = self.interface.create_coupled(self.parent.pd_model(), self.name)
        
        # Child models
        if self.log_value:
            __value_2 = self.interface.create_tologger(self._pd_model, "value", [])
            self.submodels["__value_2"] = __value_2
            self.interface.connect_input_to_model(__value_2, 0, 0) 
        
        if self.log_samples:
            __sampledValue_0 = self.interface.create_tologger(self._pd_model, "sampledValue", [])
            self.submodels["__sampledValue_0"] = __sampledValue_0
            __sample2plot_1 = self.interface.create_samplehold(self._pd_model, "sample2plot", [self.samplingPeriod, "0"])
            self.submodels["__sample2plot_1"] = __sample2plot_1
            self.interface.connect_models(__sample2plot_1, 0, __sampledValue_0, 0)
            self.interface.connect_input_to_model(__sample2plot_1, 0, 0)
            
        if self.log_accumulated:
            __QSSIntegrator0_3 = self.interface.create_qss_integrator(self._pd_model, "QSS Integrator0", ["QSS3", "1e-6", "1e-3", "0"])
            self.submodels["__QSSIntegrator0_3"] = __QSSIntegrator0_3
            __accumValue_4 = self.interface.create_tologger(self._pd_model, "accumValue", [])
            self.submodels["__accumValue_4"] = __accumValue_4
            self.interface.connect_models(__QSSIntegrator0_3, 0, __accumValue_4, 0)
            self.interface.connect_input_to_model(__QSSIntegrator0_3, 0, 0)
           
        return self._pd_model
