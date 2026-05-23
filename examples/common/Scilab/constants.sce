// this file defines constants (just for readability)

// Distributions (Check matching in ParameterReader::DISTRIBUTION_PARAMETER enum)
DISTRIBUTION_CONSTANT = 0;      // parameters: 'value' 
DISTRIBUTION_EXPONENTIAL = 1;   // parameters: 'mu'
DISTRIBUTION_PARETO = 2;       // parameters: 'shape' and 'scale'
DISTRIBUTION_SLIT = 3;		   // parameters: 'val1', 'val2' and 'mean'. Like a bernulli, but with values different from 0 or 1.	
DISTRIBUTION_NORMAL = 4;	   // parameters: 'mu', 'var'  
DISTRIBUTION_FELIX = 5;        // parameters: 'period', 'mode'. If(mode==DISTRIBUTION_FELIX_MODE_HIGH_THROUGHOUT) 'size_bytes', 'buffer_bytes','timeout', 'out_size_bytes'
    FELIX_MODE_LOW_LATENCY = 0;
    FELIX_MODE_HIGH_THROUGHOUT = 1;
    
// Loggers . 
// To be configured with <modelName>.<varName>.logger variable 
// All these loggers will use the configured logging backend (Scilab or CmdLine, set with option variable_logging_backend)
//(Check matching in ConfigurationLogger::CONFIGURATION_LOGGERS enum)
LOGGER_ALL = 0;      // logs everything 
LOGGER_SAMPLER = 1;   // parameters: 'sampling_period' samples events that represent continuous signals
LOGGER_DISCRETE_SAMPLER = 2;   // parameters: 'sampling_period' samples discrete events

// RANDOM SEEDS (from STDEVS.h) (any other will be taken as a custom seed if you want to reproduce a particular simulation)
REPRODUCIBLE_SEED = 1 ; // Uses always the same random numbers (Reproducible)
UNIQUE_SEED = 0;       // Uses unique random numbers (generates a new seed).
CUSTOM_SEED = "set_your_seed_here"; // Any other value (different from 0 or -1) will be used as a custom seed (to reproduce a past simulation) 

// Network usual notation
k=1E3; // Kilobit unit
M=k^2; // Megabit unit
G=k^3; // Gigabit unit
B2b=8;  // Bytes to bits

// log levels
LOG_LEVEL_NONE = 0;
LOG_LEVEL_IMPORTANT = 100;
LOG_LEVEL_PRIORITY = 1000;
LOG_LEVEL_FULL_LOGGING = 10000;
LOG_LEVEL_ALL = 99999999;
LOG_LEVEL_INIT = 10;


// Colors in RGB
RGB_COLOR_BLACK = [0, 0, 0]; 
RGB_COLOR_RED = [1, 0, 0]; 
RGB_COLOR_PINK = [1, 0.75, 0.75];
RGB_COLOR_GREEN = [0, 0.56, 0]; 
RGB_COLOR_LIGHTGREEN = [0, 1, 0];
RGB_COLOR_BLUE = [0, 0, 1];
RGB_COLOR_LIGHTBLUE = [0.53, 0.81, 1]; 

