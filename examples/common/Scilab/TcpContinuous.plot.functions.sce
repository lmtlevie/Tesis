exec('../examples/common/Scilab/functions.sce', 0)
function [] = plotQueue(queueName)
	queue = eval(queueName);	
	f = scf();
	
	//////////// Just queue size
	// title
	f.figure_name= queueName;
	xtitle(queueName);a=get("current_axes"); a.title.font_size=title_size;	
	
	// queue size	
	plotValueAndSample(queue.queueSize, RGB_COLOR_BLUE); legend([queueName + "queueSize", queueName + "sampledQueueSize"], leg=legendPosition);
	plotValueAndSample(queue.qSize_avg, RGB_COLOR_RED); appendLegend([queueName + " queueSize avg", ""], leg=legendPosition);
	ylabel("$\mbox{bits}$");
	
	
	/////////////// other things of the queue too
	// title
	scf();
	f.figure_name= queueName;
	xtitle(queueName);a=get("current_axes"); a.title.font_size=title_size;	
	
	drawlater();
 	totalPlots=3;
	currentPlot=0;
	
	// queue size	
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
	plotValueAndSample(queue.queueSize, RGB_COLOR_BLUE); legend([queueName + "queueSize", queueName + "sampledQueueSize"], leg=legendPosition);
	plotValueAndSample(queue.qSize_avg, RGB_COLOR_RED); appendLegend([queueName + " queueSize avg", ""], leg=legendPosition);
	ylabel("$\mbox{bits}$");
	
	//  output departure rate
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);	
	plotValueAndSample(queue.departureRate1, RGB_COLOR_BLUE);  appendLegend([queueName + "departureRate1", ""], leg=legendPosition);
	plotValueAndSample(queue.departureRate2, RGB_COLOR_RED);	appendLegend([queueName + "departureRate2", ""], leg=legendPosition);
	ylabel("$\mbox{bits/second}$");
	a=gca(); a.data_bounds(4) = max(queue.outputLink.Capacity) * 1.05; //  increase the top limit of the plot (to see the max value)
	
	// discrete packet discard rate
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
	plotValueAndSample(queue.dropRate1, RGB_COLOR_RED);  appendLegend([queueName + "discardRate1", ""], leg=legendPosition);
	plotValueAndSample(queue.dropRate2, RGB_COLOR_BLUE);	appendLegend([queueName + "discardRate2", ""], leg=legendPosition);	
	ylabel("$\mbox{bits/second}$");
	
	// RED Drops Rate 
    //currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
	//plotValueAndSample(queue.REDDropsRate1, RGB_COLOR_RED);  appendLegend(["RED drops 1", ""], leg=legendPosition);
	//plotValueAndSample(queue.REDDropsRate2, RGB_COLOR_BLUE);	appendLegend(["RED drops 2", ""], leg=legendPosition);	
	//ylabel("$\mbox{bits/second}$");
	
	// Continuous qDelay
	//currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
	//plotValueAndSample(ContinousQueue.delayedDepartureRate2.qDelay);	
	//legend(["applied queueingDelay", "sampled applied QDelay"], leg=legendPosition);
	//ylabel("$\mbox{second}$");

	drawnow();
endfunction