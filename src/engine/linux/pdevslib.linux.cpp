#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "types.h"
#include <sys/time.h>
#include <math.h>
#include <sys/io.h>
#include <root_simulator.h>

//---------------------- agregado 24-01-2018 -----------------------
#include <pthread.h>
#include <signal.h>
//-------------------------- fin agregado --------------------------

//03/2010: file updated by Guillermina Galache
#define FILEQ "."
#define CHARQ 'q'
#define KEYQUEUE(id) ftok(FILEQ,id);
#define SIZEBUFFER 512
#define SIZETYPEMSG 128
#define SEPARATOR '='

static PortListenersList *listeners = NULL;
static MsgQueue* netQueue = NULL;
static int qtyThreads = 0;
static pthread_t threads[10];//TODO hacerlo dinamico
static int threadSocket[10];//TODO hacerlo dinamico
static volatile bool endNet = false;
static bool isNetInitialized = false;

// This structure is used to notify that a un network request has happened
struct NETMessage{
	int port;//by which port has been receive the request
	Time time;
	char data[SIZEBUFFER];//message received
};

void* net_handler(void*args);
void cleanNet();
void alertOfNET(NETMessage msg);
//---------------------------------------------------------------------- 

OS getOs() 
{
	return LINUX;
}

double getTime()
{
	return 1.0*clock()/CLOCKS_PER_SEC;
}


double getRealSimulationTime()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return (tv.tv_sec + tv.tv_usec*1.0e-6)  - realTiSimulation;
}

int waitFor(Time t, RealTimeMode m) 
{
	//---------------------- agregado 24-01-2018 -----------------------
	NETMessage msg;
	QUEUEMSG queueMsg;
	//----------------------------------------------------------------------
	double ti=getRealSimulationTime();
	if (t<=0) 
		return 0;
	while ((getRealSimulationTime()-ti)<t) {
	//---------------------- agregado 24-01-2018 -----------------------
                if (isNetInitialized) {
			int size;
			int recvresult = netQueue->recvNoWait(&queueMsg);
			if (recvresult != -1) {
				msg.port = queueMsg.port;
				memcpy(&msg.data,&queueMsg.data,sizeof(queueMsg.data));				
				msg.time = getRealSimulationTime();//getTime();
				alertOfNET(msg); 
			}
                        // printLog("[waitFor] msg.port: %i, msg.data: %s, msg.time: %d\n",msg.port,msg.data,msg.time); //EP
		}
                // if (isNetInitialized == 0) => solo se queda en el while() perdiendo tiempo
	//----------------------------------------------------------------------
	}
	return 0;
}

void initLib()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	realTiSimulation = tv.tv_sec + tv.tv_usec*1.0e-6;
}

void cleanLib()
{
	//---------------------- agregado 24-01-2018 -----------------------
	if(isNetInitialized){
		cleanNet();
	}
	//----------------------------------------------------------------------
}

void parseCommandLine(char *program, char* cmdLineTxt, char*** argv, int* argc){
	int count = 1;

	char *cmdLineCopy = strdupa(cmdLineTxt);
	char* match = strtok(cmdLineCopy, " ");
	// First, count the number of arguments
	while(match != NULL){
		count++;
		match = strtok(NULL, " ");
	}

	*argv = (char**) malloc(sizeof(char*) * (count+1));
	(*argv)[count] = 0;
	**argv = strdup(program); // The program name would normally go in here

	if (count > 1){
		int i=1;
		cmdLineCopy = strdupa(cmdLineTxt);
		match = strtok(cmdLineCopy, " ");
		do{
			(*argv)[i++] = strdup(match);
			match = strtok(NULL, " ");
		} while(match != NULL);
	}
	*argc = count;
}

void spawnProcess(const char *path, char *arg) {
	char **argv;int argc;
	parseCommandLine((char*)path,arg,&argv,&argc);
	if (fork()==0) { // Child process
		/*
		printLog("Running program %s with args:", path);
		for (int i=0;i<argc;i++) 
		printLog("%d=%s, ",i,argv[i]); 
		printLog("\n");
		*/
		execv(path,argv);
		exitStatus = -1;
		printLog("ERROR: %s not found\n",path);
		abort();
	}
}

void writeToPort(short v,int port) {
	if (ioperm(port,1,1)) {
		printLog("Couldnt get the permissions for port %x\n", port);
		exit(-1);
	}
	outb_p(v,port);
}

short readFromPort(int port) {
	if (ioperm(port,1,1)) {
		printLog("Couldnt get the permissions for port %x\n", port);
		exit(-1);
	}
	return inb_p(port);
}

long int PDFileOpen(const char* name, char mode) {
	char strMode[2];
	strMode[0]=mode;
	strMode[1]='\0';
	return (long int)fopen(name,strMode);
}

long int PDFileWrite(long int file ,char* buf,int size) {
	long int r=fwrite(buf,size,1,(FILE*)file);	
	fflush((FILE*)file);
	return r;
}

long int PDFileRead(long int file ,char* buf ,int size){
	int r;
	if ((r=fread(buf,size,1,(FILE*)file))<size)
		printLog("Incomplete read from file\n");
	return r;
}
void PDFileClose(long int file){
	fclose((FILE*)file);
};

void printLog(const char *fmt,...) {
#ifdef NOPRINTPDEVSLOG
    return;
#endif
	va_list va;
	va_start(va,fmt);
	vprintLog(fmt, va);
	va_end(va);
}

void vprintLog(const char *fmt, va_list va) {
	static int init=0;
	char fileName[1024];
	sprintf(fileName,"%s/pdevs.log",getExecutablePath().c_str());

	if (!init) {
		init=1;
		fclose(fopen(fileName,"w"));
	}

	FILE *fd=fopen(fileName,"a");
	vfprintf(fd,fmt,va);
	fclose(fd);
}

void RequestIRQ(unsigned irq, void  *a){

}

//---------------------- agregado 24-01-2018 -----------------------

// #include "pdevslib.common.cpp"

/** 
 * Network Connection Management 
 * 03/2010: addedd by Guillermina Galache 
 **/

/**
 * 
 */
void initNet() {
	printLog("[initNet] Initializing the network's resources\n");
	listeners = new PortListenersList();
	key_t key = ftok(FILEQ,CHARQ);
	netQueue = new MsgQueue(key,true);
	printLog("[initNet] Queue created with key: %d\n",key);
	isNetInitialized = true;
}

/*
 *
 */
//TODO ver de donde se puede llamar 
void cleanNet() {
	printLog("[cleanNet] Cleaning net resources\n");
	//close all socket and delete the ipc resources
	endNet = true;
	if(netQueue != NULL){
		netQueue->deleteResource();
	}
	if(listeners != NULL){
		delete (listeners);
		listeners = NULL;
	}
	isNetInitialized = false;
	endNet = true;
	for(int i = 0; i < qtyThreads; i++){
		if (close(threadSocket[i])==0)
			printLog("[cleanNet] socket succesfully closed\n");
		// Comento la linea que sigue porque salta segmentation fault
		printLog("[cleanNet] pthread_killed()=%i\n",pthread_kill(threads[i],SIGINT));
		//~ threads[i]=NULL;
	}
	// Comento las lineas que siguen porque salta segmentation fault
			for(int i = 0; i < qtyThreads; i++){
				pthread_join(threads[i],NULL);
				threads[i] = NULL;
			}
	qtyThreads = 0;
}

/*
 *
 */
static void endnet (int dummy) {
	printLog("[endnet] Ending net resources\n");
	//~ _exit(0);
	pthread_exit(NULL); // parece que aca tira segmentation fault
	printLog("[endnet] pthread_exit(NULL) = %i, \n");
	// return NULL;
}

/*
 *
 */
void* net_handler(void *args) {
	//~ NETMessage msg;
	printLog("[net_handler] Enter to net_handler \n");
        int port = *((int *)args);
        free(args); // libero la memoria
        printLog("[net_handler] Port: %i \n",port);
	signal(SIGHUP, endnet);
        signal(SIGINT, endnet);
        signal(SIGKILL, endnet);
        signal(SIGTERM, endnet);
	char buffer[SIZEBUFFER];
	//create socket
	struct sockaddr_in local_addr;
	memset(buffer, '\0', SIZEBUFFER);
	local_addr.sin_family = AF_INET;
        local_addr.sin_addr.s_addr = INADDR_ANY;
        local_addr.sin_port = htons(port);
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0); // socket UDP
   	threadSocket[qtyThreads++] = sockfd;

	if (sockfd < 0) {
	    printLog("[net_handler] Error opening socket: %d\n", sockfd);
            _exit(1);
        }
	
	bind(sockfd, (struct sockaddr *) &local_addr,sizeof(struct sockaddr_in));

	while (true) {
                memset(buffer,'\0',SIZEBUFFER); // EP: limpio el buffer
		//wait for request
		printLog("[net_handler] Waiting for data\n");
		int ret;
		ret = recvfrom(sockfd,&buffer,SIZEBUFFER,0,NULL,0); // funcion bloqueante
		printLog("[net_handler] Data received\n"); // si llego aca es porque recibio data
		//~ if (endNet){ 
			//~ printLog("end net\n");
			//~ break;
		//~ }
                if (ret > 0) {
			QUEUEMSG msgqueue;
                        msgqueue.port = port;
			memcpy(msgqueue.data,buffer,ret);
			msgqueue.data[ret] = '\0';
			netQueue->send(&msgqueue);
                        printLog("[net_handler] Received: %s on port: %i and send this to queue\n",buffer,port);
                } else if (ret == -1) {//EP
                        printLog("[net_handler] Error: %s\n", strerror(errno));
                } else {//EP
                        printLog("[net_handler] No message available\n");
                }
        }
}

/**
 *
 */
void alertOfNET(NETMessage msg){
	Time time = getRealSimulationTime(); // getTime() - realTi;
	if (strchr(msg.data,SEPARATOR) == NULL) {
		listeners->alertOfNETMsg(msg.port,msg.data,time);
	} else {
		char typeMsg[SIZETYPEMSG];
		char buffer[SIZEBUFFER];
		int i = 0;
		int j = 0;
		char* pdata = &msg.data[0];
		while(*pdata != SEPARATOR && i<SIZETYPEMSG-1){
			typeMsg[j++] = *pdata;
			i++;
			pdata++;
		}
		typeMsg[j] = '\0';
		pdata++;
		//memcpy(buffer,pdata,strlen(msg.data)-strlen(typeMsg)-1);
		strcpy(buffer,pdata);
		//printLog("tipo de mensaje: %s dato: %s\n",typeMsg,buffer);
		listeners->alertOfNETMsg(msg.port,buffer,time,typeMsg);
	}
}

/**
 * This function is used by atomics to register as port listener for a type message
 */ 
void RequestNET(int port, Simulator *atomic, char* msgType){
	bool existed;
        int* taskIdPtr = (int *) malloc(sizeof(int)); // EP: 14/03/2018
        *taskIdPtr = port; // La variable port tiene alcance local y una vez que 
                           // termina la ejecución de RequestNET() se pierde.
                           // Si al thread le paso un puntero a port entonces
                           // ese puntero apunta a la posición de memoria de port
                           // que se pierde ni bien se sale de RequestNET(). Por
                           // eso en la función net_handler() leía 0 en lugar del port 

	if (!isNetInitialized) {
		initNet();
	} else {
                printLog("[RequestNET] Recursos de red ya inicializados\n");
        }
	if (msgType == NULL) {
		printLog("[RequestNET] Port: %d \n",port);
		existed = listeners->addPortListener(port,atomic);
		//~ PortListeners *list=listeners->findPortListeners(port);
		//~ listeners->findPortListeners(port);
	} else {
		printLog("[RequestNET] Port: %d - TypeMsg: %s \n",port,msgType);
		existed = listeners->addPortListener(port,atomic,msgType);
	}
	//create thread to listener in the port
	//printLog("new thread will create\n");
	if (!existed) {//si ya no existia un listener para el port
    	        pthread_create(&threads[qtyThreads], NULL, net_handler, (void *) taskIdPtr);
	}
	printLog("[RequestNET] New thread created\n");
	printLog("[RequestNET] Listeners: %d\n",listeners);
}

/**
 * This function is used by atomics to register as port listener for any type message
 */
void RequestNET (int port, Simulator* atomic){
	RequestNET(port, atomic, NULL);
}

/**
 * This function is used by atomics to send data by net.
 */
void sendNET(int port, char* ip, char* data, int size){
	printLog("[sendNET] Sent data:  %s  to ip: %s , port: %d \n",data,ip,port);
	struct sockaddr_in remote_addr;
	struct sockaddr_in local_addr;
	local_addr.sin_family      = AF_INET;
        local_addr.sin_addr.s_addr = INADDR_ANY;
        local_addr.sin_port        = htons(port+1);
	remote_addr.sin_family = AF_INET;
        inet_aton(ip, &remote_addr.sin_addr);
        remote_addr.sin_port = htons(port);
        /* Create new socket. */
        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bind(sockfd, (struct sockaddr *) &local_addr,sizeof(struct sockaddr_in));
	int result = sendto(sockfd,data,size,0,(struct sockaddr*)&remote_addr,sizeof(remote_addr));
	if (result == -1) {
		printLog("[sendNET] Error sending data to ip: %s , port: %d", ip, port);
	}
	close(sockfd); // EP: agrego esta linea para cerrar el puerto despues de cada envio.
}

//------------------------------------------------------------------
