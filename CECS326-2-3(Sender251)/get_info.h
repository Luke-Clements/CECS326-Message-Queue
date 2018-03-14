/*
 * get_info.h
 *
 *  Created on: Mar 13, 2018
 *      Author: lukecjm
 */

#ifndef GET_INFO_H_
#define GET_INFO_H_

#include<iostream>
#include<signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>

// declare my message buffer
	struct buf {
		long mtype; // required long datatype, and required to be first
		char greeting[50]; // mesg content
	};

void get_info(int qid, msgbuf *exitmsg, int size, long mtype);

#endif /* GET_INFO_H_ */
