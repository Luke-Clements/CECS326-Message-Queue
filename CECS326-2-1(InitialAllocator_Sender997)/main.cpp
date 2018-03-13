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
 * First, the 997 sender sends each event to all receivers and
requires acknowledgement for each message from both receivers before it continues
execution. Also, this sender terminates when it gets/observes a random number smaller
than 100.
 */
int main() {

	//initializes the queue
	int qid = msgget(ftok("/volumes/USBDRIVE/eclipseworkspace/",'u'), IPC_EXCL|IPC_CREAT|0600);

	// declare my message buffer
	struct buf {
		long mtype; // required long datatype, and required to be first
		char greeting[50]; // mesg content
	};

	buf msg;

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
	}while(randomUInt%997 != 0 || randomUInt < 100);

	while(randomUInt >= 100)
	{
		strcpy(msg.greeting, "Hello from Sender 997: ");
		strcat(msg.greeting, (char *)(randomUInt%997));
		//initializing message for receiver 100
		msg.mtype = 997;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0);

		msgrcv(qid, (struct msgbuf *)&msg, size, 101, 0);

		if(stillReceiving200)
		{
			//initializing message for receiver 200
			msg.mtype = 997;
			strcpy(msg.greeting, "Hello from Sender 997: ");
			strcat(msg.greeting, (char *)(randomUInt%997));
			msgsnd(qid, (struct msgbuf *)&msg, size, 0);

			//will create some slowing as order is expected as important here between the two receives
			msgrcv(qid, (struct msgbuf *)&msg, size, 201, 0);

			if(msg.greeting[0] == 'L')
			{
				stillReceiving200 = false;
			}
		}

		//get a random unsigned integer value that is divisible by 997
		do
		{
			randomUInt  = UINT_MAX*rand();
		}while(randomUInt%997 != 0 || randomUInt < 100);
	}

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

	//sending termination message to closer program (sender 251)
	msg.mtype = 998;
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);
	exit(0);
}
