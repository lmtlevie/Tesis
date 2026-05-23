// timeSerie should have timeSerie.value and timeSerie.t
function [sqDiff] = squareDiff(timeSerie1, timeSerie2, samplingPeriod) // normalized mean square error 
   // set the same finish time
   max_t = ceil(max(max(timeSerie1.t), max(timeSerie2.t)));
   timeSerie1.t($+1)=max_t; timeSerie1.value($+1)=timeSerie1.value($);
   timeSerie2.t($+1)=max_t; timeSerie2.value($+1)=timeSerie2.value($);
   
   // make them same size with regular sampling
   t1 =  sampleTS(timeSerie1, samplingPeriod);
   t2 =  sampleTS(timeSerie2, samplingPeriod);
   
   // calculate sqr diff
   diff = t1.value - t2.value;
   sqDiff = sqrt(mean(diff^2));    
   
   // normalized
   norm = (mean(t1.value) + mean(t2.value)) / 2;
   if(norm==0) then norm=1; end
   sqDiff = sqDiff / norm;

endfunction


function [sqDiff] = sampleTS(timeSerie, samplingPeriod)
 max_t = ceil(max(timeSerie.t));
 
 sqDiff.value=[];
 sqDiff.t=[];
 for t = 0:samplingPeriod:max_t
    allValues = find(timeSerie.t <= t)
    sqDiff.t($+1) = t;
 	sqDiff.value($+1) = timeSerie.value(allValues($));
 	if(length(allValues)==0) then	
 		sqDiff.value($+1)=0;
 	end
 end 

endfunction


function [] = saveSimulationResults(saveDirectory, simu_index)
	dt = getdate();
	backupDirectory = sprintf('%s/%s_%i%.2i%.2i/', saveDirectory, SimulationName, dt(1),dt(2),dt(6));
	logFileName = sprintf('%s_%i_%i%.2i%.2i_%2i%2i%2i', SimulationName, simu_index,  dt(1),dt(2),dt(6), dt(7), dt(8), dt(9));
	mkdir(backupDirectory);
	
	printf("Making a backup of all variables at " + + backupDirectory + logFileName + ".sod   ...\n")
	save(backupDirectory + logFileName + '.sod'); // saving all variables
	unix_g('cp pdevs*.log ' + backupDirectory ); // backup log
	unix_g('cp *.performance.log ' + backupDirectory ); // backup performance	 
endfunction

function [ParameterValues] = completeParameterSpace(parameter_1, parameter_2)
	ParameterValues = [];
	[col, i_rows] = size(parameter_1);
	[col, j_rows] = size(parameter_2);
	for i = 1:i_rows
		for j = 1:j_rows
	       ParameterValues(1,$+1) = parameter_1(i)
	       ParameterValues(2,$) = parameter_2(j)
		end
	end
endfunction

function [ParameterValues] = complete3ParameterSpace(parameter_1, parameter_2, parameter_3)
	ParameterValues = [];
	[col, i_rows] = size(parameter_1);
	[col, j_rows] = size(parameter_2);
	[col, k_rows] = size(parameter_3);
	for i = 1:i_rows
		for j = 1:j_rows
			for k = 1:k_rows
	       		ParameterValues(1,$+1) = parameter_1(i)
	       		ParameterValues(2,$) = parameter_2(j)
	       		ParameterValues(3,$) = parameter_3(k)
			end
		end
	end
endfunction

// sets a sampler for variable. Ej: flow1.size_bits = setSampler(packetRateSamplingPeriod);
function [variable] = setSampler(	samplingPeriod)
    tmp.logLevel = LOG_LEVEL_ALL;
    tmp.logger = LOGGER_SAMPLER;
    tmp.sample_period = samplingPeriod;
    variable = tmp;
endfunction

function [variable] = setDiscreteSampler(samplingPeriod)
    tmp.logLevel = LOG_LEVEL_ALL;
    tmp.logger = LOGGER_DISCRETE_SAMPLER;
    tmp.sample_period = samplingPeriod;
    variable = tmp;
endfunction


// useful Times series functions (values and times of variables)
function [values, times] = sampledValueFromTime(sampledVariable, timeRange)
    [values, times] = getValueFromTime(sampledVariable.t, sampledVariable.value, timeRange)
endfunction

function [values, times] = getValueFromTime(timeVariable, valueVariable, timeRange)
    index = find(timeVariable >= timeRange(1) & timeVariable <= timeRange($))
    values = valueVariable(index)
    times =  timeVariable(index)
endfunction

function [times, values] = sampledTimeOfValue(sampledVariable, valueRange)
    [times, values] = getTimeOfValues(sampledVariable.t, sampledVariable.value, valueRange)
endfunction

function [times, values] = getTimeOfValues(timeVariable, valueVariable, valueRange)
    index = find(valueVariable >= valueRange(1) & valueVariable <= valueRange($))
    times = timeVariable(index)
    values = valueVariable(index)
endfunction


function [] = saveResults(fileName, directoryName)
	dt = getdate();
	
	// default parameters
	[out, inp] = argn(0);  	
  	if inp < 2 then, directoryName = sprintf('ScilabLogs/QT_%i%.2i%.2i/%.2d%.2d%.2d/', dt(1),dt(2),dt(6),dt(7), dt(8), dt(9)), end
  	if inp < 1 then, fileName = sprintf('simuResults_%i%.2d%.2d_%.2d%.2d%.2d', dt(1),dt(2),dt(6), dt(7), dt(8), dt(9)), end
	
	mkdir(directoryName);
	
	printf("making a backup of all variables at " + + directoryName + fileName + ".sod   ...\n")
	unix_g('cp pdevs*.log ' + directoryName ); // backup log
	unix_g('cp *.performance.log ' + directoryName ); // backup performance log
	save(directoryName + fileName + '.sod'); // saving all variables 
endfunction

/////////////////////////////////
/// Plotting//////////////////////
////////////////////////////////

function [] = saveCurrentPlot()
	plotHandle = gcf();
	axes = gca();
	
	dt = getdate();
	saveDirectory = "ScilabLogs";
	backupDirectory = sprintf('%s/%s_%i%.2i%.2i', saveDirectory, SimulationName, dt(1),dt(2),dt(6));
	mkdir(backupDirectory);
	plot_file_name = sprintf('%s/%s_%i%.2i%.2i_%2i%2i%2i', backupDirectory, axes.title.text, dt(1),dt(2),dt(6), dt(7), dt(8), dt(9));
	
	// save as SVG 
	xs2svg(plotHandle, plot_file_name);
endfunction

function [] = plotValueAndSample(sampledTimeSerie, colorRGB, isPlotSample, legendTxt, legendPosition, legendSize, linest) // for color use getColorRGB(mycolor)
	if ~exists("colorRGB","local") then
	    colorRGB = RGB_COLOR_BLUE;
	end
	if ~exists("isPlotSample","local") then
	    isPlotSample = %T;
	end
	if ~exists("legendTxt","local") then
	    legendTxt = "";
	end
	if ~exists("legendPosition","local") then
	    legendPosition = "out_upper_right";
	end
	if ~exists("legendSize","local") then
	    legendSize = 2;
	end
	if ~exists("linest","local") then
	     linest="-";
	end

    valueSampleStyle = "";    
    legendsTxt = [legendTxt];
	if (isPlotSample) then
		plot2d2(sampledTimeSerie.sampledValue.value.t, sampledTimeSerie.sampledValue.value.value, 1); // plot sampled value
		col = color(colorRGB(1)*255,colorRGB(2)*255,colorRGB(3)*255)
		a=gca();poly1= a.children(1).children(1); poly1.foreground=col;
		
		// prepare the plot of values considering the samples were plotted
		valueSampleStyle = "."; 
		linest="none"
		legendsTxt($+1)="";
	end	

	if(length(sampledTimeSerie.value.value.value)>0) then
		plot(sampledTimeSerie.value.value.t, sampledTimeSerie.value.value.value, valueSampleStyle, 'color', colorRGB, 'linest', linest); // plot the events
	end 
	
	appendLegend(legendsTxt, legendPosition, legendSize);
	
	a=gca(); // get the handle of the current axes	
	a.data_bounds(4) = max(a.data_bounds(4), max(sampledTimeSerie.value.value.value)*1.1); //  increase the top limit of the plot (to see the max value)
	a.data_bounds(3) = min(a.data_bounds(3), min(sampledTimeSerie.value.value.value)*0.9); //  increase the top limit of the plot (to see the max value)
endfunction

function [] = plotValue(timeSerie, mycolor)
	if ~exists("mycolor","local") then
	    mycolor = 1;
	end

	if(length(timeSerie.value)>0) then
		plot(timeSerie.t, timeSerie.value, ".", 'color', getColorName(mycolor),'linest', 'none'); // plot the events
	end	
endfunction


function previousLegends = getCurrentLegend()
	a=get("current_axes"); // this will open a new window if there is non :-(
	childrenSize = length(a.children);
	legendChild = [];
	for i = 1:childrenSize
		if(a.children(i).type == "Legend") then
			legendChild = a.children(i);
			break;
		end		       
	end
	
	previousLegends = [];
	if(legendChild ~= []) then
			previousLegends = matrix(legendChild.text,1,-1); // as array (columns instead of rows)
	end
	
endfunction

function [] = appendLegend(legendsToAppend, legendPosition, legendSize)
	if ~exists("legendPosition","local") then
	    legendPosition = "upper_caption";
	end
	if ~exists("legendSize","local") then
	    legendSize = 2;
	end

	previousLegends = getCurrentLegend();
	hl=legend(cat(2, previousLegends, matrix(legendsToAppend, 1, -1)), legendPosition);
	hl.font_size=legendSize;
	
endfunction

// This one works after resizeing a window with subplots. Sometimes the subplots stay small and need to be fixed
function [] = fixMargins(totalPlots)
    drawlater();
    maxRMargin=-999;
	for i = 1:totalPlots  // auto set margins
		subplot(totalPlots,1,i);a=gca();
		a.auto_margins = "off";a.auto_margins = "on";
		maxRMargin = max(maxRMargin,a.margins(2)); 
	end
	for i = 1:totalPlots // make all plots the same width
		subplot(totalPlots,1,i);a=gca();
		a.margins(1) = 0.1; // left margin
		a.margins(2) = maxRMargin; 
	end
	drawnow();
endfunction

function colorRGB = getColorRGB(colorId)
	cmap=get(gcf(),"color_map");
	colorRGB = cmap(colorId,:);
 	 
 	// rainbow
	// cmap = rainbowcolormap (25); 
	//colorRGB = cmap(modulo(colorId*2, 25),:); // first pair numbers to avoid similarity
	
	if colorRGB	== [1,1,1] then
		colorRGB(1)=0;
	end	        
endfunction

function colorName = getColorName(colorId)
	 colorRGB = getColorRGB(colorId)
    colorNames = rgb2name(colorRGB*255);
    colorName = colorNames(1);
endfunction



