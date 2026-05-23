import os
import unittest
from mock import MagicMock, Mock, patch
from base_test import BaseTest


import network
import py2pdevs.params as params
from py2pdevs.library.datanetworks_highlevel import  TcpSender, TcpReceiver, Router


class ModelTest(BaseTest):
    def setUp(self):
        BaseTest.setUp(self)
        
        self.model = network.new_model('test_model')
        
    def tearDown(self):
        #         os.system('rm -rf test_model_0.h5') 
        pass
       
    
    def test_default_params(self):
        self.model.set_global_parameters(slowLink=1000, delay=0.001)
        params = self.model.serialize()
        
        self.assertIn('slowLink', params)
        self.assertEquals(1000, params['slowLink'])
        
        self.assertIn('delay', params)
        self.assertEquals(0.001, params['delay'])
    
    def test_connect(self):
        src = Mock()
        dst = Mock()
        
        # internal connections
        self.model.connect((src, 1), (dst, 2))
        self.assertIn((src, 1, dst, 2), self.model.internal_connections)
        self.model.connect(src, (dst, 3))
        self.assertIn((src, 0, dst, 3), self.model.internal_connections)
        self.model.connect((src, 3), dst)        
        self.assertIn((src, 3, dst, 0), self.model.internal_connections)
        
        # output connections
        self.model.connect((src, 1), 2)
        self.assertIn((src, 1, 2), self.model.output_connections)
        self.model.connect(src, 3)
        self.assertIn((src, 0, 3), self.model.output_connections)
        
        # input connections
        self.model.connect(2, (dst, 1))
        self.assertIn((2, dst, 1), self.model.input_connections)
        self.model.connect(3, dst)
        self.assertIn((3, dst, 0), self.model.input_connections)
        
        # assert
        self.assertRaises(RuntimeError, self.model.connect, 3, 5)
    
    def test_connect_should_not_notify_src_and_dst(self):
        src = Mock()
        dst = Mock()
        
        # inner connection
        self.model.connect((src, 1), (dst, 2))                
        src.connected.assert_called_with(src, 1, dst, 2)
        src.connected_to_input.assert_not_called()
        src.connected_to_output.assert_not_called()
        dst.connected.assert_called_with(src, 1, dst, 2)
        dst.connected_to_input.assert_not_called()
        dst.connected_to_output.assert_not_called()
        
        # out connection
        src.reset_mock()
        dst.reset_mock()
        self.model.connect((src, 1), 4)                
        src.connected.assert_not_called()
        src.connected_to_input.assert_not_called()
        src.connected_to_output.assert_called_with(1, 4)
        dst.connected.assert_not_called()
        dst.connected_to_input.assert_not_called()
        dst.connected_to_output.assert_not_called()
        
        # in connection
        src.reset_mock()
        dst.reset_mock()
        self.model.connect(3, dst)                
        src.connected.assert_not_called()
        src.connected_to_input.assert_not_called()
        src.connected_to_output.assert_not_called()
        dst.connected.assert_not_called()
        dst.connected_to_input.assert_called_with(0, 3)
        dst.connected_to_output.assert_not_called()
        
        
    def test_connect_should_not_repeat_connections(self):
        src = Mock()
        dst = Mock()
        
        #internal connections
        self.model.connect((src, 1), (dst, 2))
        self.assertIn((src, 1, dst, 2), self.model.internal_connections)
        self.model.connect((src, 1), (dst, 2))
        self.assertEqual(1, len(self.model.internal_connections))
        
        # output connections
        self.model.connect((src, 1), 2)
        self.assertIn((src, 1, 2), self.model.output_connections)
        self.model.connect((src, 1), 2)
        self.assertEqual(1, len(self.model.output_connections))
        
        # input connections
        self.model.connect(3, (src, 1))
        self.assertIn((3, src, 1), self.model.input_connections)
        self.model.connect(3, (src, 1))
        self.model.connect(4, (src, 1))
        self.assertIn((4, src, 1), self.model.input_connections)
        self.assertEqual(2, len(self.model.input_connections))
        
    @unittest.skip("Need to review the logging objects in py2pdevs")    
    def test_full_model_run(self):
        net_params = {"bandwidth":1000,
            "propagationDelay":0.001,
            "MSS":1024,
            "TCP.CWND_ssthresh":64,
            "TCP.RTT_initial":0.2,
            "TCP.CWND_max":250,
            "bufferSize":10000,
            "tf":1,
            "max_steps":10000,
            "finalization_script":str(),
            "reading_backend":'Python',
            "logging_backend":'hdf5'}
        self.model.set_global_parameters(**net_params)
        
        sender = TcpSender(self.model, 'sender', FlowName="Flow1")
        router = Router(self.model,'router')
        receiver = TcpReceiver(self.model, 'receiver')
        
        flow = self.model.new_flow("Flow1", src=(sender, 10887), dst=(receiver, 21))
        flow.set_route((sender, 0), (router, 0), (receiver, 0))
        
        self.model.connect((sender, 0), (router, 0))
        self.model.connect((router, 0), (receiver, 0))
        self.model.connect((receiver, 0), (sender, 0))
        
        # TODO: fix logging mechanisms
#         self.model.log(sender.CWND_var_for(flow, alias='cwnd'))
        
        self.model.run()
        
        reader = self.model.get_h5_reader()
        times, values = reader.read_variable('cwnd')
        
        self.assertNotEquals(0, len(times))
        self.assertNotEquals(0, len(values))