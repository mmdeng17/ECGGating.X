/* 
 * File:   OutFcns.h
 * Author: BME464
 *
 * Created on October 2, 2015, 10:16 AM
 */

#ifndef QUEUEFCNS_H
#define	QUEUEFCNS_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef	__cplusplus
}
#endif

#include <stdio.h>

typedef struct Queue {
	int data[QUEUE_SIZE];
	int front = 0;
	int rear = -1;
	int elementsNumber = 0;
}

boolean isFull();
boolean isEmpty();
void enqueue(char c);
char dequeue();


#endif	/* QUEUEFCNS_H */

