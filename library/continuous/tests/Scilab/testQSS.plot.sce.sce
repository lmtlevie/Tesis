//ExperimentNumber=-1;
printf("QT:Last Simulation Finished. \n")
exec('../examples/common/Scilab/functions.sce', 0)

title_size = 6;
legendPosition = "upper_caption";legendSize = 2;
//legendPosition = "out_upper_right";legendSize = 4;

if(%T) then
	scf();	
	totalPlots=2;
	currentPlot=0;
	
	drawlater();
	
	// input
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
	plotValueAndSample(qder, 3, %t);appendLegend(["qder (integrated value)"])
	
	// integrator
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
	plotValueAndSample(q, 3, %t);appendLegend(["q (integrated value)"])
	
	drawnow();
	
end