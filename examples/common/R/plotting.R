require(ggplot2)
require(rhdf5) # to install: source("http://bioconductor.org/biocLite.R"); biocLite("rhdf5"); library(rhdf5)
require(latticeExtra)


plotWithErrorBars <- function (data, x, y, group){
  x = prob.all$ro
  width = (max(x) - min(x)) / (length(x)*5)
  ggplot(prob.all, aes(x = ro, y = proc_time.mean, color = dispatcher.strategy)) + 
    geom_point() + geom_line() + 
    geom_errorbar(aes(ymax=proc_time.mean + proc_time.sd, ymin=proc_time.mean - proc_time.sd), width = width) + # error bars
    #background_grid(major = 'xy', minor = "none") + # add thin horizontal lines 
    #panel_border() +  # and a border around each panel
    labs(title="Mean Processing Time in the critical regime \n N=100; theta=10", x="system load (ro=lambda/(mu * N))", y="Mean processing Time (s)") 
  
}

plotLogXVsYPerGroup <- function(xVsy, groupName, simulation.results, groupValueToString=function(x) toString(x)){
  groupColumn = simulation.results[,c(groupName)]
  groupValues = unique(groupColumn)
  
  names = unlist(lapply(groupValues, function(x) paste(groupName, groupValueToString(x), sep="=")))
  colors = unlist(lapply(groupValues, function(x) x+1))  # x+1 to avoid color=0
  
  legend = list(space="top",
                lines=list(col=colors),
                text=list(names)
  )
  
  plot=NULL
  for (groupValue in groupValues){
    subplot = xyplot(xVsy, data=simulation.results[groupColumn==groupValue,], col=groupValue+1,
                     type=c("p", "l"), grid=TRUE,
                     scales=list(y=list(log=10)), yscale.components=yscale.components.log10ticks,  # plot in log scale
                     key=legend)
    
    if(is.null(plot)){
      plot = subplot
    } else {
      plot = plot + as.layer(subplot)
    }
  }  
  
  return(plot)
}

plotXVsYPerGroup <- function(xVsy, groupName, simulation.results, groupValueToString=function(x) toString(x)){
  groupColumn = simulation.results[,c(groupName)]
  groupValues = unique(groupColumn)
  
  names = unlist(lapply(groupValues, function(x) paste(groupName, groupValueToString(x), sep="=")))
  colors = unlist(lapply(groupValues, function(x) x+1))  # x+1 to avoid color=0
  
  legend = list(space="top",
                lines=list(col=colors),
                text=list(names)
  )
  
  plot=NULL
  for (groupValue in groupValues){
    subplot = xyplot(xVsy, data=simulation.results[groupColumn==groupValue,], col=groupValue+1,
                     type=c("p", "l"), grid=TRUE,                     
                     key=legend)
    
    if(is.null(plot)){
      plot = subplot
    } else {
      plot = plot + as.layer(subplot)
    }
  }  
  
  return(plot)
}



plotLogScaleByN <- function(df, df20, df50){
  legend = list(space="top",
                    lines=list(col=c("red","blue", "green")),
                    text=list(c("N=10","N=20", "N=50"))
                )
  plot = xyplot(prob_discard~ro, data=df, col='red',
                type=c("p", "l"), scales=list(y=list(log=10)), 
                yscale.components=yscale.components.log10ticks, grid=TRUE,
                key=legend)
  plot = plot + as.layer(xyplot(prob_discard~ro, data=df20, col='blue',
                                type=c("p", "l"), scales=list(y=list(log=10)), 
                                yscale.components=yscale.components.log10ticks, grid=TRUE, 
                                key=legend))
  plot = plot + as.layer(xyplot(prob_discard~ro, data=df50, col='green',
                                type=c("p", "l"), scales=list(y=list(log=10)), 
                                yscale.components=yscale.components.log10ticks, grid=TRUE, 
                                key=legend))
  return (plot)
}

plotBoxPlotByN <- function(data.blocking.prob){
  boxplot(prob.discard~N,data=data.blocking.prob, main="Blocking Probability by N",
          xlab="N", ylab="Blocking Probabiliy") 
}
  

theme_dotted_line_plot <- function (plot){
  #plot = plot +  ggtitle("OpenTSDB input method performance comparison")
  #plot = plot +  labs(x="Number of added points", y="Execution Time (ms)")
  plot = plot +  geom_line(size=2) + geom_point(size=6)   
  plot = plot +  theme(plot.title = element_text( face="bold", size = 26))
  plot = plot +  theme(axis.title = element_text(face="bold", size = 14))
  plot = plot +  theme(axis.text = element_text( size = 15))
  plot = plot +  theme(legend.text = element_text(size = 20), legend.position="top", legend.title=element_text(face="bold", size = 20))
  
  return(plot)
}