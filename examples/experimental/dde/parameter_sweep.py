#!/usr/bin/env /bin/python3

import os
import sys
import traceback
import argparse
import multiprocessing
import copy
import pandas as pd
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor, as_completed
from pathlib import Path
from datetime import datetime

MY_DIR = os.path.dirname(os.path.realpath(__file__))
MODEL_DIR = MY_DIR + "/.."
PD_DIR = MODEL_DIR + "/../.."
OUTPUT_DIR = PD_DIR + "/output"
PLOTS_DIR = OUTPUT_DIR + "/plots/"

# Set simulation parameters file
SIMU_PARAMETER_FILE = "cellular_network_spikes.params"
SIMU_PARAMETER_FILE_PATH = "../examples/experimental/dde/"+SIMU_PARAMETER_FILE

def list_param_sweep(args, config_csv, parameters, threads):
    """
    Performs the sweeping analysis considering the corresponding values of parameter1 and parameter2.
    """
    count_iterations = len(parameters) 
    print(f"Executing a total of {count_iterations} with {threads} threads: ")

    if count_iterations == 0:
        return

    # Folder to save simulation files
    results_folder = get_results_dir(config_csv)
    os.system(f"cp {config_csv} {results_folder}") # copy the sweep file for backup
    os.system(f"cp {SIMU_PARAMETER_FILE} {results_folder}") # copy the parameters file for backup

    # Run simulations in parallel

    #with ThreadPoolExecutor(max_workers=threads) as executor:
    #    futures = []
    #    parameters_list = parameters.to_dict(orient='records')
    #    print("Parameter list for sweep: {}".format(parameters_list))

    #    for experiment_number, params in enumerate(parameters_list): # params is a list of dict with all parameters in a single line of the df
    #        print("Running experiment %d" % experiment_number)
    #        print(f'params: {params}')
    #        # avoid concurrent access to the same object
    #        config_file_options = copy.deepcopy(params) 
    #        future = executor.submit(run_and_save_simulation, experiment_number, results_folder, config_file_options)
    #        futures.append(future)

    with ProcessPoolExecutor(max_workers=threads) as executor:
        futures = []
        parameters_list = parameters.to_dict(orient='records')
        print(f"Parameter list for sweep: {parameters_list}")

        for experiment_number, params in enumerate(parameters_list):
            print("Running experiment %d" % experiment_number)
            #params['simuNumber'] = experiment_number
            #params['resultsFolder'] = results_folder
            print(f'params: {params}')
            # avoid concurrent access to the same object
            config_file_options = copy.deepcopy(params) 
            future = executor.submit(run_and_save_simulation, experiment_number, results_folder, config_file_options)
            futures.append(future)

        for future in as_completed(futures):
            try:
                future.result()  # Check for exceptions in the task
            except Exception as e:
                print("Error in process: {}".format(e))
                traceback.print_exc()
        
        # Ensure all processes are cleaned up properly
        executor.shutdown(wait=True)

def get_results_dir(name):
    """
    Creates the folder to save simulation files with current date and time.
    """
    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    results_folder = f"results_simu_{name}/{timestamp}"
    if not os.path.isdir(results_folder): 
        os.makedirs(results_folder)

    return results_folder

#def run_and_save_simulation(args, simu_number, results_folder, params):
def run_and_save_simulation(simu_number, results_folder, params):
    """
    Construct string to pass parameters as --<param_name>=<param_value>.
    """
    params_str = ""
    for param_key in params:   
        if param_key == SIMULATE_TIME_COLUMN: # special case for -tf
            params_str += f" -tf {params[param_key]}"
        else:
            params_str += f" -{param_key}={params[param_key]}"

    # Run simulation
    cmd = f"cd {OUTPUT_DIR}; /usr/bin/time -v ./model -variable_logging_backend hdf5 -c {SIMU_PARAMETER_FILE_PATH} {params_str} --ExperimentNumber={simu_number}; cd -" 
    print(f"[Simu {simu_number} - {datetime.now()}] executing: {cmd}", flush=True)
    os.system(cmd)
    print(f"[Simu {simu_number} - {datetime.now()}] finish: {cmd}", flush=True)

    # Save simulation results
    save_simulation(args, simu_number, results_folder, params)

def save_simulation(args, simu_number, results_folder, params):
    """
    Save simulation results.
    """
    #simu_file = f"cellular_network_spikes_test_{simu_number}.h5"  #file produced by each simulation. to be copied to simulations_folder
    simu_file = f"cellular_network_spikes_test_0.h5"  #file produced by each simulation. to be copied to simulations_folder
    #log_file = f"pdevs_run{simu_number}.log"  #file produced by each simulation. to be copied to simulations_folder
    log_file = f"pdevs_run0.log"  #file produced by each simulation. to be copied to simulations_folder
    
    # new_filename contains names and values  of each parameter sweeped
    new_filename = f"simu{simu_number}"
    
    if args.params_in_filesnames:
        param_names = params_to_sweep_df.columns.values.tolist()       
        print(f"param_names={param_names}")
        for idx, param_value in enumerate(params):
            new_filename += f"_{param_names[idx]}_{param_value}"
    
    # Copy debug and log files
    print(f"[Simu {simu_number}] Saving simulation in {results_folder} as {new_filename}.h5", flush=True)
    cmd = f"cp {OUTPUT_DIR}/{simu_file} " + os.path.join(results_folder, new_filename + '.h5')
    os.system(cmd)
    cmd = f"cp {OUTPUT_DIR}/{log_file} " + os.path.join(results_folder, new_filename + '.log')
    os.system(cmd)
    # copy gnuplot files
    cmd = f"ls -tr {PLOTS_DIR} | tail -n 2 | xargs -I{{}} cp {PLOTS_DIR}{{}} " + results_folder # TODO: avoid concurrent access to the same folder
    os.system(cmd)

    return os.path.join(results_folder, new_filename)

def read_sweep_config(config_csv):
    """
    Parse parameter sweeping file (CSV) and load data in a pandas' dataframe.
    Returns dataframe.
    """
    df = pd.read_csv(config_csv,
                     sep=',',     # field separator
                     comment='#', # comment
                     skipinitialspace=True,
                     skip_blank_lines=True
                    )
    df.columns = df.columns.str.strip() # remove spaces from column names
    return df

SIMULATE_TIME_COLUMN = 'simulate_time'
def set_simulation_time(parameters, simulate_time):
    """
    Set simulation time. This value must be provided as a parameter or as a column in the sweep file.
    """
    if simulate_time == None and not SIMULATE_TIME_COLUMN in parameters:
        print(f"ERROR: Simulation time not specified! It can be set as '{SIMULATE_TIME_COLUMN}' parameter to sweep \
                in the config, or use the simule_time or simule_events arguments")
        exit(1)
    
    if simulate_time:
        if SIMULATE_TIME_COLUMN in parameters:
            print(f"ERROR: simule_time can not be used if sweeping '{SIMULATE_TIME_COLUMN}'")
            exit(1)
        
        parameters[SIMULATE_TIME_COLUMN] = 1.0 * simulate_time

    return parameters

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Execute a simulation parameter sweep')    
    parser.add_argument('--processes', default=multiprocessing.cpu_count() // 2, type=int,
                        help='Number of parallel simulation to execute')
    # parser.add_argument('--config', default='config.params', type=str,
    #                     help='Config values to apply to all simulations')
    parser.add_argument('--params_to_sweep', default='parameters.sweep', type=str,
                        help='CSV file with the name and value of the parameters to sweep')
    parser.add_argument('--simulate_time', default=None, type=float, required=False,
                        help='Simulation time to run (exclusive with simulate_events argument)')
    parser.add_argument('--params_in_filesnames',action='store_true', default=False,
                        help="Set this option to save results files using long names including each parameter sweepd. \
                              Can easily cause 'file name too long' errors when sweeping several parameters")

    args = parser.parse_args() # parse command line arguments
    params_to_sweep_df = read_sweep_config(args.params_to_sweep) # load sweeping file in a dataframe
    set_simulation_time(params_to_sweep_df, args.simulate_time)
    list_param_sweep(args, args.params_to_sweep, params_to_sweep_df, args.processes)