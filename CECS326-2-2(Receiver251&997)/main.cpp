/*
 * main.cpp
 *
 *  Created on: Feb 15, 2018
 *      Author: lukecjm
 */

/*
This is a simple illustration of the use of:
	ftok, msgget, msgsnd, msgrcv
Program A will use a message queue created by Program B.
Then, they will pass messages back and forth.
Program A sends the first message and reads the reply. Program A
also sends a "fake" message to the msgQ that will never be read
by anyone.
Both child processes use message type mtype = 113 and 114. (these two numbers will not be used)
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
	int qid = msgget(ftok(".",'u'), 0);

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

	do
	{
	msgrcv(qid, (struct msgbuf *)&msg, size, 997, 0); // reading
	cout << getpid() << ": gets message" << endl;
	cout << "reply: " << msg.greeting << endl;
	cout << getpid() << ": now exits" << endl;
	msg.mtype = 101;
	strcpy(msg.greeting, "Goodbye from Receiver 100");
	msgsnd (qid, (struct msgbuf *)&msg, size, 0); //exit protocall

	msgrcv(qid, (struct msgbuf *)&msg, size, 251, 0);
	cout << getpid() << ": gets message" << endl;
	cout << "reply: " << msg.greeting << endl;
	cout << getpid() << ": now exits" << endl;

	//find out how to tell when a sender stops sending
	}while(true);

	exit(0);
}
