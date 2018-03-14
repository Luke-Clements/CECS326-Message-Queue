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
 * The second receiver gets messages from 257 & 997 senders.
 * The second receiver terminates after it has received a total of 5000
messages. (Also, remember to send the ack to 997 sender.)
Leaves no extra messages in the queue
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

	//indicates if the sender is still sending
	bool StillSending997 = true;
	int messageCount = 0;

	do
	{
		msgrcv(qid, (struct msgbuf *)&msg, size, 199, 0);

		cout << getpid() << ", Receiver 200 gets message: " << msg.greeting << endl;

		if(msg.greeting[0] == '9')
		{
			cout << "herebacon: " << messageCount << endl;
			msg.mtype = 201;
			strcpy(msg.greeting, "Goodbye from Receiver 200");
			msgsnd(qid, (struct msgbuf *) &msg, size, 0);
		}
		else
		{
			cout << "herebacon2: " << messageCount << endl;
			msg.mtype = 202;
			strcpy(msg.greeting, "Goodbye from Receiver 200");
			msgsnd(qid, (struct msgbuf *) &msg, size, 0);
		}

		messageCount++;
	}while(messageCount < 5000);

	strcpy(msg.greeting, "Last goodbye from Receiver 200");

	//termination notification sent to sender 257
	msg.mtype = 202;
	msgsnd (qid, (struct msgbuf *)&msg, size, 0); //exit protocol

	//termination notification sent to sender 997
	msg.mtype = 203;
	msgsnd (qid, (struct msgbuf *)&msg, size, 0); //exit protocol

	exit(0);
}
