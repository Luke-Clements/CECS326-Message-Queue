/*
 * main.cpp
 *
 *  Created on: Feb 15, 2018
 *      Author: lukecjm
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
using namespace std;

/*
 * Sender does not accept any ack message. The 251 sender only reports its event
to one receiver & terminates on a 'kill' command from a separate terminal.
 */
int main() {
	//qid -> an id that is internal to this program
	int qid = msgget(ftok("/volumes/USBDRIVE/eclipseworkspace/",'u'), 0);

	// declare my message buffer
	struct buf {
		long mtype; // required long datatype, and required to be first
		char greeting[50]; // mesg content
	};

	//holds info about the queue for when this program removes the queue
	//	(used for the number of messages in the queue)
	msqid_ds * queueInfo;

	buf msg;

	/*
	Calculate the size of the message
	OS needs to know how much memory to allocate for the passed message
	*/
	int size = sizeof(msg)-sizeof(long);

	//seeds the rand() function
	srand(time(NULL));
	//gets a random unsigned int value [0 - 2^32] to be passed between programs
	unsigned int randomUInt;

	//initializing message for receiver 100
	msg.mtype = 251;

	do
	{
		//get a random unsigned integer value that is divisible by 5
		do
		{
			randomUInt  = UINT_MAX*rand();
		}while(randomUInt%251 != 0);

		strcpy(msg.greeting, "Hello from Sender 251");
		strcat(msg.greeting, (char *)(randomUInt%251));
		msgsnd(qid, (struct msgbuf *)&msg, size, 0);

	} while(true);

	//sends final message to receiver to the receiver can be terminated
	msg.mtype = 251;
	strcpy(msg.greeting, "Last message sent from sender 251");
	msgsnd (qid, (struct msgbuf *)&msg, size, 0); //exit protocol

	//waits for last message to be sent by 998 sender
	msgrcv(qid, (struct msgbuf *)&msg, size, 998, 0);

	//waits for last message to be sent by 257 sender
	msgrcv(qid, (struct msgbuf *)&msg, size, 258, 0);

	//waits for last message to be sent by 100 receiver
	msgrcv(qid, (struct msgbuf *)&msg, size, 103, 0);

	//waits for last message to be sent by 200 receiver
	msgrcv(qid, (struct msgbuf *)&msg, size, 203, 0);

	msgctl(qid, IPC_STAT, queueInfo);

	cout << queueInfo->msg_qnum << endl;
	//will empty the queue
	while(queueInfo->msg_qnum > 0)
	{
		msgrcv(qid, (struct msgbuf *)&msg, size, 0, 0);
	}

	cout << queueInfo->msg_qnum << endl;

	//removes the queue
	msgctl(qid, IPC_RMID, NULL);

	exit(0);
}
