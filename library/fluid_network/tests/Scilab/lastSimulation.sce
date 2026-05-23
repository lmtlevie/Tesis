//ExperimentNumber=-1;
printf("QT:Last Simulation Finished. \n")

minQueue = min(queueSize.value)
maxQueue = max(queueSize.value)

// Perform post processing and plotting here

// input rate
//To facilitate plotting add a last value to the variables
inputRate.t(1, $+1) = ExperimenetTracker.finalSimulationTime;
inputRate.value(1, $+1) = inputRate.value(1, $);

if(%T) then
	scf();
	subplot(6,1,1);
	//plot(fluidTCP_AQM.saveWc.t, fluidTCP_AQM.saveWc.value, ".-"); 
	//legend(["TCP_AQM: W(t) size", ]);
	plot(w.t, w.value, ".-"); 
	legend(["New TCP: W(t) size", ]);
	
	subplot(6,1,2);
	plot(inputRate.t, inputRate.value, "."); plot2d2(sampledInputRate.t, sampledInputRate.value, 2);
	legend(["inputRate", "sampledInputRate"]);
	xlabel("$\mbox{t (seconds)}$");
	ylabel("$\mbox{bits/second}$");
	
	subplot(6,1,3);
	plot2d2(sampledQueueSize.t, sampledQueueSize.value, 3); plot(queueSize.t, queueSize.value, "."); plot2d2(queueSize.t, queueSize.value, 2);
	legend(["sampledQueueSize", "queueSize"]);
	
	subplot(6,1,4);
	plot2d2(sampledDepartureRate.t, sampledDepartureRate.value, 3); plot(departureRate.t, departureRate.value, "."); plot2d2(departureRate.t, departureRate.value, 2);
	legend(["sampledDepartureRate", "departureRate"]);
	
	subplot(6,1,5);
	plot2d2(sampledDropsRate.t, sampledDropsRate.value, 3); plot(dropsRate.t, dropsRate.value, "."); plot2d2(dropsRate.t, dropsRate.value, 2);
	legend(["sampledDropsRate", "dropsRate"]);

	subplot(6,1,6);
	plot2d2(sampledQDelay.t, sampledQDelay.value, 3); plot(qDelay.t, qDelay.value, "."); plot2d2(qDelay.t, qDelay.value, 2);
	legend(["sampledQDelay", "queueingDelay"]);

end


// AQM queue
if(%T) then
	scf();
	subplot(5,1,1);
	plot2d2(sampledQueueSize.t, sampledQueueSize.value, 3); plot(queueSize.t, queueSize.value, "."); plot2d2(queueSize.t, queueSize.value, 2);
	legend(["sampledQueueSize", "queueSize"]);
	
	subplot(5,1,2);
	plot2d2(AQMContinousQueue.sampled_w_avg.t, AQMContinousQueue.sampled_w_avg.value, 3); plot(AQMContinousQueue.w_avg.t, AQMContinousQueue.w_avg.value, "."); plot2d2(AQMContinousQueue.w_avg.t, AQMContinousQueue.w_avg.value, 2);
	legend(["w_avg"]);
	
	
	subplot(5,1,3);
	plot2d2(AQMContinousQueue.sampled_RED_dropProb.t, AQMContinousQueue.sampled_RED_dropProb.value, 3); plot(AQMContinousQueue.RED_dropProb.t, AQMContinousQueue.RED_dropProb.value, "."); plot2d2(AQMContinousQueue.RED_dropProb.t, AQMContinousQueue.RED_dropProb.value, 2);
	legend(["RED dropProb"]);
	
	subplot(5,1,4);
	plot2d2(AQMContinousQueue.RED_dropRate.sampled.t, AQMContinousQueue.RED_dropRate.sampled.value, 3); plot(AQMContinousQueue.RED_dropRate.value.t, AQMContinousQueue.RED_dropRate.value.value, "."); plot2d2(AQMContinousQueue.RED_dropRate.value.t, AQMContinousQueue.RED_dropRate.value.value, 2);
	legend(["RED_dropRate"]);
	
	subplot(5,1,5);
	plot2d2(AQMContinousQueue.RED_queueInput.sampled.t, AQMContinousQueue.RED_queueInput.sampled.value, 3); plot(AQMContinousQueue.RED_queueInput.value.t, AQMContinousQueue.RED_queueInput.value.value, "."); plot2d2(AQMContinousQueue.RED_queueInput.value.t, AQMContinousQueue.RED_queueInput.value.value, 2);
	legend(["RED_queueInput"]);
		
	
end

// fluid TCP_AQM
if(%F) then
	scf();
	subplot(2,1,1);
	plot(fluidTCP_AQM.saveWc.t, fluidTCP_AQM.saveWc.value, ".-"); 
	legend(["TCP_AQM W(t) packets", ]);
	
	subplot(2,1,2);
	plot(fluidTCP_AQM.TCP_CONT1.save_delay.t, fluidTCP_AQM.TCP_CONT1.save_delay.value, ".");
	legend(["Delay", ]);
	
	//subplot(3,1,2);
	//plot(fluidTCP_AQM.TCP_CONT1.saveRTT.t, fluidTCP_AQM.TCP_CONT1.saveRTT.value, ".-"); 
	//legend(["TCP_AQM RTT(t)", ]);
	
	//subplot(3,1,3);
	//plot(fluidTCP_AQM.saveQch.t, fluidTCP_AQM.saveQch.value, ".-red"); plot(fluidTCP_AQM.TCP_CONT1.saveXch.t, fluidTCP_AQM.TCP_CONT1.saveXch.value, ".-");
	//legend(["TCP_AQM q(t)", "TCP_AQM x(t) - estimated queue size"]);
end

// new TCP
if(%F) then
	scf();
	subplot(3,1,1);
	plot(dropsRate.t, dropsRate.value, ".");
	legend(["dropsRate(t)"]);
	
	subplot(3,1,2);
	plot(TCP_clientNew.TCP.save_predelay.t, TCP_clientNew.TCP.save_predelay.value, ".");
	legend(["W*RTT*p (t)"]);
	
	subplot(3,1,3);
	plot(TCP_clientNew.TCP.saveDropProb.t, TCP_clientNew.TCP.saveDropProb.value, ".");
	legend(["p(t)"]);
end


