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

	// create my msgQ with key value from ftok()
	/*
	msgget:
	either allocates or locates existing queue
	2 parameters

	IPC_CREAT -> using this to allocate the queue
	IPC_EXCL -> brand new queue
	0600 	-> access permission (octal)
		-> each digit(last three) represents RWX (Read Write Execute) mix
			-> 0600 => 6 = 110 -> R = 1, W = 1, X = 0 (read write access)
		-> first zero is required to fill space
		-> second digit is in regards to the owner
		-> third digit regards the group
		-> fourth group regards others
		permission should be as strict as possible

	ftok(".", 'u') 	-> internal OS identifier
			-> better to let the OS allocate these identifiers to avoid conflicts (can be hard coded, but don't do that)
			-> generates an id for the system to use based on the two parameters in ftok
				-> first parameter: string path to your program (eg. /USBDRIVE/eclipseworkspace/...)
				-> second parameter: user selected integer
	*/
	//qid -> an id that is internal to this program
	int qid = msgget(ftok("/volumes/USBDRIVE/eclipseworkspace/",'u'), IPC_EXCL|IPC_CREAT|0600);

	/*
	holds the messages that is passed between the programs
	the OS uses some information in this struct, so the "long mtype" must go first so it can find it
	*/
	// declare my message buffer
	struct buf {
		long mtype; // required long datatype, and required to be first
		char greeting[50]; // mesg content
	};

	buf msg;

	/*
	Calculate the size of the message
	OS needs to know how much meory to allocate for the passed message
	*/
	int size = sizeof(msg)-sizeof(long);
	
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
		/*
		Used by this program to receive a message
		each receive removes one message from the queue if successful

		qid 	-> see above
		(struct msgbuf *) -> typecast so the OS will "understand" what is being passed since this function is provided by the OS
		msg 	-> received message passed by reference
		size 	-> see above (how many bytes of message to copy)
		117 	-> mtype 	-> used to differentiate between messages
					-> must be accurate in order for a passed message te be received
					-> within a given mtype (if there were multiple of the same mtype) they will behave FIFO
					-> abstractions will help (hint)
		0 	-> determines how function behaves
			-> 0 -> synchronice (wait until we get the message to continue)[forces a halt in execution until the message is available]
				-> do not put IPC_NOWAIT here!!!
			-> IPC_NOWAIT -> asynchronice -> NEVER USE THIS!!!!!!!!!!!!!!
		*/
		//msgrcv(qid, (struct msgbuf *)&msg, size, 117, 0); // read mesg
							// don't read "fake" mesg

		//initializing message for receiver 100
		msg.mtype = 100;
		strcpy(msg.greeting, "Hello from Sender 997: ");
		msgsnd(qid, (struct msgbuf *)&msg, size, 0);

		//initializing message for receiver 200
		msg.mtype = 200;
		msgsnd(qid, (struct msgbuf *)&msg, size, 0);

		//will create some slowing as order is expected as important here between the two receives
		msgrcv(qid, (struct msgbuf *)&msg, size, 101, 0);
		cout << getpid() << ": gets message" << endl;
		cout << "message: " << msg.greeting << endl;

		msgrcv(qid, (struct msgbuf *)&msg, size, 201, 0);
		cout << getpid() << ": gets message" << endl;
		cout << "message: " << msg.greeting << endl;

		//get a random unsigned integer value that is divisible by 997
		do
		{
			randomUInt  = UINT_MAX*rand();
		}while(randomUInt%997 != 0 || randomUInt < 100);
	}
	
	//sending last message to both receivers
	//initializing message for receiver 100
	msg.mtype = 100;
	strcpy(msg.greeting, "Last hello from Sender 997: ");
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);

	//initializing message for receiver 200
	msg.mtype = 200;
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);

//		/*
//		-112 (mtype) -> looking for an mtype that is <= |-112|, which means that the message with the 111 mtype will be gathered
//		if there are more than one with mtype <= |-112|, FIFO is in effect
//		see: man 2 msgrcv for a description of how negative mtypes work
//
//		0 (mtype) -> looking for an mtype that has been in the queue for the longest (forces FIFO on the queue), which means that 113 will be gathered
//
//		These two are essentially emptying the queue
//		*/
//		msgrcv (qid, (struct msgbuf *)&msg, size, -112, 0);
//		msgrcv (qid, (struct msgbuf *)&msg, size, 0, 0);
//		//exit protocall -> prevents the queue from being removed too early and having a race condition: removing queue too soon
//		msgrcv (qid, (struct msgbuf *)&msg, size, 117, 0); //we know this is the last message, therefore we are finished with A (A no longer needs the queue)
//
//		/*
//		IPC_RMID -> remove this id
//		NULL -> pointer we don't care to provide
//		*/
//		// now safe to delete message queue
//		msgctl (qid, IPC_RMID, NULL);

	exit(0);
}
