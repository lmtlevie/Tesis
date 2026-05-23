import os
import sys

# Make py2pdevs package visible. An alternative is to export the
# environment variable PYTHONPATH appeding the absolute path to
# build/lib and then running the script without the following
# line.
sys.path.append('../../../../build/lib')

import py2pdevs.core as core
import py2pdevs.network as devs
import py2pdevs.library.queueing as queueing
from py2pdevs.common.distributions import Normal, Constant, Exponential

def top_model():
    # create top model setting global parameters
    global_params = {}
    model = devs.new_model('basic',
        params = global_params, # parameters as dictionary
        logLevel = 0, # 0 logs only explicitly configured time series. 9999999 logs all (slow and big files)
        debugLevel = 0,     # 0 does not record debug messages. 9999999 slow and big file (pdevs_log)  
        )

    # Create network objects. Parameters to individual models 
    dh = queueing.JobGenerator(model, 'DataHandler',
                               StartTime = 0,
                               Period = "DataHandler.period",
                               AvgJobWeight = "DataHandler.jobSize",
                               EventsThreshold = -1)
    sh = queueing.JobQueue(model, 'StorageHandler',
                           MaxCapacity = -1)
    ef = queueing.ProcesorSharing(model, 'EventFilter',
                                  servicePower = 400,
                                  maxJobs = 1)    

    # Connect models   
    model.connect((dh,   0), (sh, 0))
    model.connect((sh,   0), (ef,   0))
    model.connect((ef, 0)  , (sh,   1))

    
    # Configure logging 
    # this is yet another way of setting parameters (this could also be donde in a configuration file, when instanciating each model, or globaly when creating the top model)
    params = model.get_global_parameters()
    
    params.update(Exponential(0.01).serialize_for("DataHandler.period")) # TODO: find a better way to pass a distribution to a model
    params.update(Constant(5).serialize_for("DataHandler.jobSize"))
    
    logs_sample_period=0.1
    params.update(dh.set_sampler_logger("%s.count" % dh.name, "packet", logs_sample_period))
    params.update(sh.set_sampler_logger("%s.qsize" % sh.name, "packet", logs_sample_period))
    params.update(ef.set_sampler_logger("%s.finished" % ef.name, "packet", logs_sample_period))
    model.set_global_parameters(**params)        
    
    # Run model and finalize. 
    model.run()
    print ("Simulation finished")
    
if __name__ == '__main__':
    top_model()

