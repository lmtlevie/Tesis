import unittest
from base_test import BaseTest
from mock import MagicMock, Mock, patch, call
 
import network
import py2pdevs.params as params
from common.distributions import Normal, Exponential


class FlowTest(BaseTest):
    def setUp(self):
        BaseTest.setUp(self)
            
        self.model = network.new_model('test model')
        self.model.set_global_parameters(
            bandwidth=100,
            propagation=0.01,
            MSS=1024,
            CWND_max=250,
            ssthresh=64,
            initial_RTT=0.2)        
    
    def test_basic_flow(self):
        sender = self.model.new_tcp_sender('Sender')
        receiver = self.model.new_tcp_receiver('Receiver')
        
        flow = self.model.new_flow("Flow1", src=sender, dst=receiver)
        flow.set_route((sender, 3, 0), (receiver, 0), addReturnRoute=False, addConnections=False)
        
        flow_in_sender = sender.flows[0][0]
        receiver = flow_in_sender.get_dst()
        
        self.assertEquals('Flow1', flow_in_sender.get_name())
        self.assertEquals('Receiver', receiver.get_name())
        
        params = flow.serialize()
        
        self.assertIn('Flow1.route', params)
        self.assertEquals(['Sender', 'Receiver'], params['Flow1.route'])
        
        self.assertIn('Flow1.route.outPorts', params)
        self.assertEquals([0, 0], params['Flow1.route.outPorts'])
        
        self.assertIn('Flow1.route.inPorts', params)
        self.assertEquals([3, -1], params['Flow1.route.inPorts'])
        
        self.assertIn('Flow1.typeOfService', params)
        self.assertEquals(0, params['Flow1.typeOfService'])
        
        self.assertIn('Flow1.startStopTimes', params)
        self.assertEquals([0], params['Flow1.startStopTimes'])
        
    def test_flow_with_distributions(self):
        sender = self.model.new_tcp_sender('Sender')
        receiver = self.model.new_tcp_receiver('Receiver')
        
        flow = self.model.new_flow("Flow1", src=sender, dst=receiver)
        flow.set_packet_size_distribution(Normal(1.1, 0.1))
        flow.set_period_distribution(Exponential(2))
        flow.set_route((sender, 0), (receiver, 0), addReturnRoute=False, addConnections=False)
        
        params = flow.serialize()
        
        self.assertIn('Flow1.period', params)
        self.assertEquals(Exponential.CODE, params['Flow1.period'])
        self.assertIn('Flow1.period_mu', params)
        self.assertEquals(2, params['Flow1.period_mu'])
        
        self.assertIn('Flow1.packetSize', params)
        self.assertEquals(Normal.CODE, params['Flow1.packetSize'])
        self.assertIn('Flow1.packetSize_mu', params)
        self.assertEquals(1.1, params['Flow1.packetSize_mu'])
        self.assertIn('Flow1.packetSize_var', params)
        self.assertEquals(0.1, params['Flow1.packetSize_var'])
        
    def test_flow_names_in_model_serialization(self):
        sender1 = self.model.new_tcp_sender('Sender 1')
        receiver1 = self.model.new_tcp_receiver('Receiver 1')
        receiver2 = self.model.new_tcp_receiver('Receiver 2')
        
        flow1 = self.model.new_flow("Flow1", src=sender1, dst=receiver1)
        flow2 = self.model.new_flow("Flow2", src=sender1, dst=receiver2)
        
        params = self.model.serialize()
        
        self.assertIn('PacketFlowNames', params)
        self.assertEquals(['Flow1', 'Flow2'], params['PacketFlowNames'])
        
    def test_connect_route(self):
        # prepare
        parent = Mock()
        node1 = Mock()
        node1.parent = parent
        node2 = Mock()
        node2.parent = parent
        node3 = Mock()
        node3.parent = parent        
        flow = network.flow.NetworkFlow(
            name= "Flow1",
            src=(node1,0),
            dst=(node3,0))
        
        # test
        flow.set_route((node1, 0), (node2, 2, 3), (node3,5, 0), (node1, 1), addReturnRoute=False, addConnections=False)
        parent.connect.assert_not_called()
        flow.connect_route()
        
        # assert
        calls= [call((node1, 0), (node2, 2)),
                call((node2, 3), (node3, 5)),] # (node3 -> node1) can not be connected as not input port specified
        self.assertEquals(2, parent.connect.call_count)
        parent.connect.assert_has_calls(calls, any_order=True)
        
    def test_add_return_route(self):
        # prepare
        node1 = MagicMock()
        node1.__repr__= lambda x: "Node1"
        node2 = MagicMock()
        node2.__repr__= lambda x: "Node2"
        node3 = MagicMock()
        node3.__repr__= lambda x: "Node3"  
        node4 = MagicMock()
        node4.__repr__= lambda x: "Node4"     
        flow = network.flow.NetworkFlow(
            name= "Flow1",
            src=(node1,0),
            dst=(node3,0))
        
        # test
        flow.set_route((node1, 0), (node2, 2, 3), (node3, 5, 0), (node4, 0), addReturnRoute=False, addConnections=False)
        self.assertEquals([node1, node2, node3, node4], flow.hosts)
        flow.add_return_route()
        
        # assert
        self.assertEquals([node1, node2, node3, node4, node3, node2, node1], flow.hosts)
        self.assertEquals([-1, 2, 5, -1, 0, 3, 0], flow.inports)
        self.assertEquals([0, 3, 0, 0, 5, 2, -1], flow.outports)        
        