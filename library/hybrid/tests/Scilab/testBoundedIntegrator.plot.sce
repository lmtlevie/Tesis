//ExperimentNumber=-1;
printf("QT:Last Simulation Finished. \n")
exec('../examples/common/Scilab/functions.sce', 0)

title_size = 6;
legendPosition = "upper_caption";legendSize = 2;
//legendPosition = "out_upper_right";legendSize = 4;


function [] = plotIntegrator(integratorName)
	integrator = eval(integratorName);	
	f = scf();
	
	// title
	f.figure_name= integratorName;
	xtitle(integratorName);a=get("current_axes"); a.title.font_size=title_size;	
	
	drawlater();
 	totalPlots=2;
	currentPlot=0;
	
	// title
	f.figure_name= integratorName;	
	
	// input
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
	plotValueAndSample(integrator.f_der, RGB_COLOR_RED, %T, "f_der");
	
	// integrator
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
	plotValueAndSample(integrator.f, RGB_COLOR_BLUE, %T, "f");
		
	drawnow();
endfunction

if(%T) then
	//plotIntegrator("boundInt_logicalOps");
	//plotIntegrator("boundInt_AWD");
	//plotIntegrator("boundInt_crossAndReset");
	plotIntegrator("bi"); 	
	plotIntegrator("new");	
end