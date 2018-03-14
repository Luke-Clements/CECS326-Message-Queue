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
	int qid = msgget(ftok("/Desktop/CECS326-Message-Queue-master/",'u'), IPC_EXCL|IPC_CREAT|0600);

	// declare my message buffer
	struct buf {
		long mtype; // required long datatype, and required to be first
		char greeting[50]; // mesg content
	};

	buf msg;
	char tempValue[9];

	//holds info about the queue for when this program removes the queue
	//	(used for the number of messages in the queue)
	msqid_ds * queueInfo;

	int size = sizeof(msg)-sizeof(long);

	//indicates whether the sender should continue sending values to the receiver
	bool stillReceiving200 = true;

	//seeds the rand() function
	srand(time(NULL));
	//gets a random unsigned int value [0 - 2^32] to be passed between programs
	int randomUInt;

	//get a random unsigned integer value that is divisible by 997
	do
	{
		randomUInt  = INT_MAX*rand();
	}while(randomUInt%997 != 0 || randomUInt < 100);

	while(randomUInt >= 100)
	{
		cout << "997Active: " << randomUInt << endl;
		sprintf(tempValue, "%d", randomUInt);
		strcpy(msg.greeting, "997 says hello: ");
		strcat(msg.greeting, tempValue);
		//initializing message for receiver 100
		msg.mtype = 99;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0);

		cout << "997 sent successfully" << endl;
		msgrcv(qid, (struct msgbuf *)&msg, size, 101, 0);
		cout << getpid() << ", Sender 997 receiving: " << msg.greeting << endl;

		if(stillReceiving200)
		{
			//initializing message for receiver 200
			msg.mtype = 199;
			sprintf(tempValue, "%d", randomUInt);
			strcpy(msg.greeting, "997 says hello: ");
			strcat(msg.greeting, tempValue);
			msgsnd(qid, (struct msgbuf *)&msg, size, 0);

			//will create some slowing as order is expected as important here between the two receives
			msgrcv(qid, (struct msgbuf *)&msg, size, 201, 0);
			cout << getpid() << ", Sender 997 receiving2: " << msg.greeting << endl;

			if(msg.greeting[0] == 'L')
			{
				cout << "nothere" << endl;
				stillReceiving200 = false;
				//msg.mtype = 201;
				//strcopy(msg.greeting, "Last 251 message received");
				//msgsnd(qid, (struct msgbuf *)&msg, size, 0);
			}
		}

		//get a random unsigned integer value that is divisible by 997
		do
		{
			randomUInt = INT_MAX*rand();
		}while(randomUInt%997 != 0 || randomUInt < 100);
	}
	cout << "997NotActive\n";

	strcpy(msg.greeting, "Last hello from Sender 997");

	//initializing message for receiver 100
	msg.mtype = 997;
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);

	if(stillReceiving200)
	{
		//initializing message for receiver 200
		msg.mtype = 997;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0);
	}

	//waits for last message to be sent by 251 sender
	msgrcv(qid, (struct msgbuf *)&msg, size, 252, 0);

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
