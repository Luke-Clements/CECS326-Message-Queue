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
#include "get_info.h"
#include <limits.h>
using namespace std;

/*
 * Sender does not accept any ack message. The 251 sender only reports its event
to one receiver & terminates on a 'kill' command from a separate terminal.
 */
int main() {
	//qid -> an id that is internal to this program
	int qid = msgget(ftok("/Desktop/CECS326-Message-Queue-master/",'u'), 0);

	static char initialMessage[] = "Hello from Sender 251: ";
	char* tempMessage;

	// declare my message buffer
	struct buf {
		long mtype; // required long datatype, and required to be first
		char greeting[50]; // mesg content
	};

	//holds info about the queue for when this program removes the queue
	//	(used for the number of messages in the queue)
	msqid_ds * queueInfo = new msqid_ds;

	buf msg;
	char tempValue[9];

	strcpy(msg.greeting, "Last message sent from sender 251");

	/*
	Calculate the size of the message
	OS needs to know how much memory to allocate for the passed message
	*/
	int size = sizeof(msg)-sizeof(long);

	get_info(qid, (struct msgbuf *)&msg, size, 251);

	cout << getpid() << endl;

	//seeds the rand() function
	srand(time(NULL));
	//gets a random unsigned int value [0 - 2^32] to be passed between programs
	int randomUInt;

	//initializing message for receiver 100
	msg.mtype = 99;

	do
	{
		//get a random unsigned integer value that is divisible by 5
		do
		{
			randomUInt  = INT_MAX*rand();
		}while(randomUInt%251 != 0);

		sprintf(tempValue, "%d", randomUInt);
		strcpy(msg.greeting, "Hello from Sender 251: ");
		strcat(msg.greeting, tempValue);

		cout << getpid() << ": message sent 251" << endl;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0);

	} while(true);
	cout << "251NotActive\n";

	//send a message to the closing program (sender 997) that sender 251 has terminated
	msg.mtype = 252;
	strcpy(msg.greeting, "Last message sent by Sender 251");
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);

	exit(0);
}
