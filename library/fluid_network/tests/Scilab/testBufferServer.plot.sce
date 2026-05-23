//ExperimentNumber=-1;
printf("QT:Last Simulation Finished. \n")
exec('../examples/common/Scilab/functions.sce', 0)
exec('../examples/common/Scilab/TcpContinuous.plot.functions.sce', 0)

// Perform post processing and plotting here
title_size = 4;
//legendPosition = "upper_caption";legendSize = 2;
legendPosition = "out_upper_right";legendSize = 4;
//legendPosition = "in_upper_right";legendSize = 4;

// plot continuous queue
if(%T) then
	scf();	
	totalPlots=4;
	currentPlot=0;
	
	// title
	f = get("current_figure");
	f.figure_name="continuousTopology";
	xtitle(f.figure_name);a=get("current_axes"); a.title.font_size=title_size;	
	
	drawlater();
	
	////////////// TCPs
	// Continuous W
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
	plotValueAndSample(TcpHost1.w, RGB_COLOR_RED, %T, "Fluid TCP Window 1", legendPosition, legendSize);
	plotValueAndSample(TcpHost2.w, RGB_COLOR_BLUE, %T, "Fluid TCP Window 2", legendPosition, legendSize);
	 
	// Continuous input rates
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot); 
	plotValueAndSample(TcpHost1.sendRate, RGB_COLOR_RED); appendLegend(["Fluid Send Rate 1"], legendPosition, legendSize);
	plotValueAndSample(TcpHost2.sendRate, RGB_COLOR_BLUE); appendLegend(["Fluid Send Rate 2"], legendPosition, legendSize);
	
	//////////////////// Queues		
	// Queue 1 size
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);	
	plotValueAndSample(buffer_server.q, RGB_COLOR_BLUE, %T, "Fluid Queue Size 1", legendPosition, legendSize);	
	
			
	drawnow();
	
	// maximize
	f = get("current_figure");
	f.figure_position = [-8,-8];
	f.figure_size = [2500,1556];
	fixMargins(totalPlots)
	
end
