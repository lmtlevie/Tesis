require(rhdf5) # to install: source("http://bioconductor.org/biocLite.R"); biocLite("rhdf5"); library(rhdf5)
require(ggplot2)
require(plyr)
require(knitr)
require(scales)
require(data.table)
require(gridExtra)
require(lattice)
require(latticeExtra)

getSimulationFiles <- function(data.dir) return(paste(data.dir, list.files(data.dir, pattern = "*.\\.sod$"), sep="/"))

Sys.time.minimum <- function() { strptime("1970-01-01 00:00:00.0", "%Y-%m-%d %H:%M:%OS") }

readTimeSerieFromSOD <- function (file, varName, tName){
  value = readSOD(file, varName)
  t = readSOD(file, tName)
  
  df = data.frame(t, value)
  
  return (df)
}

#plot full time series
plotTimeSerie <-function(timeSerie, timeSerieName, subtitle="", save=FALSE){
  title <- sprintf("'%s' VS Time \n %s", timeSerieName, subtitle)
  fileName <- sprintf("%s_%s.png", timeSerieName, subtitle)
  
  plot <- ggplot(timeSerie, aes(x = t, y = value)) + 
    geom_point() + geom_line() + 
    background_grid(major = 'xy', minor = "none") + # add thin horizontal lines 
    panel_border() +  # and a border around each panel
    labs(title=title, x="time (s)", y=timeSerieName)   
  
  if(save){
    ggsave(fileName, width = 14, height = 7)
  }
  print(plot)
}

# readSimulationResults <- function (files) {
#   #pre alocate vectors
#   size= length(files)
#   ro= numeric(size)
#   theta= numeric(size)
#   N= numeric(size)
#   dispatcher.strategy = numeric(size)
#   prob.discard = numeric(size)
#   total.discards = numeric(size) 
#   total.sent = numeric(size) 
#   final.time = numeric(size) 
#   file.name = character(size)
#   mean_proc_time = numeric(size) 
#   c = numeric(size)
#   
#   # read each file
#   for (i in 1:size) {
#     f = files[i]
#     
#     # read values from file
#     file.ro = h5read(f, "/ro")
#     file.theta = h5read(f, "/theta")
#     file.N = h5read(f, "/N")
#     file.dispatcher.strategy = h5read(f, "/dispatcher_strategy")
#     file.discardSampler_count = h5read(f, "/discardSampler_count")
#     file.assignedSampler_count = h5read(f, "/assignedSampler_count")
#     file.assignedSampler_t = h5read(f, "/assignedSampler_t")
#     file.mean_proc_time = h5read(f, "/mean_proc_time")
#     file.c = h5read(f, "/c")
#     
#     
#     # perform calculations
#     ro[i] = file.ro
#     theta[i] = file.theta
#     N[i] = file.N
#     dispatcher.strategy[i] = file.dispatcher.strategy
#     mean_proc_time[i] = file.mean_proc_time
#     file.name[i] = f
#     total.discards[i] = sum(file.discardSampler_count)
#     total.sent[i] = sum(file.assignedSampler_count)
#     final.time[i] = max(file.assignedSampler_t)
#     prob.discard[i] = total.discards[i] / total.sent[i]
#   }
#   
#   # Create the data frame
#   df = data.frame(ro,
#                   theta,
#                   N,
#                   dispatcher.strategy,
#                   prob.discard,
#                   mean_proc_time,
#                   total.discards,
#                   total.sent,
#                   final.time,
#                   file.name
#                   )
#   
#   return(df[with(df, order(ro)),])
# }

# Common functions
# variable.name should use the scilab variable name. For example: Link0.Link.sent_bits.avg
readSOD <- function(file, scilab.variable.name){
  # Scilab treats the "." as an object separator. And stores each object as a HDF5 group
  nameParts = unlist(strsplit(scilab.variable.name, "[.]")) # slit in the different HDF5 group
  
  if(length(nameParts) == 1){ # if it is a variable outside all objects, just read it
    value = h5read(file, scilab.variable.name)  
  } else { # if it is a variable inside an object, we have to find it the way scilab stores objects  
    #varsInFile = h5ls(f)   
    
    varName = sprintf("#%s#", sep=nameParts[1]) # first there is an object, scilab saves it as /#variableName#
    for (i in 2:length(nameParts)) {
      currentVarName = nameParts[i];
      
      #TODO: this prints a very anoying message, but can not be avoided: http://stackoverflow.com/questions/5405291/r-do-not-show-warnings
      suppressWarnings(object <- h5read(file, sprintf("/%s", varName))) # read the previous object attributes //TODO: this is not very performant as it reads all the object values, we only need the names. Maybe try: readAttributes=FALSE
      
      objectVarNames = object[[1]] # first row is the names of its inner variables
      index = match(currentVarName, objectVarNames, nomatch = 0) # get index of current variable 
      
      if(index <= 0){
        stop(sprintf("Can't read '%s'. Variable '%s' does not exist in object '%s'. File: %s", scilab.variable.name, currentVarName, varName, file))
      }
      index = index - 1 # stored 0-based
      
      if(length(nameParts) == i){ # if it the last one, read the value using the index
        indexName = sprintf("#%i#", index)
        value = unlist(object[indexName], use.names = FALSE)
      } else {
        varName = sprintf("#%s_#%i##", varName, index)  ##varName = /##variableName#
      }
    }  
  }
  
  H5close()  
  
  #df = data.frame(value) # create a dataframe  
  return (value)
}


insertRow <- function(existingDF, newrow, r) { #faster than rbind(existingDF[1:r,],newrow,existingDF[-(1:r),])
  existingDF[seq(r+1,nrow(existingDF)+1),] <- existingDF[seq(r,nrow(existingDF)),]
  existingDF[r,] <- newrow
  existingDF
}

readDispatcherValues <- function (files, Values=data.frame()) {
  ret = Values
  for (f in files) {
    #run.generator.t = h5read(f, "/generator_t")
    #run.generator.intergen = h5read(f, "/generator_intergen")
    #run.generator.job.weight = h5read(f, "/generator_jobWeight")
    run.dispatcher.t = h5read(f, "/dispatcher_t")
    run.dispatcher.sent = h5read(f, "/dispatcher_sent")
    run.dispatcher.discarded = h5read(f, "/dispatcher_discarded")
    run.dispatcher.finished = h5read(f, "/dispatcher_finished")
    
    run.df = data.frame(#generator.t = t(run.generator.t),
                        #generator.intergen = t(run.generator.intergen),
                        #generator.job.weight = t(run.generator.job.weight),
                        dispatcher.t = t(run.dispatcher.t),
                        dispatcher.sent = t(run.dispatcher.sent),
                        dispatcher.discarded = t(run.dispatcher.discarded),
                        dispatcher.finished = t(run.dispatcher.finished))
    
    ret = rbind(ret, run.df) 
  }
  # ret = merge(ret, readCommonValues(f))
  return(ret)
}











