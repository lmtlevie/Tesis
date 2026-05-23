import unittest
import os

import network
import py2pdevs.params as params
from base_test import BaseTest
import py2pdevs.library.sources as sources
from py2pdevs.library.qss_continuous import SignalLogger

class BaseAtomicTest(BaseTest):
    def setUp(self, name="base_atomic_test"):
        BaseTest.setUp(self)
        
        self.name = name
        self.model = network.new_model(name)
        self.input =  sources.customSteps(self.model, "input_events")
        self.output =  SignalLogger(self.model, "output")
        self.log_in =  SignalLogger(self.model, "input") # for testing
        self.model.connect((self.input, 0), (self.log_in,0) )
        
    def tearDown(self):
        BaseTest.tearDown(self)
        if self.remove_logs:
            os.system("rm -rf %s_0.h5" % self.name )
    
    def set_input_events(self, times, values, values1=None, values2=None, values3=None, values4=None, input=None):
        if not input:
            input = self.input
            
        if not values1:
            values1 = [0] * len(times)
        if not values2:
            values2 = [0] * len(times)
        if not values3:
            values3 = [0] * len(times)
        if not values4:
            values4 = [0] * len(times)
        
        input.timesChanges = times
        input.ValueChanges = values
        input.ValueChanges1stderivative = values1
        input.ValueChanges2ndderivative = values2
        input.ValueChanges3rdderivative = values3
        input.ValueChanges4thderivative = values4
    