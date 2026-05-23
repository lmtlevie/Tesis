import os
import unittest
import numpy as np
from mock import MagicMock, Mock, patch
from base_atomic_test import BaseAtomicTest


import network
import py2pdevs.params as params
import py2pdevs.library.sources as sources
from py2pdevs.network_plot import Plotter # to read h5, needs refactoring
from py2pdevs.library.qss_continuous import SignalLogger
from py2pdevs.library.base_model import BaseModel

class QSSSum(BaseModel):
    def __init__(self, parent, name):
        BaseModel.__init__(self, parent, name, [])
        
    def _do_create_model(self):
        self._pd_model = self.interface.create_qss_sum(self.parent.pd_model(), self.name, [])

        return self._pd_model

class TestQSSSum(BaseAtomicTest):
    def setUp(self):
        BaseAtomicTest.setUp(self, 'test_qss_sum')
        
        self.sum = QSSSum(self.model, "sum")
        
        self.model.connect((self.input, 0), (self.sum, 0))
        self.model.connect((self.sum, 0), (self.output,0)    )        
        
    def tearDown(self):
        BaseAtomicTest.tearDown(self)
    
    def test_2signals(self):
        params = {"logging_backend":'hdf5'}
        self.model.set_global_parameters(**params)
        
        self.input2 =  sources.customSteps(self.model, "input_events2")
        self.model.connect((self.input2, 0), (self.sum, 1))
                 
        # configure input
        self.set_input_events(input = self.input,
                              times= [0, 1 ],
                              values=[3, 8])
        self.set_input_events(input = self.input2,
                              times= [0.5, 1.5],
                              values=[1  , 7  ])
        
        self.model.run()
        
        # read results
        with Plotter("test_qss_sum_0.h5") as reader:
            times, values = reader.getTimeSerie_continuous("output", samples=False)
            np.testing.assert_almost_equal([0, 0.5, 1, 1.5], times.tolist())
            np.testing.assert_almost_equal([3, 4, 9, 15], values.tolist())
    
    def test_Nsignals(self):
        params = {"logging_backend":'hdf5',
                  "tf":"1000"}
        self.model.set_global_parameters(**params)
        
        N=100
        for i in xrange(1,N):
            input = sources.customSteps(self.model, "input_events%d" % i)
            self.model.connect((input, 0), (self.sum, i))
            
            self.set_input_events(input = input,
                              times= [i,i+N],
                              values=[1,0])
        
                                 
        # configure input
        self.set_input_events(input = self.input,
                              times= [0, N],
                              values=[3, 0])
        
        self.model.run()
        
        # read results
        with Plotter("test_qss_sum_0.h5") as reader:
            times, values = reader.getTimeSerie_continuous("output", samples=False)
            np.testing.assert_almost_equal(range(0,N*2), times.tolist())
            rev = range(0, N)
            rev.reverse()            
            np.testing.assert_almost_equal(range(3, N+3)+rev, values.tolist())

    def test_should_advance_time(self):
        params = {"logging_backend":'hdf5'}
        self.model.set_global_parameters(**params)
        
        self.input2 =  sources.customSteps(self.model, "input_events2")
        self.model.connect((self.input2, 0), (self.sum, 1))
                 
        # configure input
        self.set_input_events(input = self.input,
                              times =  [0, 3],
                              values = [1, 2],
                              values1= [1, 3]) # slope of 1 
        self.set_input_events(input = self.input2,
                              times=  [2], # after 2s, signal1 should equal to 3=1+2*1
                              values= [5],
                              values1=[2])
        
        self.model.run()
        
        # read results
        with Plotter("test_qss_sum_0.h5") as reader:
            times, values = reader.getTimeSerie_continuous("output", samples=False)
            np.testing.assert_almost_equal([0, 2, 3], times.tolist())
            np.testing.assert_almost_equal([1, 5+3, 7+2], values.tolist())
    