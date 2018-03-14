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
		if(StillSending997)
		{
			msgrcv(qid, (struct msgbuf *)&msg, size, 997, 0);

			cout << getpid() << ", Receiver 200 gets message: " << msg.greeting << endl;

			//will set the boolean to receive from the 997 sender if the message begins with 'L'
			//	(expected "Last hello from sender 997")
			if(msg.greeting[0] == 'L')
			{
				StillSending997 = false;
			}

			//putting this here instead of above will ensure that no extra ack
			//	messages are sent back to sender 997 after it has terminated
			if(StillSending997)
			{
				msg.mtype = 201;
				strcpy(msg.greeting, "Goodbye from Receiver 200");
				msgsnd (qid, (struct msgbuf *)&msg, size, 0); //exit protocol
			}
			messageCount++;
		}

		msgrcv(qid, (struct msgbuf *)&msg, size, 257, 0);

		cout << getpid() << ", Receiver 200 gets message2: " << msg.greeting << endl;
		msg.mtype = 202;
		strcpy(msg.greeting, "Goodbye from Receiver 200");
		msgsnd (qid, (struct msgbuf *)&msg, size, 0); //exit protocol

		messageCount++;
	}while(messageCount < 5000);

	//removes the excess received message from sender 997 if this receiver terminates before sender 997
	if(StillSending997)
	{
		msgrcv(qid, (struct msgbuf *)&msg, size, 997, 0);
	}

	//termination notification sent to sender 997
	msg.mtype = 203;
	strcpy(msg.greeting, "Last goodbye from Receiver 200");
	msgsnd (qid, (struct msgbuf *)&msg, size, 0); //exit protocol

	//termination notification sent to sender 257
	msg.mtype = 202;
	msgsnd (qid, (struct msgbuf *)&msg, size, 0); //exit protocol

	exit(0);
}
