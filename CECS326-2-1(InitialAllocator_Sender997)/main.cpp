/*
 * main.cpp
 *
 *  Created on: Feb 15, 2018
 *      Author: lukecjm
 */

#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cstdio>
#include <limits.h>
using namespace std;

/*
 * First, the 997 sender sends each event to all receivers and
requires acknowledgement for each message from both receivers before it continues
execution. Also, this sender terminates when it gets/observes a random number smaller
than 100.
 */
int main() {

	//initializes the queue
	int qid = msgget(ftok("/Desktop/a/",'u'), IPC_EXCL|IPC_CREAT|0600);

	// declare my message buffer
	struct buf {
		long mtype; // required long datatype, and required to be first
		char greeting[50]; // mesg content
	};

	buf msg;
	char tempValue[9];

	//holds info about the queue for when this program removes the queue
	//	(used for the number of messages in the queue)
	struct msqid_ds queueInfo;

	int size = sizeof(msg)-sizeof(long);

	//indicates whether the sender should continue sending values to the receiver
	bool stillReceiving200 = true;

	//seeds the rand() function
	srand(time(NULL));
	//gets a random unsigned int value [0 - 2^32] to be passed between programs
	unsigned int randomUInt;

	//get a random unsigned integer value that is divisible by 997
	do
	{
		randomUInt  = UINT_MAX*rand();
	}while(randomUInt%997 != 0);

	while(randomUInt >= 100)
	{
		sprintf(tempValue, "%u", randomUInt);
		strcpy(msg.greeting, "997 says hello: ");
		strcat(msg.greeting, tempValue);
		//initializing message for receiver 100
		msg.mtype = 99;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0);

		msgrcv(qid, (struct msgbuf *)&msg, size, 101, 0);
		cout << "Sender 997 Receives: " << msg.greeting << endl;

		if(stillReceiving200)
		{
			//initializing message for receiver 200
			msg.mtype = 199;
			sprintf(tempValue, "%u", randomUInt);
			strcpy(msg.greeting, "997 says hello: ");
			strcat(msg.greeting, tempValue);
			msgsnd(qid, (struct msgbuf *)&msg, size, 0);

			msgrcv(qid, (struct msgbuf *)&msg, size, 201, 0);
			cout << "Sender 997 Receives: " << msg.greeting << endl;

			if(msg.greeting[0] == 'L')
			{
				stillReceiving200 = false;
			}
		}

		//get a random integer value that is divisible by 997
		do
		{
			randomUInt = UINT_MAX*rand();
		}while(randomUInt%997 != 0);
	}

	strcpy(msg.greeting, "997 Last hello from sender");

	//initializing message for receiver 100
	msg.mtype = 99;
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);

	if(stillReceiving200)
	{
		//initializing message for receiver 200
		msg.mtype = 199;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0);
	}

	//waits for last message to be sent by 257 sender
	msgrcv(qid, (struct msgbuf *)&msg, size, 258, 0);

	//waits for last message to be sent by 100 receiver
	msgrcv(qid, (struct msgbuf *)&msg, size, 103, 0);

	//waits for last message to be sent by 200 receiver
	msgrcv(qid, (struct msgbuf *)&msg, size, 203, 0);

	//sets up to receive info about the length of the queue's leftover messages for removal
	msgctl(qid, IPC_STAT, &queueInfo);

	//will empty the queue
	while(queueInfo.msg_qnum > 0)
	{
		msgrcv(qid, (struct msgbuf *)&msg, size, 0, 0);
		msgctl(qid, IPC_STAT, &queueInfo);
	}

	//removes the queue
	msgctl(qid, IPC_RMID, NULL);
	exit(0);
}
