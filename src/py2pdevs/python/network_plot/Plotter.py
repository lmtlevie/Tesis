#!python2.7

from __future__ import division # division of ints can return floats
import sys
import re
import h5py
import math
import numpy as np
import matplotlib
import datetime
#matplotlib.use('GTKAgg')
import matplotlib.pyplot as plt
import pandas as pd

from py2pdevs.common.distributions import Distribution

#TEMPLATE_TCP_SND = "%s.TcpSession.TCP_SND._%i" # for vectrorial models
TEMPLATE_TCP_SND = "%s.TcpSession_%i.TCP_SND" # for vectrorial models

        
class Plotter(object):
    
    def __init__(self, h5_file, time_range=(0, -1)):
        # L&F
        # self.legend_loc='upper right'#'best' #'upper left'
        # self.legend_box = (1.35, 1.1) # location outside the plot
        # self.axesTextSize=14#'best' #'upper left'
        
        # default styles can be foun in https://github.com/matplotlib/matplotlib/tree/master/lib/matplotlib/mpl-data/stylelib
        # for publication
        plt.rcParams.update({'font.size':24,
                            'xtick.labelsize': 30, 
                            'ytick.labelsize': 30,
                            'axes.labelsize':40,
                            'axes.grid': True,
                            'lines.markersize':12,
                            'legend.fontsize':16})
        # for testing
        plt.rcParams.update({'font.size':18,
                            'xtick.labelsize': 20, 
                            'ytick.labelsize': 20,
                            'axes.labelsize':25,
                            'axes.grid': True,
                            'lines.markersize':12,
                            'legend.fontsize':10,})

        # Use ConciseDateFormatter for dynamic date labels in the axis
        Plotter.registerConciseDateFormatter()

        self.legend_loc = 'best'
        # self.legend_box = (1, 1) # location outside the plot
        self.legend_box = None # location default        
        self.markerWeight = 3
                
        self.fileName = h5_file
        self.file = None # not opened yet
        self.file_all_keys = None
        self.file_open_counter = 0
        self.time_range = time_range
        
        # read experiment info
        with self as ref:
            final_time = None
            execution_time = None
            if self.hasTimeSerie('ExperimenetTracker/finalSimulationTime'):
                t, final_time = self.getTimeSerie('ExperimenetTracker/finalSimulationTime', time_range=(-1,-1))
                t, execution_time = self.getTimeSerie("ExperimenetTracker/execution_ms", time_range=(-1,-1))
                

            # NOTE: to be used when simulation does not finish and tracker is not recorded
            if not final_time:            
                final_time = [999999999]
            if not execution_time:
                execution_time = [999999999]

        self.final_time = final_time[0]
        self.execution_time = execution_time[0]
        
        # rearange time_range
        if self.time_range[0] == -1:
            self.time_range = (0, self.time_range[1])
        if self.time_range[1] > self.final_time or self.time_range[1] == -1:
            self.time_range = (self.time_range[0], self.final_time)
        if self.time_range[0] >= self.time_range[1]:
            raise RuntimeError('Trying to plot an invalid time range: (%d, %d) ' % (self.time_range[0], self.time_range[1]))
            
    #file access methods            
    def __enter__(self):
        if self.file_open_counter == 0:
            self.file = h5py.File(self.fileName, 'r')
            # print "File opened!"
        
        self.file_open_counter += 1            
        return self
    
    def __exit__(self, type, value, traceback):
        # TODO: Exception handling here
        if self.file_open_counter == 0:
            raise RuntimeError('Plotter:: Trying to exit object without first entering. ')
        
        self.file_open_counter -= 1
        
        if self.file_open_counter == 0:
            self.file.close()
            self.file = None
            # print "File closed!"
            
    #################### Time series methods  ################
    ###### TODO: eventually move them outside the plotter class ##### 
    def get_time_range_lenght(self):
        return self.time_range[1] - self.time_range[0]
     
    @staticmethod
    # from_sec/to_sec=-1 implies no crop from that side
    def crop(times, values, from_sec, to_sec):
        if not len(times) or not len(values):            
             return (np.asarray([]), np.asarray([]))
         
        if from_sec <= 0:
            from_sec = 0
        if to_sec <= 0:
            to_sec = times[-1] or 0
            
        idx = np.logical_and(times >= from_sec, times <= to_sec)
        return (times[idx], values[idx])
    
    @staticmethod 
    def scale2Str(scale):
        if scale==1:
            return "bits"
        elif scale == 1e3:
            return "Kb"
        elif scale == 1e6:
            return "Mb"
        elif scale == 1e9:
            return "Gb"
           
    @staticmethod    
    def downsample(times, values, down_rate=3):
        return (times[::down_rate], values[::down_rate])
    
    @staticmethod    
    def nonzeros(times, values):
        idx = np.nonzero(values)
        return (times[idx], values[idx])
    
    """
    Forecast error of two time series (time, values)
    forecast_error = expected_value - predicted_value

    For time series, each prediction point is compared against the interpolation between the 2 closest expected values 
    """
    @staticmethod
    def forecast_errors(ts_expected, ts_forecast):
        errors = []
        for time, forecast in zip(ts_forecast[0], ts_forecast[1]):
            expected = np.interp(time, ts_expected[0], ts_expected[1])
            print ("At t=%f    expected=%f ; forecast=%f" % (time, expected, forecast))
            errors.append(expected - forecast)
        
        return errors
        
    """
    Mean Squared error of two time series (time, values)
    """
    @staticmethod 
    def MSE(ts1, ts2):
        errors = np.array(Plotter.forecast_errors(ts1, ts2))
        return  np.mean(np.power(errors,2))
    
    """
    Root Mean Squared error of two time series (time, values)
    """
    @staticmethod 
    def RMSE(ts1, ts2):
        return  math.sqrt(Plotter.MSE(ts1, ts2))
    
    @staticmethod
    def compareTimeSeries(ts1, ts2, name):
        times, values = ts1
        times2, values2 = ts2
        
        equalT = Plotter.compare(times, times2, name +".times")
        equalV = Plotter.compare(values, values2, name +".values")
        if not  equalT or not  equalV:
            plt.subplot(3, 1, 1)
            plt.plot(times, values, color="blue", linewidth=2.5, marker="x", linestyle='None', label=name +" x")
            plt.legend()  
            
            plt.subplot(3, 1, 2)
            plt.plot(times2, values2, color="red", linewidth=2.5, marker="x", linestyle='None', label=name +" y")
            plt.legend()  
            
            plt.subplot(3, 1, 3)
            plt.plot(times, values, color="blue", linewidth=2.5, marker="x", linestyle='None', label=name +" x")
            plt.plot(times2, values2, color="red", linewidth=2.5, marker="x", linestyle='None', label=name +" y")
            
            plt.legend()    
            plt.show()
     
    @staticmethod    
    def compare(x,y, name):
        if x.size != y.size:
            print( name +" x size: %f ; [0]=%f ; [-1]=%f ; avg=%f ; sum=%f ; min=%f, max=%f ; non-zero size=%f ; non-zero avg=%f; non-zero min=%f" % (x.size, x[0], x[-1], np.average(x), np.sum(x), np.min(x), np.max(x),  x[np.nonzero(x)].size,  np.average(np.nonzero(x)), np.min(np.nonzero(x))))
            print( name +" y size: %f ; [0]=%f ; [-1]=%f ; avg=%f ; sum=%f ; min=%f, max=%f ; non-zero size=%f ; non-zero avg=%f; non-zero min=%f" % (y.size, y[0], y[-1], np.average(y), np.sum(y), np.min(y), np.max(y),  y[np.nonzero(y)].size,  np.average(np.nonzero(y)), np.min(np.nonzero(y))))
        elif not np.equal(x, y).all(): 
            print( name + " diff x: ", x[np.where(x != y)]   )         
            print( name + " diff y: ", y[np.where(x != y)])            
            
            return False
        else:
             print( name + " are equal")   
             return True
    
    def getParameter(self, name, type):
        value = -1
        with self as ref:
            param = self.file["Parameters/" + name]    
            value = param[0]
        return type(value)
    
    def hasParameter(self, name):
        param_path = "Parameters/" + name
        with self as ref:
            return param_path in self.file and isinstance(self.file[param_path], h5py.Dataset)

    """
    returns a from py2pdevs.common.distributions.Distribution based on the parameters
    """
    def getDistribution(self, parameterName):
        params = {parameterName :  self.getParameter(parameterName, float)}
        if self.hasParameter(f"{parameterName}_value"):
            params[f"{parameterName}_value"] = self.getParameter(f"{parameterName}_value", float)
        if self.hasParameter(f"{parameterName}_mu"):
            params[f"{parameterName}_mu"] = self.getParameter(f"{parameterName}_mu", float)
        if self.hasParameter(f"{parameterName}_sigma"):
            params[f"{parameterName}_sigma"] = self.getParameter(f"{parameterName}_sigma", float)
        # TODO: add other parameters
        return Distribution.from_config(parameterName, params)

    def hasTimeSerie (self, ts_name):
        with self as ref:
            # print (f"{ts_name}={self.file[ts_name]} . isfloat={self.file[ts_name].dtype == float} isf8={self.file[ts_name].dtype == 'f8'}")
            return ts_name in self.file and isinstance(self.file[ts_name], h5py.Dataset) and self.file[ts_name].dtype == float
            
    # Automatically crops
    def getTimeSerie(self, ts_name, time_range=None, tryAvg=False):
        time_range = time_range or self.time_range # set the actual range to use
        
        # load from h5    
        with self as ref:
            if tryAvg and not (ts_name) in self.file:
                ts_name += "/avg"                
            if not self.hasTimeSerie(ts_name):
                raise RuntimeError(f"Plotter:: '{ts_name}' time serie does not exist in file: {self.fileName}")
            times = self.file[ts_name][0]
            values = self.file[ts_name][1]
                        
        #crop
        times, values = Plotter.crop(times, values, time_range[0], time_range[1])
              
        return (times, values)  
  
    def hasTimeSerie_continuous (self, ts_name):
        keys=(ts_name + '/value/value',
              ts_name + '/accumValue/value')
        with self as ref:
            return any(elem in self.file for elem in keys)             
    
    def getTimeSerie_continuous(self, ts_name, derivative=0, samples=False, accumulated=False):
        with self as ref:
            if accumulated:
                times, values = self.getTimeSerie(ts_name + '/accumValue/value')
                 
                # When the time range is restricted, the accumulator will report on accumulated valued since the start anyway, so we compenase here
                if values.size > 0:
                    values = values - values[0] # assumes accumulated values starts in 0.
                else:
                    print( "There are no accumulated values for %s in the time range (%f, %f)" %(ts_name, self.time_range[0], self.time_range[1]))
                    # times, values = self.getTimeSerie(ts_name + '/accumulatedValue/value', time_range=(-1,-1))
                    # times = np.asarray(self.time_range[1])
                    # values = np.asarray(values[-1])
                                         
                    # print times
                    # print values
                         
            elif derivative >0:
                times, values = self.getTimeSerie("%s.value/value%i" % (ts_name, derivative))
            elif samples:
                times, values= self.getTimeSerie(ts_name + '/sampledValue/value')
            else:
                times, values = self.getTimeSerie(ts_name + '/value/value')
                     
            return (times, values)
         
    def print_results(self):
        print ("Plotting Time range = [%f, %f] (%f s)" % (self.time_range[0], self.time_range[1], self.time_range[1]-self.time_range[0]))
        print ("Simulated time: %d s" % self.final_time)
        print ("Execution time: %d ms" % self.execution_time)
              
    def print_summary(self, tsName, time_serie):
        times, values = time_serie
        if len(times)>0:
            print ("%s [%f,%f]: count=%d;  min=%f ; max=%f; avg=%f " % (
                tsName, np.min(times), np.max(times), len(values), np.min(values), np.max(values), np.average(values)))
        else:
            print ("%s [%f,%f]: empty time serie" %(tsName, self.time_range[0], self.time_range[1]))
    
    @staticmethod
    def adjust_lightness(color, amount=0.5):
        import matplotlib.colors as mc
        import colorsys
        try:
            c = mc.cnames[color]
        except:
            c = color
        # c = colorsys.rgb_to_hls(*mc.to_rgb(c))
        c = colorsys.rgb_to_hls(*mc.ColorConverter().to_rgb(c))
        return colorsys.hls_to_rgb(c[0], max(0, min(1, amount * c[1])), c[2])
    
    """
    Plots an histogram for the specified variable
    range: lamda which takes mean and std as parameters, returning a tuple (min_range, max_range). Outside the range, values are considered outliers
    e.g.: range=lambda mean,std: ((mean-2*std, mean+2*std))
    """
    def plot_histogram(self, variable_or_df, normalized=False, range=None):
        with self as ref:
            if isinstance(variable_or_df, str): 
                df = pd.DataFrame({variable_or_df: self.file[variable_or_df]})
            elif isinstance(variable_or_df, pd.Series):
                df = variable_or_df
            else:
               sys.exit("plot_histogram: first parameter 'variable_or_df' must be an instance of str or pandas.Series. Received type '%s'" % type(variable_or_df))
                
            fig = plt.figure()
            plt.title("%shistogram for %s" % ("Normalized " if normalized else "", df.name))
            
            # use the 1st column only
            # df = df[df.columns[0]]  
            mean = df.mean()
            std = df.std()
            outliers = None
            if range:
                #outliers = df[~(np.abs(stats.zscore(df)) < 3).all(axis=1)]    
                outliers = df[~df.between(range[0], range[1])]
                #print (outliers.count()) 
            
            # histogram
            x, bins, p = plt.hist(df, range=range, bins=100)
            if normalized:
                for item in p:
                    item.set_height(item.get_height()/sum(x))
                ax = plt.gca()    
                ax.relim()
                ax.autoscale_view()
            
            # add some textual info (mean, std, count, outliers)    
            plt.axvline(mean, color='k', linestyle='dashed', linewidth=1)    
            min_ylim, max_ylim = plt.ylim()
            min_xlim, max_xlim = plt.xlim()
            plt.text(min_xlim*1.01, max_ylim*0.9, 'Mean: {:f}'.format(mean))
            plt.text(min_xlim*1.01, max_ylim*0.85, 'STD: {:f}'.format(std))
            plt.text(min_xlim*1.01, max_ylim*0.75, 'count: {:d}'.format(df.count()))
            if outliers:
                plt.text(min_xlim*1.01, max_ylim*0.8, 'outliers: {:d} (min={:F}, max={:f})'.format(outliers.count(), outliers.min(), outliers.max()))
     
    def plot_timeserie(self, var_name):
        with self as ref:
            time, value = self.getTimeSerie(var_name)
            df = pd.DataFrame({'time': time, 'value': value})
            df['time'] = pd.to_datetime(df.time, unit='s')
            df.set_index('time')
            # df = df.groupby([pd.Grouper(key="time", freq="100ms")]).sum() # group by 1s (to get Hz)
            # df['sent'] = df['sent'] * 10 # to get Hz (from the 100ms grouping)
            # print(f"'{var_name}' timeserie: {df}")
            
            ax = df.plot.line(x='time', y='value', style="x-", label=var_name)
            ax.set_ylabel(var_name)
            ax.set_xlabel("Time (s)")
            ax.set_title(var_name)
            ax.xaxis_date()
            # ax.xaxis.set_major_formatter(DateFormatter("%S.%f"))
            
            plt.legend()  
            
            return df
        
    """
    match the pattern in all variable names, returning a list with the matches
       eg: plotter.extract_values("core_router.egressPort(.*).queue") => return [0,1,2,3,...]
    """
    def extract_values(self, pattern):
        with self as ref:
            if not self.file_all_keys:
                self.file_all_keys = []
                self.file.visit(lambda key: self.file_all_keys.append(key))
            compiled_pattern = re.compile(pattern)
            return set(re.findall(compiled_pattern, '\n'.join(self.file_all_keys)))

    def join_values(self, pattern):
        var_names = self.extract_values(pattern)
        # print(f"pattern={pattern}; var_names={var_names}")     
    
        all_values = np.array([])
        for var_name in var_names:
            if self.hasTimeSerie(var_name):
                # print(f"It has parameter {var_name}")
                all_values = np.append(all_values, self.getTimeSerie(var_name)[1]) 

        return all_values
    
    """
    Registers a custom ConciseDateFormatter to be used as default in all plots involving date / datetimes
    The ConciseDateFormatter generates shorter and dynamic labels for datetimes in plots. It might because the default in future matplotlib versions
    See https://matplotlib.org/stable/gallery/ticks/date_concise_formatter.html
    """
    @staticmethod    
    def registerConciseDateFormatter():
        # Code adapted from https://matplotlib.org/stable/gallery/ticks/date_concise_formatter.html

        # Custom conciseConverter
        ## TODO: for longer simulations it would be better to show days as 'day of the year' instead of day/month/year.  

        # formats: list of formats is for the normal tick labels
        formats = ['%S.%f',          # ticks are mostly years
            '%b',     # ticks are mostly months
            '%d',     # ticks are mostly days
            '%H:%M',  # hrs
            '%H:%M:%S',  # min
            '%-S.%f', ]  # secs (not zero-padded)
        # # formatter.zero_formats are the formats of the ticks that are "zeros". These are tick values that are either the first of the year, month, or day of month, or the zeroth hour, minute, or second. These are usually the same as the format of the ticks a level above
        # # these can be the same, except offset by one level....
        zero_formats = [''] + formats[:-1]
        # # ...except for ticks that are mostly hours, then its nice to have month-day
        zero_formats[3] = '%d-%b'

        # #  offset_formats: how the "offset" string on the right of the axis is formatted. This is usually much more verbose than the tick labels
        
        offset_formats = ['',
                        '%Y',
                        '%b %Y',
                        '%d %b %Y',
                        '%d %b %Y',
                        '', ] # dont offset when plotting only seconds (default %d %b %Y %H:%M)
        # does not show offset                        
        converter = matplotlib.dates.ConciseDateConverter(formats=formats, zero_formats=zero_formats, offset_formats=offset_formats, show_offset=True)
        # converter = matplotlib.dates.ConciseDateConverter(formats=formats, show_offset=False)

        # default conciseConverter
        # converter = matplotlib.dates.ConciseDateConverter()
        
        # register
        matplotlib.units.registry[np.datetime64] = converter
        matplotlib.units.registry[datetime.date] = converter
        matplotlib.units.registry[datetime.datetime] = converter