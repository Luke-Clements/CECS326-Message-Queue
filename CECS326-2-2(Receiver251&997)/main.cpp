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
 */
int main()
{
	/*
	0 -> assuming that the queue exists
	ftok 	-> same identifier as the other program (these must match otherwise the queue will not be found)[passed parameters must be the same]
		-> these programs must be in the same directory
	*/
	int qid = msgget(ftok("/volumes/USBDRIVE/eclipseworkspace/",'u'), 0);

	/*
	must be the same as other programs because the message objects in the queue are byte aligned
	*/
	// declare my message buffer
	struct buf {
		long mtype; // required
		char greeting[50]; // mesg content
	};
	buf msg;
	int size = sizeof(msg)-sizeof(long);

	/*
	getpid() -> gets an identifier for this program, not too useful here, but will be useful in later assignments
	*/
//	cout << getpid() << ": sends greeting" << endl;

	bool StillSending997 = true;
	bool StillSending251 = true;

	do
	{
		if(StillSending997)
		{
			if(0 > msgrcv(qid, (struct msgbuf *)&msg, size, 997, 0)) // reading
			{
				if(errno == EINTR)
				{
					StillSending997 = false;
				}
				else
				{
					cout << "something else went wrong" << endl;
				}
			}
			else
			{
				cout << getpid() << ": gets message" << endl;
				cout << "reply: " << msg.greeting << endl;
				cout << getpid() << ": now exits" << endl;
				msg.mtype = 101;
				strcpy(msg.greeting, "Goodbye from Receiver 100");
				msgsnd (qid, (struct msgbuf *)&msg, size, 0); //exit protocall
			}
		}

		if(StillSending251)
		{
			if(0 > msgrcv(qid, (struct msgbuf *)&msg, size, 251, 0))
			{
				if(errno == EINTR)
				{
					StillSending251 = false;
				}
				else
				{
					cout << "something else went wrong" << endl;
				}
			}
			else
			{
				cout << getpid() << ": gets message" << endl;
				cout << "reply: " << msg.greeting << endl;
				cout << getpid() << ": now exits" << endl;
			}
		}
	}while(StillSending997 || StillSending251);

	exit(0);
}
