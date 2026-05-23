from __future__ import division # division of ints can return floats
import sys
sys.path.append('/powerdevs/examples/common/python/network_plot') 

from Plotter import Plotter
import h5py
import numpy as np
import matplotlib
matplotlib.use('GTKAgg')
import matplotlib.pyplot as plt

def plot_comparison(plotter, loggerName, legend):
    totalPlots = 2
    currentPlot = 0
    
    Qmax = 250 #plotter.getParameter("maxValue", type=int)

    # QSS Integrator
    currentPlot += 1 
    plt.subplot(totalPlots, 1, currentPlot)
    times, values = plotter.getTimeSerie_continuos('input')
    times = np.append(times, plotter.final_time) ## add a ficticious last value so that the plot shows values until the end
    values= np.append(values, values[-1])
    plt.plot(times, values, color="red", markersize=15, markeredgewidth=2, marker="x", drawstyle='steps-post', label="Input")
    plt.xlim([0, plotter.final_time])
    
    if not loggerName == "integra":
        plt.axvline(x=3, color="black", linestyle='--') # Qmax
    
    #plt.xlabel('Time [s]')
    plt.ylabel("Value f'")
    plt.grid()
    plt.minorticks_on()
    plt.legend()
    
    currentPlot += 1 
    plt.subplot(totalPlots, 1, currentPlot)
    times, values = plotter.getTimeSerie_continuos(loggerName) 
    plt.plot(times, values, color="blue", markersize=15, markeredgewidth=2, marker="x", linestyle='None', label=legend) # events
    times, values = plotter.getTimeSerie_continuos(loggerName, samples=True)
    plt.plot(times, values, color="blue") # samples
    plt.hlines(Qmax,  0,  plotter.final_time, color="black", linestyle='--') # Qmax
    plt.hlines(0,  0,  plotter.final_time, color="black", linestyle='--') # Qmin=0
    
    
    if not loggerName == "integra":
        plt.axvline(x=3, color="black", linestyle='--') # Qmax
    
    
    plt.xlabel('Time [s]')
    plt.ylabel("Value f")
    plt.legend()
    plt.xlim([0, plotter.final_time]) # plot until end of simulation    
    plt.grid()
    plt.minorticks_on()
   
    plt.show()
#     plt.savefig(loggerName+".pdf", bbox_inches='tight')
    

if __name__ == '__main__':
    h5FileName = "testBoundedIntegrator_0.h5"   
    if len(sys.argv) > 1:
        h5FileName = sys.argv[1]    
    plotter = Plotter(h5FileName)
    
    print "Simulated time: %d s" % plotter.final_time
    print "Execution time: %d ms" % plotter.execution_time
       
    plot_comparison(plotter, "integra", legend="QSS Integrator")   
    plot_comparison(plotter, "bound", legend="QBI Integrator")
    plot_comparison(plotter, "saturat", legend="Integrator+Saturation")
    
    
