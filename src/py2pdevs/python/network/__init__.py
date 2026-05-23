from .model import RootModel

import py2pdevs.core as core
import py2pdevs.params as parameters


def new_model(name=None, time=None, params=[], **global_params):
    name = name or 'Unnamed network model'
    time = time or 0.
    model_obj = core.new_model(time, name)
    model_obj.printStatus = True
    model = RootModel(model_obj, name)
    
    options = parameters.get_cmdline_options()
    options.update(params)
    options.update(global_params)
    model.set_global_parameters(**options)
    parameters.set_options(options)
    
    # add a tracker model to track simulations times
    model.interface.create_simulationexperimenttracker(model.pd_model(), "ExperimenetTracker", ["SimulationName","ExperimentNumber","ParameterValues","ParameterNames"])
    
    return model