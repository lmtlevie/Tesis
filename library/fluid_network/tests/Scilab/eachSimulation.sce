// Simulation Execution already update the ExperimentNumber for next run, so use the previous value
expNumber = ExperimentNumber - 1;

printf("--------------   Simulation %i Results -------------\n", expNumber)
// pre-calculate some results (WARNING: if there is an error in these calculations, nothing will be saved!!!)


// fist of all, save results
dt = getdate();
backupDirectory = sprintf('ScilabLogs/%s_%i%.2i%.2i/', SimulationName, dt(1),dt(2),dt(6));
logFileName = sprintf('%s_%i_%i%.2i%.2i_%2i%2i%2i', SimulationName, expNumber,  dt(1),dt(2),dt(6), dt(7), dt(8), dt(9));
mkdir(backupDirectory);

printf("making a backup of all variables at " + + backupDirectory + logFileName + ".sod   ...\n")
unix_g('cp pdevs*.log ' + backupDirectory ); // backup log
unix_g('cp *.performance.log ' + backupDirectory ); // backup performance log
save(backupDirectory + logFileName + '.sod'); // saving all variables 


// print values in scilab
printf("-----------------------------------\n")
