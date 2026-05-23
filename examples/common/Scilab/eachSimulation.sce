exec('../examples/common/Scilab/functions.sce', 0);

// Simulation Execution already update the ExperimentNumber for next run, so use the previous value
expNumber = ExperimentNumber - 1;

printf("--------------   Simulation %i Results -------------\n", expNumber)
// pre-calculate some results (WARNING: if there is an error in these calculations, nothing will be saved!!!)


// fist of all, save results
saveSimulationResults('ScilabLogs', expNumber);

// print values in scilab
printf("-----------------------------------\n")
