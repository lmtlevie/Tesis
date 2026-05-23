//ExperimentNumber=-1;
printf("QT:Last Simulation Finished. \n")




scf();
legendPosition = "upper_caption";
totalPlots=3;
currentPlot=0;

// output of saturation should be the same as the input (because it never saturates).
// bugfix:
// without this fix the ramp down is lot after saturation because the input reaches exactly the limit of the saturation

currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot); 
plotValueAndSample(input, 3); appendLegend(["input", ""], leg=legendPosition);

currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot); 
plotValueAndSample(outSat, 3); appendLegend(["saturation output", ""], leg=legendPosition);