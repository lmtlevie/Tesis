import unittest
from base_test import BaseTest

import network
import py2pdevs.params
from network.objects import TCPSender
from network.loggers import SamplerLogger


class LoggingTest(BaseTest):
    def setUp(self):
        BaseTest.setUp(self)    
            
        self.model = network.new_model('test model')
        self.model.set_global_parameters(
            bandwidth=100,
            propagation=0.01,
            MSS=1024,
            CWND_max=250,
            ssthresh=64,
            initial_RTT=0.2,
            buffer_size=100)
        
    def test_sender_log_config(self):
        sender = self.model.new_tcp_sender('Sender')
        receiver = self.model.new_tcp_receiver('Receiver')

        flow1 = self.model.new_flow("Flow1", src=(sender,5555), dst=(receiver,80))
        flow1.set_route((sender, 0), (receiver, 0), addReturnRoute=False, addConnections=False)
                
        flow2 = self.model.new_flow("Flow2", src=(sender,5556), dst=(receiver,443))
        flow2.set_route((sender, 0), (receiver, 0), addReturnRoute=False, addConnections=False)        
        
        self.model.log([sender.CWND_var_for(flow1), sender.CWND_var_for(flow2)])
        
        params = self.model.serialize()

        for idx in [0, 1]:        
            logger = 'Sender.TcpSession%d.TCP_SND.cwnd.logger' % idx
            self.assertIn(logger, params)
            self.assertEquals(params[logger], 0)
            
            log_level = 'Sender.TcpSession%d.TCP_SND.cwnd.logLevel' % idx
            self.assertIn(log_level, params)
            self.assertEquals(params[log_level], 100)
            
    def test_router_log_config(self):
        router = self.model.new_router('Router')

        self.model.log(router.queue_size_var_for(0), logger=SamplerLogger())
        
        params = self.model.serialize()
        
        logger = 'Router.egressPort0.queue.qSize_bits.logger'
        self.assertIn(logger, params)
        self.assertEquals(params[logger], SamplerLogger.CODE)
        
        log_level = 'Router.egressPort0.queue.qSize_bits.logLevel'
        self.assertIn(log_level, params)
        self.assertEquals(params[log_level], 100)