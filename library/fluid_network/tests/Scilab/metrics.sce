// this file defines the output of the simulation 
// loggers define the variables recorded (generally to scilab)
// debugLevel defines the messages printed in pdevs.log

debugLevel = LOG_LEVEL_ALL; 

source.debugLevel = LOG_LEVEL_NONE;
Router_hybrid.delay.debugLevel = LOG_LEVEL_IMPORTANT;
Router_hybrid.RateSampler.debugLevel = LOG_LEVEL_NONE;
Router_hybrid.packetDiscard.debugLevel = LOG_LEVEL_PRIORITY;
Router_hybrid.Router3.debugLevel = LOG_LEVEL_IMPORTANT;
Router_hybrid.queue_disc.debugLevel = LOG_LEVEL_NONE;
Router_hybrid.intermediateTmp.debugLevel = LOG_LEVEL_NONE;
TCP1.TCPSND2.debugLevel = LOG_LEVEL_PRIORITY;
TCP1.TCPRCV2.debugLevel = LOG_LEVEL_PRIORITY;
TCP1.AppQueue2.debugLevel = LOG_LEVEL_NONE;
Router_discrete.RouterQueue3.debugLevel = LOG_LEVEL_NONE;
Router_discrete.Router3.debugLevel = LOG_LEVEL_IMPORTANT;

