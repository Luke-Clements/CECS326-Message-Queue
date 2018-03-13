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
#include<unistd.h>
#include <sys/types.h>

void get_info(int qid, msgbuf *exitmsg, int size, long mtype);

#endif /* GET_INFO_H_ */
