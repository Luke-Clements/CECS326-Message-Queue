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
 * Each receiver repeatedly gets a message & displays the value and sender's identity. The
first receiver accepts messages from 251 & 997 senders. This receiver only terminates
after both of its senders had terminated.
Leaves no messages in the queue
 */
int main()
{
	int qid = msgget(ftok("/Desktop/CECS326-Message-Queue-master/",'u'), 0);

	// declare my message buffer
	struct buf {
		long mtype; // required
		char greeting[50]; // mesg content
	};
	buf msg;
	int size = sizeof(msg)-sizeof(long);

	//indicates if the sending programs are still reporting events
	int countOfTerminations = 0;

	do
	{
		msgrcv(qid, (struct msgbuf *)&msg, size, 99, 0);

		cout << getpid() << ", Receiver 100 gets message2: " << msg.greeting << endl;

		if(msg.greeting[0] == '9')
		{
			cout << "here" << endl;
			msg.mtype = 101;
			strcpy(msg.greeting, "Goodbye from Receiver 100");
			msgsnd(qid, (struct msgbuf *) &msg, size, 0);
		}

		//will set the boolean to receive from the 997 sender if the message begins with 'L'
		//	(expected "Last hello from sender 997")
		if(msg.greeting[0] == 'L')
		{
			countOfTerminations ++;
		}
	}while(countOfTerminations < 2);

	//notifies sender 997 of termination
	msg.mtype = 103;
	strcpy(msg.greeting, "Last goodbye from Receiver 100");
	msgsnd (qid, (struct msgbuf *)&msg, size, 0); //exit protocol

	exit(0);
}
