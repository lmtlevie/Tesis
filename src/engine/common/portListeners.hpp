/****************************************************************************
**
**  Copyright (C) 2009 Facultad de Ciencia Exactas Ingeniería y Agrimensura
**		       Universidad Nacional de Rosario - Argentina.
**  Contact: PowerDEVS Information (kofman@fceia.unr.edu.ar, fbergero@fceia.unr.edu.ar)
**
**  This file is part of PowerDEVS.
**
**  PowerDEVS is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  PowerDEVS is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with PowerDEVS.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/
//03/2010 added by Guillermina Galache

#ifndef PORTLISTENERS_H
#define PORTLISTENERS_H
#include "simulator.h"

/**
 * 
 */ 
class PortListeners 
{
	struct ListenerTypeMsg{
		char*typeMsg;
		//Simulator**atomics;
		Simulator* atomic;
		
	};
	private:
		static const int INIT_LENG_LIST=10;
		int lenListeners;
		int port;//port number
		//int sockfd;//socket 
		//char* typeMsg;//message type
		//Simulator** atomics;//atomics that listener for any message
		//Simulator* atomics;//atomic with listener for any message
		ListenerTypeMsg *atomicByTypeMsg[INIT_LENG_LIST];
		int qtyListeners;	
	public:
		/**
		 * Constructor
		 */
		PortListeners(int port){
			this->port=port;
			this->qtyListeners=0;
			this->lenListeners=INIT_LENG_LIST;
		}
		~PortListeners(){
			//TODO free memory
			//printLog("porrtListneres destructor\n");
		}
	
		/**
		 *  Add a listener.
		 */ 
		void addListener(char*msgType,Simulator* atomic){
			
			ListenerTypeMsg* listener=(ListenerTypeMsg*)malloc(sizeof(ListenerTypeMsg));
			listener->typeMsg=msgType;
			listener->atomic=atomic;
			if(this->qtyListeners < this->lenListeners){
				this->atomicByTypeMsg[qtyListeners++]=listener;
			}else{//resizing
				//TODO completar
			}
		}
		/**
		 *  Add a listener.
		 */ 
		//~ void addListener(Simulator* atomic){
			//~ this->atomics=atomic;
		//~ }
		/**
		 * Return the port number 
		 */
		int getPort(){
			return this->port;
		}
		/**
		 *  Set de socket descriptor
		 */
		/*void setSocket(int socket){
			this->sockfd=socket;
		}*/
		/**
		 *  Set de socket descriptor
		 */
		/*int getSocket(){
			return this->sockfd;
		}*/
		/**
		 * Return number of atomics that wait for a type message and load its in the parameter. 
		 */
		int getAtomics(Simulator** atomics,char*type){
			//TODO implementar
			return 1;
		}
		/**
		 * Return number of  atomics that wait for any type message and load its in the parameter.
		 */
		//~ int getAtomics(Simulator** atomics){
			//~ atomics=&this->atomics;
			//~ return 1;
		//~ }
		/**
		 * Return number of atomics that wait for a type message. 
		 */
		int getQtyAtomics(char*type){
			//TODO implementar
			return 1;
		}
		/**
		 * Return number of  atomics that wait for any type message.
		 */
		int getQtyAtomics(){
			//TODO implementar
			return 1;
		}
		/**
		 * Alert atomics for data received.
		 */ 
		void alertOfMsg(void*data,Time t,char*typeMsg=NULL){
			//this->atomics->externalinput(t,data);
			printLog("[alertOfMsg] Message: %s \n",data);
			for(int i=0;i<qtyListeners;i++){
				ListenerTypeMsg *listener=this->atomicByTypeMsg[i];
				if(listener->typeMsg==NULL){//si el atomico escucha cualquier tipo de mensaje
					printLog("[alertOfMsg] Escucha todo tipo de mensaje, tiempo: %d s\n",t);
					listener->atomic->externalinput(t,data);//,typeMsg);
				} else {//sino viene el tipo de mensaje o viene y es el que escucha el atomico
					if(typeMsg==NULL || (strcmp(listener->typeMsg,typeMsg)==0)){
						listener->atomic->externalinput(t,data);		
					}
				}
			}
		} 
};

/**
 * 
 */ 
class PortListenersList 
{
	struct SListenerNode{
		PortListeners* listener;
		struct SListenerNode* next;
		struct SListenerNode* previous;
		
	};
	typedef struct SListenerNode ListenerNode;
	private:
		//static const int INIT_QTY_SOCKET=10;
		ListenerNode* firstListener;
		ListenerNode* lastListener;
		int qtyNodes;
		//int sockets[INIT_QTY_SOCKET];
		/**
		 *
		 */ 
		ListenerNode* createNode(int port, Simulator* atomic){
			PortListeners* listener;
			ListenerNode *newNode=(ListenerNode*)malloc(sizeof(ListenerNode));
			newNode->previous=NULL;
			newNode->next=NULL;
			listener=createListener(port,atomic,NULL);
			newNode->listener=listener;
			return newNode;
		}
		/**
		 *
		 */ 
		ListenerNode* createNode(int port, Simulator* atomic,char* typeMsg){
			PortListeners* listener;
			ListenerNode *newNode=(ListenerNode*)malloc(sizeof(ListenerNode));
			newNode->previous=NULL;
			newNode->next=NULL;
			listener=createListener(port,atomic,typeMsg);
			newNode->listener=listener;
			return newNode;
		}
		/**
		 * 
		 */
		PortListeners *createListener(int port, Simulator* atomic,char* typeMsg){
			PortListeners* listener;
			listener=new PortListeners(port);
			listener->addListener(typeMsg,atomic);
			//int sockfd=listener->createSocket();
			return listener;
		} 
		/**
		 * 
		 */
		PortListeners* findByPort(int port){
		 	ListenerNode *node=firstListener;
			//printLog("listener: %d\n",node->listener);
			//printLog("%d\n",node->listener->getPort());
			PortListeners* listener=NULL;
			bool found=false;
			while (node!=NULL && !found){
				//printLog("%d\n",node->listener->getPort());
				if(node->listener->getPort()==port){
					listener=node->listener;
					found=true;
				}
				node=node->next;
			}
			return listener;
		} 
		/**
		 * 
		 */
		 /*PortListeners* findBySocketfd(int socketfd){
		 	ListenerNode *node=firstListener;
			PortListeners* listener=NULL;
			bool found=false;
			while (node!=NULL && !found){
				if(node->listener->getSocket()==socketfd){
					listener=node->listener;
					found=true;
				}
				node=node->next;
			}
			return listener;
		 }*/
	public:
		/**
		 * Constructor
		 */
		PortListenersList(){
			firstListener=NULL;
			lastListener=NULL;
			qtyNodes=0;
		}
		/**
		 * Destructor
		 */ 
		~PortListenersList(){
			//printLog("porrtListneresList destructor\n");
			//Free memory
			if(firstListener!=NULL){
				int i=0;
				ListenerNode *n,*p,*last;
				n=firstListener;
				do{
				   delete(n->listener);
				   last=n;
				   n=n->next;
				  // printLog("%d\n",i++);
				}while(n!=NULL);
				//printLog("fin 1\n");
				int j=0;
				n=last;
				do{
					p=n->previous;
					free(n);
					n=p;
					//printLog("%d\n",j);
				}while(p!=NULL);
			 }
		}
		/**
		 * 
		 */ 
		bool addPortListener(int port,Simulator* atomic){
			return addPortListener(port,atomic,NULL);
		}
		/**
		 * 
		 */ 
		bool addPortListener(int port,Simulator* atomic,char* typeMsg){
			bool existed=false;
			if (firstListener==NULL){
				ListenerNode* newNode= createNode(port,atomic,typeMsg);
				firstListener=newNode;
				lastListener=newNode;
				qtyNodes++;
				//~ printLog("first listener created to port: %d\n",newNode->listener->getPort());
				//printLog("first listener created: %d\n",newNode->listener);
			} else {
				PortListeners* listener;
				listener=findByPort(port);
				if(listener==NULL){
					ListenerNode* newNode= createNode(port,atomic,typeMsg);
					lastListener->next=newNode;
					newNode->previous=lastListener;
					lastListener=newNode;
					qtyNodes++;
					//printLog("listener created\n");
				} else {
					listener->addListener(typeMsg,atomic);
					//printLog("atomic added\n");
					existed=true;
				}
			}
			//printLog("first listener: %d\n",firstListener->listener);
			return existed;
		}
		/**
		 * Find port's listeners, return null if not exist.
		 * port: port number
		 */ 
		PortListeners* findPortListeners(int port){
			//printLog("first listener: %d\n",firstListener->listener);
			return findByPort(port);
		}
		/**
		 * Find listeners asociated to socketfd, return null if not exist.
		 * socketfd: socket filde descriptor
		 */ 
		/*PortListeners* findSockListeners(int socketfd){
			
			return findBySocketfd(socketfd);
		}*/
		/**
		 * Return all socket's file descriptor.
		 * The result structure must be freed.
		 */ 
		/*int* getAllSockets(){
			int* sockets=(int*)malloc(sizeof(int)*qtyNodes);
			int i=0;
			ListenerNode *node=firstListener;
			while (node!=NULL){
				sockets[i]=node->listener->getSocket();
				node=node->next;
				i++;
			}
			return sockets;
		}*/
		/**
		 * Return all socket's file descriptor.
		 * The result structure must be freed.
		 */ 
		/*int getQtySockets(){
			return qtyNodes;
		}*/
		/**
		 * 
		 */
		void alertOfNETMsg( int port,void*data,Time time){
			//printLog("alert of net - port: %d\n",port);
		 	PortListeners* listener=findByPort(port);
			//printLog("listener encontrado\n");
			if (listener!=NULL){
				listener->alertOfMsg(data,time);
			}
		} 
		/**
		 * 
		 */
		void alertOfNETMsg( int port,void*data,Time time,char*typeMsg){
			//printLog("alert of net - port: %d type of msg: %s \n",port,typeMsg);
		 	PortListeners* listener=findByPort(port);
			if (listener!=NULL){
				//printLog("listener encontrado %d\n",listener);
				listener->alertOfMsg(data,time,typeMsg);
			}
		} 
};
#endif
