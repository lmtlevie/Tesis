from argparse import ArgumentParser
from configparser import ConfigParser
import io    # # import StringIO for handling unicode strings

import py2pdevs.core as core

def parse_options(custom_params=None):
    options = get_cmdline_options()

    params = custom_params or dict()
    params = {k : str(v) for k, v in params.iteritems()}
    params.update(options)
    
    set_options(params)


def get_cmdline_options(): 
    options = dict()
    parser = ArgumentParser()
    
    parser.add_argument('-tf', '--tf', dest='tf', action='store',
                        type=float, required=True,
                        help='final time of the simulation')
    
    parser.add_argument('-ti', '--ti', dest='ti', action='store',
                        type=float, default=0.,
                        help='initial time of the simulation')
    
    parser.add_argument('-b', '--max-steps', dest='max_steps', action='store',
                        default=1000000, type=int,
                        help='maximum number of simulation steps (to detect illegitimate models)')
    
    parser.add_argument('-f', '--finalization_script', dest='finalization_script', action='store',
                        default='', type=str,
                        help='Python script to perform finalization tasks')
    
    parser.add_argument('-c', '--params', dest='params', action='store',
                        default='', type=str,
                        help='file to read model parameters')
    
    parser.add_argument('-parameter_reading_backend', '--parameter_reading_backend',
                        dest='reading_backend', action='store', default='Python', type=str,
                        help='Parameter reading backend (defaults to Python)')
    
    parser.add_argument('-variable_logging_backend', '--variable_logging_backend',
                        dest='logging_backend', action='store', default='Default', type=str,
                        help='Variable logging backend (defaults to Default, which is Scilab)')
    
    #args = parser.parse_args()
    args, unknown = parser.parse_known_args() # allow unknown arguments
    params = vars(args)
    
    # reparse unknowns and add them to params
    for arg in unknown:
        if arg.startswith(("-", "--")):
            parser.add_argument(arg.split('=')[0], type=str, action='store')
    args = parser.parse_args()
    params.update(vars(args))
    
    # read also the config file if the option was set
    if args.params:
        config_params = get_config_file_options(args.params)
        config_params.update(params) # cmdline argunments have priority
        params = config_params
        
    return params

def get_config_file_options(configPath):
    # Add a fake section to make it compatible with C++ parser. ConfigParser requires the file to start with a section (which is not the case in c++)
    ini_str = '[FAKE_SECTION_NAME]\n' + open(configPath, 'r').read()
    ini_str = ini_str.replace("#", "\n#") # comments with "#" are only allowed in new lines. Better interpresed as ";" instead of "#"
    ini_fp = io.BytesIO(ini_str)
#   config = ConfigParser.SafeConfigParser() # in python2
    config = ConfigParser() # in python2/3 compatibility
    config.optionxform=str # case sensitive (like c++ parser)
    config.readfp(ini_fp)
    
    items = config._sections['FAKE_SECTION_NAME']
    
    return items

def set_options(options):
    new_options = dict()
    
    for key, value in options.iteritems():
        str_val = str(value)
        if isinstance(value, (list,tuple)):
            str_val = '{' + ','.join(map(str, value)) + '}'
        new_options[key] = str_val
        
    core.set_options(new_options)
    
