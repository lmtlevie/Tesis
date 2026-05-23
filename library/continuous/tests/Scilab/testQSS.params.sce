SimulationName = "testBoundedIntegrator";
exec('../examples/common/Scilab/model.scilabParams', 0)
exec('../examples/common/Scilab/functions.sce', 0)

if ~exists('ExperimentNumber') then
 ExperimentNumber=0;
end 
if exists('ParameterValues') & ExperimentNumber >= length(ParameterValues)  then
	ExperimentNumber=0;
end
printf('Starting Experiment %i ... \n', ExperimentNumber);

// General
StochasticSimu = 1;     // 1=YES (use STDEVS), 0=NO (use means as constant values).
ReproducibleSimu = REPRODUCIBLE_SEED;   // When using STDEVS, 1="Uses always the same random numbers (Reproducible) 0="Uses unique random numbers (generates a new seed). Any other value will be used as seed"
ParameterValues = [1]; // needs to be defined to execute

// load values from other simulation
load("../library/continuous/tests/Scilab/testQSSValues.sod")

// qder values
qder.TimeChanges = queue2.ContinousQueue.qder.value.t;
qder.ValueChanges  = queue2.ContinousQueue.qder.value.value;

// removing repeated values
//qder.TimeChanges  = cat(2, queue2.ContinousQueue.qder.value.t(1:7030), queue2.ContinousQueue.qder.value.t(7624:$));
//qder.ValueChanges  = cat(2, queue2.ContinousQueue.qder.value.value(1:7030), queue2.ContinousQueue.qder.value.value(7624:$)) / M;

// removing nans
//[nonan, numb] = thrownan(queue2.ContinousQueue.qder.value.value);
//qder.TimeChanges  = queue2.ContinousQueue.qder.value.t(numb);
//qder.ValueChanges  = nonan / M;

// test nan
//qder.TimeChanges  = [0, 1];
//qder.ValueChanges  = [1, %nan];


reset.TimeChanges = [queue2.ContinousQueue.reset.t];
reset.ValueChanges = [queue2.ContinousQueue.reset.value];

// QSS
QSS.queue.dQmin = 1e-6;
QSS.queue.dQrel = 1e-3;

                                
samplingPeriodForPlots = 0.01;


// For sweeping parameters: ./RunNSimulations.sh -n <sizeParameterValues> -f <finalTime>
ParameterNames = ["noSweeping"] ; // this must be changed in ScilabParameterSweeper::UpdateParameters
//ParameterValues = [0, 0.05, 0.1, 0.15, 0.2];
ParameterValues = [-1];                               
