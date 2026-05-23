import os
import sys

# Make py2pdevs package visible. An alternative is to export the
# environment variable PYTHONPATH appeding the absolute path to
# build/lib and then running the script without the following
# line.
sys.path.append('../../../../build/lib')

import py2pdevs.core as core
import py2pdevs.network as network
import py2pdevs.library.datanetworks_ext as library
from py2pdevs.common.distributions import Normal, Constant


def network_basic():
    fast_link = 10000000
    slow_link = 5000000

    # create top model. Set some global parameters
    global_params = {"TCP.CWND_max":250, 'TCP.CWND_ssthresh': 64, 'TCP.RTT_initial': 0.2, "MSS":1024}
    model = network.new_model('network_basic',
        params = global_params, # parameters as dictionary                
        bufferSize=500000, # parameters as keywords
        propagationDelay=0.001)

    # Create network objects. Parameters to individual models 
    receiver = library.TcpReceiverExt(model, 'Receiver', bandwidth=slow_link, flowId="Receiver_flow")
    router = library.RouterExt(model, 'Router', bandwidth=slow_link)
    sender =library.TcpSenderExt(model, 'Sender', bandwidth=fast_link, set_samplers = False)

#    Models can be connected in the following way.
#    For network models, it is not necesary as it     is done automatically by defining flows as below   
#     model.connect((router,   0), (receiver, 0))
#     model.connect((sender,   0), (router,   0))
#     model.connect((receiver, 0), (sender,   0))

    # Define the flow for the sender model (a flow is the path that packets will take along the network, and the size of the packets. Optionally there could be other attributes as QoS for example). 
    # Models are by default automatically connected with in duplex link. The default return path is equal to the send path reverted 
    flow_sender = model.new_flow("Sender_flow", src=sender, dst=receiver, packet_size=Normal(3624, 800))
    flow_sender.set_route((sender, 0, 0), (router, 1, 0), (receiver, 0, 0)) # route is in the form : (node1, inport, outport), ...

    flow_receiver = model.new_flow("Receiver_flow", src=receiver, dst=sender, packet_size=Constant(48))
    flow_receiver.set_route((receiver, 0, 0), (router, 0, 1), (sender, 0, 0)) 

    # Configure logging 
    # this is yet another way of setting parameters (this could also be donde in a configuration file, when instanciating each model, or globaly when creating the top model)
    params = model.get_global_parameters()
    params['TCP_SND.logLevel'] = 1000 # log CWND
    params['queue.logLevel'] = 1000 
    model.set_global_parameters(**params)        
    
    # Run model and finalize. 
    model.run()

    finalize()

def finalize():
    core.run_scilab_script('%s/../network_basic.sce' % os.getcwd())

if __name__ == '__main__':
    network_basic()
