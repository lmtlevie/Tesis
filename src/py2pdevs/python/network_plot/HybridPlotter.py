from __future__ import division # division of ints can return floats
import h5py
import numpy as np
import matplotlib.pyplot as plt

from Plotter import Plotter

class HybridPlotter(Plotter):
	def __init__(self, h5_file):
		 super(HybridPlotter, self).__init__(h5_file)
       
	def plot_drops_hybrid_packet(self, queueName, color="black"):
	   times, values= self.getTimeSerie(queueName + '.hybridMerge.discard_bits')
	   
	   plt.plot(times, values, color=color, linewidth=2.5, marker="x", linestyle='None', label="%s drops (hybrid-packet)" % queueName)
	    
	   plt.xlabel('Time [s]')
	   plt.ylabel('drops  [B/s]')
	   plt.legend(loc=self.legend_loc, bbox_to_anchor=(1.35, 1.1))	