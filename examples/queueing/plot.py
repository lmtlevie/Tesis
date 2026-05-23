import sys

# Make py2pdevs package visible. An alternative is to export the
# environment variable PYTHONPATH appeding the absolute path to build/lib 
sys.path.append('../../../build/lib')
sys.path.append('../build/lib')

import h5py
import matplotlib
# matplotlib.use('GTKAgg')
import matplotlib.pyplot as plt
from py2pdevs.network_plot import Plotter

def make_plots(plotter):
    plt.title('Advanced network example #1')

    plt.subplot(1, 1, 1)
    
    # queue size
    times, values = plotter.getTimeSerie("StorageHandler/qsize/avg", tryAvg=True)
    p1 = plt.plot(times, values, linewidth=2.5, linestyle="-", label="StorageHandler/qsize (jobs)")
    
    # generated jobs
    times, values = plotter.getTimeSerie("DataHandler/count/avg", tryAvg=True)
    p1 = plt.plot(times, values, linewidth=2.5, linestyle="-", label="DataHandler/count (jobs)")
   
    # finished
    times, values = plotter.getTimeSerie("EventFilter/finished/avg", tryAvg=True)
    p1 = plt.plot(times, values, linewidth=2.5, linestyle="-", label="EventFilter/finished (jobs)")
   
    
    plt.xlabel('Time [s]')
    plt.ylabel('Jobs')
    plt.legend(loc=plotter.legend_loc,  bbox_to_anchor=plotter.legend_box)
    

    plt.show()

if __name__ == '__main__':
    h5_file_name = "basic_0.h5"    
    if len(sys.argv) > 1:
        h5_file_name = sys.argv[1]

    plotter = Plotter(h5_file_name, time_range=(-1, -1))
    plotter.print_results() # print execution time and other generic metrics
    make_plots(plotter)

