from __future__ import division # division of ints can return floats
import h5py
import numpy as np
import matplotlib.pyplot as plt

from Plotter import Plotter
		
class FluidPlotter(Plotter):
	def __init__(self, h5_file):
		 super(FluidPlotter, self).__init__(h5_file)
		
	def plot_cwnd(self, hostName, color="blue"):
	    times, values = self.getQSSSampler(hostName + '.w')		
	    
	    plt.plot(times, values, color=color, linewidth=2.5, linestyle="-", label=hostName +" CWND (fluid)")
	    plt.xlabel('Time [s]')
	    plt.ylabel('CWND [B]')
	    plt.legend(loc=self.legend_loc,  bbox_to_anchor=self.legend_box)
	 
	def plot_send(self, hostName, color="blue", samples=False):
		if(not samples):
		 	times, values = self.getQSSSampler(hostName + '.sendRate')
		 	plt.plot(times, values, color=color, linewidth=2.5, linestyle="-", label=hostName +" send rate (fluid)")
		else:
			times, values, sampledTimes, sampledValues = self.getQSSSampler(hostName + '.sendRate', samples=samples)
			plt.plot(times, values, color=color, linewidth=2.5, linestyle="-", label=hostName +" send rate (fluid)")
			plt.plot(sampledTimes, sampledValues, color=color, marker="*", markersize=1)
		
		plt.xlabel('Time [s]')
		plt.ylabel('Send Rate [B/s]')
		plt.legend(loc=self.legend_loc,  bbox_to_anchor=self.legend_box)
	    
	def plot_qsize_red(self, queueName, color="blue"):
	    times, values = self.getQSSSampler(queueName + '.RED_buffer_server.queueSize')
	    	    
	    plt.plot(times, values, color=color, linewidth=2.5, linestyle="-", label=queueName + " queue size (fluid)")
	    
	    plt.xlabel('Time [s]')
	    plt.ylabel('Queue size [b]')
	    plt.legend(loc=self.legend_loc,  bbox_to_anchor=self.legend_box)
	    
	def plot_qsize(self, queueName, color="blue"):
	    times, values = self.getQSSSampler(queueName + '.buffer_server.queueSize')
	    	    
	    plt.plot(times, values, color=color, linewidth=2.5, linestyle="-", label=queueName + " queue size (fluid)")
	    
	    plt.xlabel('Time [s]')
	    plt.ylabel('Queue size [b]')
	    plt.legend(loc=self.legend_loc,  bbox_to_anchor=self.legend_box)    
 
	def plot_drops(self, queueName, color="blue"):
	    timesRED1, valuesRED1 = self.getQSSSampler(queueName + '.RED_buffer_server.REDDropsRate1')		
	    timesRED2, valuesRED2 = self.getQSSSampler(queueName + '.RED_buffer_server.REDDropsRate2')
	    times1, values1 = self.getQSSSampler(queueName + '.RED_buffer_server.tailDropRate1')		
	    times2, values2 = self.getQSSSampler(queueName + '.RED_buffer_server.tailDropRate2')
	    
	    plt.plot(times1, values1, linewidth=2.5, linestyle="-", label="%s tail drops 1 (fluid)" % queueName)
	    plt.plot(times2, values2, linewidth=2.5, linestyle="-", label="%s tail drops 2 (fluid)" % queueName)
	    
	    plt.plot(timesRED1, valuesRED1, linewidth=2.5, linestyle="-", label="%s RED drops 1 (fluid)" % queueName)
	    plt.plot(timesRED2, valuesRED2, linewidth=2.5, linestyle="-", label="%s RED drops 2 (fluid)" % queueName)
	    
	    
	    plt.xlabel('Time [s]')
	    plt.ylabel('drops  [B/s]')
	    plt.legend(loc=self.legend_loc,  bbox_to_anchor=self.legend_box)