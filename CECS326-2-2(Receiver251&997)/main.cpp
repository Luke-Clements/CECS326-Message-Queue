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
	int qid = msgget(ftok("/Desktop/a/",'u'), 0);

	// declare my message buffer
	struct buf {
		long mtype; // required long datatype, and required to be first
		char greeting[50]; // mesg content
	};

	buf msg;
	char tempValue[9];

	/*
	Calculate the size of the message
	OS needs to know how much memory to allocate for the passed message
	*/
	int size = sizeof(msg)-sizeof(long);

	strcpy(msg.greeting, "251 Last message sent");
	msg.mtype = 99;
	get_info(qid, (struct msgbuf *)&msg, size, 99);

	cout << getpid() << endl;

	//seeds the rand() function
	srand(time(NULL));
	//gets a random unsigned int value [0 - 2^32] to be passed between programs
	unsigned int randomUInt;

	do
	{
		//get a random unsigned integer value that is divisible by 251
		do
		{
			randomUInt  = UINT_MAX*rand();
		}while(randomUInt%251 != 0);

		msg.mtype = 99;
		sprintf(tempValue, "%u", randomUInt);
		strcpy(msg.greeting, "251 says hello: ");
		strcat(msg.greeting, tempValue);

		cout << "251(" << getpid() << ")sends: " << tempValue << endl;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0);

		//can't get it to run without receiving an acknowledgement :(
		msgrcv(qid, (struct msgbuf *)&msg, size, 102, 0);

	} while(true);

	exit(0);
}
