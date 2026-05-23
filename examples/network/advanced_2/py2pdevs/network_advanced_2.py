import sys

# Make py2pdevs package visible. An alternative is to export the
# environment variable PYTHONPATH appeding the absolute path to
# build/lib and then running the script without the following
# line.
sys.path.append('../../../../build/lib')

import py2pdevs.network as network
from py2pdevs.common.distributions import Normal, Exponential

import plot


def network_advanced_2():
    # Sweep through RED parameterization.
    experiment = 1
    for red_params in [
            (50000,200000,0.1),
            (100000,300000,0.005),
            (300000,400000,0.1)]:
        print "Running experiment %i" % experiment
        run_model(experiment, red_params)
        experiment += 1

def run_model(experiment, red_params):
    model_name = 'network_advanced_2-%d' % experiment

    model = network.new_model(
        model_name,
        MSS=1024,
        propagation=0.001,
        bandwidth=20000000,
        CWND_max=250,
        ssthresh=64,
        initial_RTT=0.2)

    # Create network objects.
    sender1 = model.new_tcp_sender('Sender1', ip='10.147.0.16')
    sender2 = model.new_tcp_sender('Sender2', ip='10.147.0.17')

    router1 = model.new_RED_router('Router1',
                                   buffer_size=400000,
                                   queue_thresh=red_params[:2],
                                   discard_prob=(0,red_params[2]),
                                   alpha=0.001)
    router2 = model.new_router('Router2', buffer_size=9423700)

    receiver = model.new_tcp_receiver('Receiver', ip='10.147.0.15')

    # Make physical connections.
    model.connect((sender1,  0), (router1,  0))
    model.connect((sender2,  0), (router1,  1))
    model.connect((router1,  0), (receiver, 0))
    model.connect((receiver, 0), (router2,  0))
    model.connect((router2,  0), (sender1,  0))
    model.connect((router2,  1), (sender2,  0))

    # Define data flows.
    # flow1: from TCP port 555 in 10.147.0.16 to port 80 in 10.147.0.15.
    flow1 = model.new_flow(src=(sender1,5555), dst=(receiver,80), packet_size=Normal(3624, 800))
    flow1.set_route((sender1, 0), (router1, 0), (receiver, 0), (router2, 0))

    # flow2: from TCP port 555 in 10.147.0.16 to port 443 in 10.147.0.15.
    flow2 = model.new_flow(src=(sender2,5555), dst=(receiver,443), packet_size=Exponential(2000))
    flow2.set_route((sender2, 0), (router1, 0), (receiver, 0), (router2, 1))

    # Configure logging.
    model.log([sender1.CWND_var_for(flow1, alias='Sender1_cwnd'),
               sender2.CWND_var_for(flow2, alias='Sender2_cwnd'),
               router1.queue_size_var_for(0, alias='Router1_qsize0'),
               router1.tail_drops_var_for(0, alias='Router1_tail_drops0'),
               router1.RED_drops_var_for(0, alias='Router1_red_drops0')])

    # Run model and finalize. 
    model.run()

    finalize(model)

def finalize(model):
    plot.make_plots(model)

if __name__ == '__main__':
    network_advanced_2()
