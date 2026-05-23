//ExperimentNumber=-1;
printf("QT:Last Simulation Finished. \n")
exec('../examples/common/Scilab/TcpDiscrete.plot.functions.sce', 0)


// Perform post processing and plotting here
title_size = 6;
legendPosition = "upper_caption";legendSize = 2;


// Plot discrete packet delays
if(%T) then
    scf();
//    totalPlots=2;
    totalPlots=1;
    currentPlot=0;

    // title
    f.figure_name= "packet_ECNTcpSnd1Rcv";
    xtitle(f.figure_name);a=get("current_axes"); a.title.font_size=title_size;

    drawlater();

//    // W average per host
//    currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
//    plot(Sender.TcpSession_0.TCP_SND.cwnd.t, Sender.TcpSession_0.TCP_SND.cwnd.value, "red"); appendLegend(["CWND 1"], leg=legendPosition);
//    a=gca(); a.data_bounds(3) = 0;
//    ylabel("packets");

    // queue size
    currentPlot=currentPlot+1;subplot(totalPlots,1,currentPlot);
    plot2d2(Router.egressPort0.queue.qSize_bits.t, Router.egressPort0.queue.qSize_bits.value, 2); appendLegend(["Queue size"], leg=legendPosition);

    ylabel("bits");

    drawnow();
end
