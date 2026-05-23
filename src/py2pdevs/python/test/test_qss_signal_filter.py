import os
import unittest
import numpy as np
from mock import MagicMock, Mock, patch
from base_atomic_test import BaseAtomicTest


import network
import py2pdevs.params as params
import py2pdevs.library.sources as sources
from py2pdevs.network_plot import Plotter # to read h5, needs refactoring
from py2pdevs.library.qss_continuous import SignalLogger, Filter

class QSSSignalFilterTest(BaseAtomicTest):
    def setUp(self):
        BaseAtomicTest.setUp(self, 'test_qss_signal_filter')
        
        self.filter = Filter(self.model, "filter", 1e-6, 1e-3)
        self.model.connect((self.input, 0), (self.filter, 0))
        self.model.connect((self.filter, 0), (self.output,0)    )
        
        
    def tearDown(self):
        BaseAtomicTest.tearDown(self)
    
    def test_forward_signal_when_out_of_error_bound(self):
        params = {"logging_backend":'hdf5'}
        self.model.set_global_parameters(**params)
                 
        # configure input
        self.set_input_events(times= [0, 0.1, 1, 2, 3     , 4  ],
                              values=[4, 5  , 6, 7, 7.3, 7.1])
        self.model.run()
        
        # read results
        with Plotter("test_qss_signal_filter_0.h5") as reader:
            times, values = reader.getTimeSerie_continuous("output", samples=False)
            np.testing.assert_almost_equal([0, 0.1, 1, 2, 3, 4], times.tolist())
            np.testing.assert_almost_equal([4, 5, 6, 7, 7.3, 7.1], values.tolist())

    def test_dont_forward_signal_when_inside_error_bound(self):
        params = {"logging_backend":'hdf5'}
        self.model.set_global_parameters(**params)
                 
        # configure input
        self.set_input_events(times= [0 , 1      , 2 , 3   ,  4, 5      ,
                                       6   , 7   , 8 ],
                              values=[1, 1+1e-7, 11, 11+0.1, 12, 12+0.01,
                                       0.1, 0.1001, 0 ])
        self.model.run()
        
        # read results
        with Plotter("test_qss_signal_filter_0.h5") as reader:
            times, values = reader.getTimeSerie_continuous("output", samples=False)
            np.testing.assert_almost_equal([0,  2 , 3    , 4 , 6   , 8], times.tolist())
            np.testing.assert_almost_equal([1, 11, 11+0.1, 12, 0.1, 0], values.tolist())

    def test_forward_when_error_exceeds_dQrel(self):
        params = {"logging_backend":'hdf5'}
        self.model.set_global_parameters(**params)
                 
        # configure input
        self.set_input_events(times= [0 , 1      , 2 , 3],
                              values=[10, 10+1e-3, 10, 11],
                              values1=[0, 0      , 1 , 0])
        self.model.run()
        
        # read results
        with Plotter("test_qss_signal_filter_0.h5") as reader:
            times, values = reader.getTimeSerie_continuous("output", samples=False)
            np.testing.assert_almost_equal([0 , 2+1e-2   , 3.011], times.tolist())
            np.testing.assert_almost_equal([10, 10 + 1e-2, 11], values.tolist())
