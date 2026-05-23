import sys

# Make py2pdevs package visible. An alternative is to export the
# environment variable PYTHONPATH appeding the absolute path to build/lib 
sys.path.append('../../../build/lib')
sys.path.append('../build/lib')

import h5py
import matplotlib
matplotlib.use('GTKAgg')
import matplotlib.pyplot as plt
from py2pdevs.network_plot import Plotter

def plot_cwnd_packet(plotter, hostName, session_index=-1, color="blue", legendName=""):
    
    if not legendName:
        legendName = hostName
    
    if session_index >= 0:
        tsName=(TEMPLATE_TCP_SND + "/cwnd") % (hostName, session_index)
    else:
        tsName = hostName + '/TcpSession/TCP_SND/cwnd'
        
    times, values = plotter.getTimeSerie(tsName, tryAvg=True)

    p1 = plt.plot(times, values, color=color, linewidth=2.5, linestyle="-", label="%s CWND %i (packet)" % (legendName, session_index))
    plt.xlabel('Time [s]')
    plt.ylabel('CWND [pkts]')
    plt.legend(loc=plotter.legend_loc,  bbox_to_anchor=plotter.legend_box)
    
    return p1

def plot_qsize_packet(plotter, routerName, port=0, withREDAvg=False, scale=1, color="blue"):
    
    tsName = routerName + '/egressPort%d/queue/qSize_bits' % port
    
    times, values = plotter.getTimeSerie(tsName, tryAvg=True)   
    plt.plot(times, values/scale, color=color, linewidth=1,  linestyle="-", label="%s(%d) queue size" % (routerName, port))
    
    if withREDAvg:
        times, values = plotter.getTimeSerie(routerName + '/egressPort%d/RED_AQM/queueAvg' % port, tryAvg=True)
        plt.plot(times, values/scale, color=Plotter.adjust_lightness(color, 0.5),  linewidth=1.5, linestyle="-", label="%s(%d) RED avg" % (routerName, port))
            
    plt.xlabel('Time [s]')
    plt.ylabel('Queue size [%s]' % Plotter.scale2Str(scale))
    plt.legend(loc=plotter.legend_loc,  bbox_to_anchor=plotter.legend_box)

def plot_drops_packet(plotter, routerName, port=0, withRED=False, scale=1, color='blue'):
    
    times1, values1 = plotter.getTimeSerie('%s/egressPort%d/queue.discard_bits' % (routerName, port))
    plt.plot(times1, values1/scale, color="blue", linewidth=2.5, marker="x", linestyle='None', label="%s(%d) tail drops" % (routerName, port))
    
    if withRED:
        times2, values2 = plotter.getTimeSerie('%s/egressPort%d/RED_AQM/drop_bits' % (routerName, port), tryAvg=True)
        times2, values2 = Plotter.nonzeros(times2, values2)
        plt.plot(times2, values2/scale, color=Plotter.adjust_lightness(color, 0.5), linewidth=1.5, marker="x", linestyle='None', label="%s(%d) RED drops" % (routerName, port))

    plt.xlabel('Time [s]')
    plt.ylabel('Queue size [%s]' % Plotter.scale2Str(scale))
    plt.legend(loc=plotter.legend_loc,  bbox_to_anchor=plotter.legend_box)

def make_plots(plotter):
    plt.title('Advanced network example #1')

    plt.subplot(3, 1, 1)
    plot_cwnd_packet(plotter, "Sender1", session_index=0, color="blue")
    
    plt.subplot(3, 1, 2)
    plot_cwnd_packet(plotter, "Sender2", session_index=0, color="red")

    plt.subplot(3, 1, 3)
    plot_qsize_packet(plotter, "Router1", port=0,scale=1e6, withREDAvg=False, color="blue")
    plot_drops_packet(plotter, "Router1", port=0,scale=1e6, withRED=False, color="red")

    plt.show()

if __name__ == '__main__':
    h5_file_name = "network_advanced_1_0.h5"    
    if len(sys.argv) > 1:
        h5_file_name = sys.argv[1]

    plotter = Plotter(h5_file_name, time_range=(-1, -1))
    plotter.print_results() # print execution time and other generic metrics
    make_plots(plotter)

