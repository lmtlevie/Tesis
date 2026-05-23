exec('../examples/common/Scilab/functions.sce', 0)

function [w_avg, sendRate] = plotHost(hostName1, nSessions)
	host1 = eval(hostName1);		
	
	f = scf();
	f.figure_name = hostName1;
	xtitle(f.figure_name);a=get("current_axes"); a.title.font_size=title_size;	
	drawlater();
 	totalPlots=1;
	currentPlot=0;
	
	// TCP windows W
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
	for i = 0:(nSessions-1)
		sessionName = hostName1 + ".TCP_session_" + string(floor(i/10)) + "._" + string(modulo(i, 10))
		//printf("sessionName=%s \n", sessionName );
		session = eval(sessionName);
		
		if (length(session.TCP_SND.cwnd.t)~=0) then
			plot(session.TCP_SND.cwnd.t, session.TCP_SND.cwnd.cwnd, ".-", 'color', getColorRGB(i+1)); appendLegend([sessionName + "TCP Window"], leg=legendPosition);
		end
	end
	xlabel("$\mbox{t (seconds)}$");
	ylabel("$\mbox{packets}$");
	drawnow();
	
	// plot average in red
	w_avg = calculateAvgW(hostName1, 0.1);
	plot(w_avg.t, w_avg.avg, ".red"); appendLegend([hostName1 + " Averaged Window"], leg=legendPosition);	
 
	drawnow();
	
	sendRate.value = host1.sentPacket.size_bits.sum ./ host1.sentPacket.size_bits.sample_period;
	sendRate.t = host1.sentPacket.size_bits.t
endfunction	

function [] = plotQueue(queueName)
	title_size = 6;
	queue = eval(queueName);
	
	
	///////// Only queue Size in pkts
	f = scf();
	f.figure_name= queueName;
	xtitle(queueName);a=get("current_axes"); a.title.font_size=title_size;	
	
	plot2d2(queue.queue.qSize_elems.t, queue.queue.qSize_elems.qSize_elems, 2); appendLegend([queueName + " size"], leg=legendPosition);
	plot(queue.RED_AQM.queueAvg.t, queue.RED_AQM.queueAvg.avg / 4000, ".red"); appendLegend([queueName + " AVG queueSize"], leg=legendPosition);	
	a=gca(); // get the handle of the current axes
	a.data_bounds(1)=0; // plot since start of simulation (even if there were no packets)
	a.data_bounds(2) = ExperimenetTracker.finalSimulationTime; // plot until end of simulation (even if queue does not change)
	a.data_bounds(4) = max(a.data_bounds(4), max(queue.queue.qSize_elems.qSize_elems)*1.1); //  increase the top limit of the plot (to see the max value)
	ylabel("$\mbox{packets}$");
	
	
	f = scf();
	
	// title
	f.figure_name= queueName;
	xtitle(queueName);a=get("current_axes"); a.title.font_size=title_size;	
	
	drawlater();
 	totalPlots=2;
	currentPlot=0;
	
	// queue size	
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
	plot2d2(queue.queue.qSize_bits.t, queue.queue.qSize_bits.avg, 2); appendLegend([queueName + " size"], leg=legendPosition);
	plot(queue.RED_AQM.queueAvg.t, queue.RED_AQM.queueAvg.avg, ".red"); appendLegend([queueName + " AVG queueSize"], leg=legendPosition);	
	a=gca(); // get the handle of the current axes
	a.data_bounds(1)=0; // plot since start of simulation (even if there were no packets)
	a.data_bounds(2) = ExperimenetTracker.finalSimulationTime; // plot until end of simulation (even if queue does not change)
	a.data_bounds(4) = max(a.data_bounds(4), max(queue.queue.qSize_bits.avg)*1.1); //  increase the top limit of the plot (to see the max value)
	ylabel("$\mbox{bits}$");
		
	// wait time
	//currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
	//plot(queue.waittime.t, queue.waittime.waittime,".");
	//a=gca(); // get the handle of the current axes
	//a.data_bounds(2) = ExperimenetTracker.finalSimulationTime; // plot until end of simulation (even if queue does not change)
	//ylabel("$\mbox{seconds}$");
	//hl=legend(["Packet waitTime (suffered qDelay)"], leg=legendPosition);hl.font_size=legendSize;	
	
	// discrete output packet rate per flow
	//currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);	
	//plot(queue.outFlow1.size_bits.t, queue.outFlow1.size_bits.sum ./ queue.outFlow1.size_bits.sample_period, ".-red"); appendLegend([queueName + " outputRate1"], leg=legendPosition);
	//plot(queue.outFlow2.size_bits.t, queue.outFlow2.size_bits.sum ./ queue.outFlow2.size_bits.sample_period, ".-blue"); appendLegend([queueName + " outputRate2"], leg=legendPosition);
	//plot(queue.outFlow3.size_bits.t, queue.outFlow3.size_bits.sum ./ queue.outFlow3.size_bits.sample_period, ".-magenta"); appendLegend([queueName + " outputRate3"], leg=legendPosition);
 	//ylabel("$\mbox{bits/s}$");
	
	
	// discrete packet discard rate	
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);	
	plot(queue.queue.discard_bits.t, queue.queue.discard_bits.sum ./ packetRateSamplingPeriod, ".-blue"); appendLegend([queueName + " tail-drop rate"], leg=legendPosition);
	plot(queue.RED_AQM.drop_bits.t, queue.RED_AQM.drop_bits.sum ./ packetRateSamplingPeriod, ".red"); appendLegend([queueName + " RED drops rate"], leg=legendPosition);
	
 	ylabel("$\mbox{bits/s}$");	


	drawnow();
endfunction

function [] = plotHosts(hostsList)
// separated window
	f = scf();
	f.figure_name = "All hosts"	;
	xtitle(f.figure_name);a=get("current_axes"); a.title.font_size=title_size;
	totalPlots=2;
	currentPlot=0;
	
	// plot average in red
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);		
	[tmp, len] = size(hosts);
	for i = 1:len
		host = hostsList(i);
		plot(host.w_avg.t, host.w_avg.avg, ".-", 'color', getColorRGB(i)); appendLegend([" Averaged Window " + string(i)], leg=legendPosition);
 	end	
 	ylabel("$\mbox{packets}$");
	
	// output packet rate		
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
	[tmp, len] = size(hosts);
	for i = 1:len
		host = hostsList(i);
		plot(host.sendRate.t, host.sendRate.value , ".-", 'color', getColorRGB(i)); appendLegend(["sendRate " + string(i)], leg=legendPosition);	 	
	end
	ylabel("$\mbox{bits/s}$");			
		
	drawnow();
endfunction

function [w_avg] = calculateAvgW(hostName1, samplingPeriod) // calculate W average
	// get final simulation time (the shity way)
	max_t=0;
	for i = 0:100
		instruction = sprintf('session = %s', hostName1 + ".TCP_session_" + string(floor(i/10)) + "._" + string(modulo(i, 10)))			
		ierr = execstr(instruction, 'errcatch')
		if ierr ~= 0 then
		  // printf("Does not exist: %s \n", instruction );
		   break;
		end
			
		// read window	
		if length(session.TCP_SND.cwnd.t) > 0 then
			max_t = max(max_t, session.TCP_SND.cwnd.t($));
		end
	end
	
	w_avg.t=[];
	w_avg.avg=[];		
	for time = samplingPeriod:samplingPeriod:max_t 
		//printf("\nTime=%g -------------\n", time );
		
		// get the active TCP sessions
		maxPort = eval(hostName1 + ".TcpPortFilter.maxPort");
		clear allValues;
		allValues = getValueFromTime(maxPort.t, maxPort.maxPort, [0:time]);
		activeTcp = allValues($); // take last value
		//printf("\n at Time=%g there were %d active sessions-------------\n", time, activeTcp );
			
		accumSum = 0;
		for i = 0:activeTcp-1
			instruction = sprintf('session = %s', hostName1 + ".TCP_session_" + string(floor(i/10)) + "._" + string(modulo(i, 10)))			
			ierr = execstr(instruction, 'errcatch')
			if ierr ~= 0 then
			   //printf("Does not exist: %s \n", instruction );
			   break;
			end
				
			// read window	
			cwnd = session.TCP_SND.cwnd;
			cwnd_values = getValueFromTime(cwnd.t, cwnd.cwnd, [0:time:time]);
			
			accumSum = accumSum + cwnd_values($); // take last value
			
			//printf("%s_cwnd=%g;  accumSum=%g \n", instruction, cwnd_values($), accumSum );
		end
		
		// avoid division by zero (set to 1 anyway sum will be 0)
		if activeTcp==0 then
			activeTcp=1;
		end
		
		w_avg.avg($+1) = accumSum / activeTcp;
		w_avg.t($+1)=time;
		
		
		//printf("t=%g avg=%g ; accumSum=%g activeTcp=%d \n", w_avg.t($), w_avg.avg($), accumSum, activeTcp );
	end
	
	w_avg.value = w_avg.avg; // redundant, but compatible with time series
endfunction

function [] = plotTCP(tcpName1, tcpName2, tcpName3)
	tcp1 = eval(tcpName1);	
	tcp2 = eval(tcpName2);
	tcp3 = eval(tcpName3);
	f = scf();
	
	// title
	f.figure_name = "TCP";
	xtitle(f.figure_name);a=get("current_axes"); a.title.font_size=title_size;	
	
	drawlater();
 	totalPlots=3;
	currentPlot=0;
		
	// TCP window W
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
	strLegends=[];
	if (length(tcp1.TCP_SND.cwnd.t)~=0) then
		plot(tcp1.TCP_SND.cwnd.t, tcp1.TCP_SND.cwnd.cwnd,".-red");	strLegends($+1)=tcpName1 + " Window";
	end
	if (length(tcp2.TCP_SND.cwnd.t)~=0) then
		plot(tcp2.TCP_SND.cwnd.t, tcp2.TCP_SND.cwnd.cwnd,".-blue");strLegends($+1)= tcpName2 + " Window";
	end
	if (length(tcp3.TCP_SND.cwnd.t)~=0) then
		plot(tcp3.TCP_SND.cwnd.t, tcp3.TCP_SND.cwnd.cwnd,".-magenta");strLegends($+1)= tcpName3 + " Window";
	end
	xlabel("$\mbox{t (seconds)}$");
	ylabel("$\mbox{packets}$"); 
	hl=legend(strLegends, leg=legendPosition); hl.font_size=legendSize;
	
	// discrete input packet rate
	inputR1 = tcp1.sentPacket.size_bits.sum / tcp1.sentPacket.size_bits.sample_period;
	inputR2 = tcp2.sentPacket.size_bits.sum / tcp2.sentPacket.size_bits.sample_period;
	inputR3 = tcp3.sentPacket.size_bits.sum / tcp3.sentPacket.size_bits.sample_period;
	currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);	
	plot(tcp1.sentPacket.size_bits.t, inputR1, ".-red"); appendLegend([tcpName1 + " sendRate"], leg=legendPosition);
	plot(tcp2.sentPacket.size_bits.t, inputR2, ".-blue"); appendLegend([tcpName2 + "sendRate"], leg=legendPosition);
	plot(tcp3.sentPacket.size_bits.t, inputR3, ".-magenta"); appendLegend([tcpName3 +  "sendRate"], leg=legendPosition);
 	ylabel("$\mbox{bits/s}$");	
 	
 	// packet latency
	if(%F) then // only for TCP
		currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
		latency1 = Receiver.tcp_latency.tcp_latency; // by default use TCP latency
		latency1_t = Receiver.tcp_latency.t; // by default use TCP latency
		latency2 = Receiver2.tcp_latency.tcp_latency; // by default use TCP latency
		latency2_t = Receiver2.tcp_latency.t; // by default use TCP latency
		if (length(latency1)==0) then
			latency1 = Receiver.latency.latency; // if no TCP use overall latency
			latency1_t = Receiver.latency.t;
			latency2 = Receiver2.latency.latency; // if no TCP use overall latency
			latency2_t = Receiver2.latency.t;
		end
		strLegends=[];
		if (length(latency1)~=0) then
			plot(latency1_t, latency1, ".-red", leg="in_upper_left"); strLegends($+1)="Trip Time 1(latency @ receiver)";
		end
		if (length(latency2)~=0) then
			plot(latency2_t, latency2, ".-blue", leg="in_upper_left"); strLegends($+1)="Trip Time 2(latency @ receiver)";
		end
		hl=legend(strLegends, leg=legendPosition);hl.font_size=legendSize;
		ylabel("$\mbox{seconds}$");
	end	

	drawnow();
endfunction