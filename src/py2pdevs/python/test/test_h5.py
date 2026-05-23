import unittest
from base_test import BaseTest

import network
import py2pdevs.params
from network.h5 import HDF5Reader

@unittest.skip("missing file python/test/data/h5test.h5. We need to generate it, add it to git and re test")
class H5Test(BaseTest):
    def setUp(self):
        BaseTest.setUp(self)
            
        self.model = network.new_model('python/test/data/h5test')
        self.model.set_global_parameters(
            bandwidth=100,
            propagation=0.01,
            buffer_size=100)
                
        self.reader = HDF5Reader(self.model)
        
    def test_read_single_variable(self):
        router = self.model.new_router('Router1')
        qsize_var = router.queue_size_var_for(0, alias='router1_qsize')
        
        self.model.log(qsize_var)
        
        times, values = self.reader.read_variable(qsize_var)
        
        self.assertEquals(len(times), 31)
        self.assertEquals(len(values), 31)
        self.assertEquals(values[-1], 16593)
        
        times1, values1 = self.reader.read_variable('router1_qsize')
        
        self.assertEquals(times1, times)
        self.assertEquals(values1, values)
    
    def test_read_all_variables(self):
        router = self.model.new_router('Router1')
        
        self.model.log(router.queue_size_var_for(0, alias='var1'))
        self.model.log(router.queue_size_var_for(1, alias='var2'))
        
        data = self.reader.read_variables(router)
        
        self.assertEquals(len(data), 2)
        
        times, values = data['var1']
        self.assertEquals(len(times), 31)
        self.assertEquals(len(values), 31)
        self.assertEquals(values[-1], 16593)
        
    def test_variable_not_found(self):
        self.assertRaises(RuntimeError, self.reader.read_variable, 'invalid_alias')
        
        router = self.model.new_router('Router1')
        var = router.queue_size_var_for(0, alias='var1')
        var.NAME = 'invalid'
        self.model.log(var)
        
        self.assertRaises(RuntimeError, self.reader.read_variable, 'var1')