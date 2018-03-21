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
 * Like the second sender, the last 257 sender only notifies one receiver & terminates when its
receiver stops receiving event notification.
 */

int main() {
	//qid -> an id that is internal to this program
	int qid = msgget(ftok("/Desktop/a/",'u'), 0);

	// declare my message buffer
	struct buf {
		long mtype; // required long datatype, and required to be first
		char greeting[50]; // mesg content
	};

	buf msg;
	int size = sizeof(msg)-sizeof(long);
	char tempValue[9];

	//seeds the rand() function
	srand(time(NULL));
	//gets a random unsigned int value [0 - 2^32] to be passed between programs
	unsigned int randomUInt;

	do
	{
		//get a random unsigned integer value that is divisible by 10
		do
		{
			randomUInt  = UINT_MAX*rand();
		}while(randomUInt%257 != 0);

		cout << "251 sends: " << randomUInt << endl;

		//initializing message for receiver 200
		msg.mtype = 199;
		sprintf(tempValue, "%u", randomUInt);
		strcpy(msg.greeting, "257 says hello: ");
		strcat(msg.greeting, tempValue);
		msgsnd(qid, (struct msgbuf *)&msg, size, 0);

		msgrcv(qid, (struct msgbuf *)&msg, size, 202, 0);
	} while(msg.greeting[0] != 'L');

	//send a message to the closing program (sender 997) that sender 257 has terminated
	msg.mtype = 258;
	strcpy(msg.greeting, "Last message sent by Sender 257");
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);

	exit(0);
}
