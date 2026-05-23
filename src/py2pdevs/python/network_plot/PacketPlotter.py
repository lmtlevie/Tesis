from __future__ import division # division of ints can return floats
import h5py
import numpy as np
import matplotlib.pyplot as plt

from Plotter import Plotter
		
class PacketPlotter(Plotter):
	def __init__(self, h5_file):
		 super(PacketPlotter, self).__init__(h5_file)
       
	def plot_cwnd(self, hostName, color="blue"):
	    times, values = self.getTimeSerie(hostName + '.TcpSession0.TCP_SND.cwnd')
	    
	    plt.plot(times, values, color=color, linewidth=2.5, linestyle="-", label=hostName +" CWND (packet)")
	    plt.xlabel('Time [s]')
	    plt.ylabel('CWND [B]')
	    plt.legend(loc=self.legend_loc,  bbox_to_anchor=self.legend_box)
	    
	def plot_send(self, hostName, color="blue"):
	    times, values = self.getTimeSerie(hostName + '.sent.size_bits.Hz')
	    
	    plt.plot(times, values, color=color, linewidth=2.5, linestyle="-", label=hostName +" sent rate (packet)")
	    plt.xlabel('Time [s]')
	    plt.ylabel('Send Rate [B/s]')
	    plt.legend(loc=self.legend_loc,  bbox_to_anchor=self.legend_box)	    
	    
	def plot_drops(self, routerName):
    	    times1, values1 = self.getTimeSerie('%s.egressPort0.queue.discard_bits' % routerName)
    	    times2, values2 = self.getTimeSerie('%s.egressPort0.RED_AQM.drop_bits' % routerName)

	    plt.plot(times1, values1, color="blue", linewidth=2.5, marker="x", linestyle='None', label="%s tail drops" % routerName)
	    plt.plot(times2, values2, color="red", linewidth=2.5, marker="x", linestyle='None', label="%s RED drops" % routerName)

	    plt.xlabel('Time [s]')
	    plt.ylabel('Queue size [b]')
	    plt.legend(loc=self.legend_loc,  bbox_to_anchor=self.legend_box)   

	def plot_qsize(self, routerName):
	    times, values = self.getTimeSerie(routerName + '.egressPort0.queue.qSize_bits')
	    
	    plt.plot(times, values, color="red", linewidth=2.5, linestyle="-", label=routerName + " queue size")
	    plt.xlabel('Time [s]')
	    plt.ylabel('Queue size [b]')
	    plt.legend(loc=self.legend_loc,  bbox_to_anchor=self.legend_box)