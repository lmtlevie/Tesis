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
from py2pdevs.library.hybrid import Comparator, boundIntegrator


class HybridLibraryTest(BaseAtomicTest):
    def setUp(self):
        BaseAtomicTest.setUp(self, 'test_hybrid')
        
    def tearDown(self):
        BaseAtomicTest.tearDown(self)
        
    @unittest.skip("remove the skip")
    def test_comparator(self):
        params = {"logging_backend":'hdf5'}
        self.model.set_global_parameters(**params)
        
        
        comparator = Comparator(self.model, "comparator")
        self.model.connect((self.input, 0), (comparator, 0))
        self.model.connect((comparator, 0), (self.output,0)    )
         
        # configure input
        self.set_input_events(times, values)
        self.model.run()
        
        # parameters py2pdevs 
#         self.assertEquals('-1', comparator.yl)
#         self.assertEquals('1', comparator.yu)

    def test_bounded_qss_integrator_basic_integration(self):
        net_params = {
            "tf":10,            
            "logging_backend":'hdf5'}
        self.model.set_global_parameters(**net_params)
        
        comparator = boundIntegrator(self.model, "BQI")
        self.model.connect((self.input, 0), (comparator, 0))
        self.model.connect((comparator, 0), (self.output,0)    )
        
        # configure input
        self.set_input_events(times=[0,0.1, 1, 2],
                              values=[4,5  , 6, 7])
        self.model.run()
        
        # read results
        with Plotter("test_hybrid_0.h5") as reader:
            times, values = reader.getTimeSerie_continuous("output", samples=False)
            np.testing.assert_almost_equal([0, 0.100001, 1.400005, 9.30003], times.tolist(), 5)
            np.testing.assert_almost_equal([0, 0.400005, 7.30003, 62.00021], values.tolist())

        
        
        
         