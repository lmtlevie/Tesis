import sys

# Make py2pdevs package visible. An alternative is to export the
# environment variable PYTHONPATH appeding the absolute path to build/lib
sys.path.append('../../../../build/lib')

import py2pdevs.network as network
import py2pdevs.library.datanetworks_ext as library
from py2pdevs.common.distributions import Normal, Exponential


def network_advanced_1():
    # Sweep through sender bandwidth.
    experiment = 0
    for bandwidth in [9800, 98000, 980000]:
        run_model(experiment, bandwidth)
        experiment += 1

def run_model(experiment, bandwidth):
    model_name = 'network_advanced_1'

    global_params = {"TCP.CWND_max":250, 'TCP.CWND_ssthresh': 64, 'TCP.RTT_initial': 0.2, "MSS":1024,
#                     'debugLevel': 999999999, # log debug messages (huge files and slow)
                     'TCP_SND.logLevel': 1000, # log CWND
                     'queue.logLevel': 1000, # log queues                     
                     } #
    model = network.new_model(model_name, params = global_params,
        ExperimentNumber=experiment, # set the experiment number (eg: results are stored in different files)                     
        bandwidth=bandwidth, # common link speeds in all network      
        propagationDelay=0.001)

    # Create network objects.
    sender1 = library.TcpSenderExt(model, 'Sender1', set_samplers = False)
    sender2 = library.TcpSenderExt(model, 'Sender2', firstPort=1, set_samplers = False) # both send to the same receiver, so we need to set up different ports
    router = library.RouterExt(model, 'Router1', bufferSize=2977000)
    receiver = library.TcpReceiverExt(model, 'Receiver', n_sessions=2)

    # Define the 2 data flows    
    flow1 = model.new_flow("Sender1_flow", src=sender1, dst=receiver, packet_size=Normal(3624, 800))
    flow1.set_route((sender1, 0, 0), (router, 1, 0), (receiver, 0, 0)) # route is in the form : (node1, inport, outport), ...
    flow2 = model.new_flow("Sender2_flow", src=sender2, dst=receiver, packet_size=Exponential(2000)) # use exponential packet sizes
    flow2.set_route((sender2, 0, 0), (router, 2, 0), (receiver, 0, 0)) 

    # Run model and finalize. 
    model.run()
    print ("Simulation %d finished" % experiment)
    
if __name__ == '__main__':
    network_advanced_1()
