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
	int qid = msgget(ftok("/volumes/USBDRIVE/eclipseworkspace/",'u'), 0);

	// declare my message buffer
	struct buf {
		long mtype; // required
		char greeting[50]; // mesg content
	};
	buf msg;
	int size = sizeof(msg)-sizeof(long);

	//indicates if the sending programs are still reporting events
	bool StillSending997 = true;
	bool StillSending251 = true;

	do
	{
		if(StillSending997)
		{
			msgrcv(qid, (struct msgbuf *)&msg, size, 997, 0);

			cout << "Receiver 100 gets message: " << msg.greeting << endl;

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
				strcpy(msg.greeting, "Goodbye from Receiver 100");
				msg.mtype = 101;
				msgsnd (qid, (struct msgbuf *)&msg, size, 0); //exit protocol
			}
		}

		if(StillSending251)
		{
			msgrcv(qid, (struct msgbuf *)&msg, size, 251, 0);

			cout << "Receiver 100 gets message: " << msg.greeting << endl;

			//will set the boolean to receive from the 997 sender if the message begins with 'L'
			//	(expected "Last hello from sender 997")
			if(msg.greeting[0] == 'L')
			{
				StillSending251 = false;
			}
		}
	}while(StillSending997 || StillSending251);

	//notifies sender 997 of termination
	msg.mtype = 102;
	strcpy(msg.greeting, "Last goodbye from Receiver 100");
	msgsnd (qid, (struct msgbuf *)&msg, size, 0); //exit protocol

	//notifies closing program (sender 251) of termination
	msg.mtype = 103;
	msgsnd (qid, (struct msgbuf *)&msg, size, 0); //exit protocol

	exit(0);
}
