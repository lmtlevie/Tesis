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
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "types.h"


//#define MSG_SIZE 

class MsgQueue
{

	private:
		key_t m_Key;
		int m_QId;
	
		//Buffer con el formato de los mensajes utilizados
		struct MSGBUFF
		{
			long mtype;
			//Time time;
			//int port;
			//char data[MSG_DATA_SIZE];
			QUEUEMSG data;
		};
	public:
		/**
		 * Constructor 
		 */ 
		MsgQueue(key_t Key, bool Create=false)
		{
			MsgQueue();
			assign(Key,Create);
		};
		
		MsgQueue()
		{
			//m_MsgBuf.data.data=malloc(MSG_DATA_SIZE);
		};
		~MsgQueue()
		{
			//free(m_MsgBuf.data.data);
		};
		/**
		 * 
		 */
		int assign(key_t Key, bool Create)
		{
			m_Key = Key;
	
			//Obtener el identificador de la cola de mensajes
			if((m_QId = msgget(Key, (Create?IPC_CREAT:0)|0666))==-1){
				return -1;
			}else{
				return 0;
			}
					
		};
		/**
		 * Delete queue
		 */
		int deleteResource()
		{
			if(msgctl(m_QId, 0, IPC_RMID)==-1){
				return -1;
			}else{
				return 0;
			}
				
		};
		/**
		 * Send data to queue
		 */ 
		int send(QUEUEMSG* data, long  type=1)
		{
			MSGBUFF msg;
			//~ if (data->size > MSG_DATA_SIZE) {
				//~ printLog("Queue message size too large\n");
				//~ return -1;
			//~ }
			msg.mtype = type;
			//m_MsgBuf.data=(char*)malloc(size);
			//msg.data=data;
			memcpy(&msg.data,data,sizeof(QUEUEMSG));
			//int sizemsg=size+sizeof(m_MsgBuf)-sizeof(long);
			int sizemsg=sizeof(msg)-sizeof(long);
			int result=msgsnd(m_QId, (MSGBUFF*)&msg, sizemsg, 0);
			if(result ==-1){
				return -1;
			}else{
				return result;
			}
		};
		/**
		 * return a number > 0  if there ara some message, 0 if threre aren't  and a number <0 if a error eas heppened.
		 */
		int recvNoWait(QUEUEMSG *data, long type=0)
		{
			MSGBUFF msg;
			//~ //int sizemsg=size+sizeof(m_MsgBuf)-sizeof(long);
			int sizemsg=sizeof(msg)-sizeof(long);
			int result= msgrcv(m_QId, (QUEUEMSG*)&msg, sizemsg, type, IPC_NOWAIT);
			if(result !=-1&& data!=NULL) {
				//~ NETMessage msg=(NETMessage)m_MsgBuf.data;
				//printLog("recieved: %d data: %s port: %d\n",result,msg.data.data,msg.data.port);
				memcpy(data,&msg.data,sizeof(QUEUEMSG));
			}
			//~ if(result-! && data!=NULL) {
				//~ memcpy(data,&m_MsgBuf.data,size);
			//~ }
			return result;
			//~ }
			//~ 
		}; 
		/**
		 * return a number > 0  if there ara some message, 0 if threre aren't  and a number <0 if a error eas heppened.
		 */
		 //~ int recvNoWait(int *data,long type=0)
		//~ {
			//int sizemsg=sizeof(int)+sizeof(m_MsgBuf)-sizeof(long);
			//~ int sizemsg=sizeof(m_MsgBuf)-sizeof(long);
			//~ int result= msgrcv(m_QId, (struct msgbuf*)&m_MsgBuf, sizemsg, type, IPC_NOWAIT);
			//~ if(result !=-1) {
				//~ NETMessage msg=(NETMessage)m_MsgBuf.data;
				//~ printLog("recieved: %d data: %s port: %d\n",result,msg.data,msg.port);
			//~ }
			//~ if(data!=NULL) memcpy(data,&m_MsgBuf.data,sizeof(int));
			//~ return result;
			//~ }
			
		//~ }; 
		
};
#endif
