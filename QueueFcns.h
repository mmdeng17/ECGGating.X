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
        int capacity;
        int size;
        int front;
        int rear;
        int *elements;
}Queue;

Queue * createQueue(int maxElements);
void Dequeue(Queue *Q);
int front(Queue *Q)
void Enqueue(Queue *Q,int element)


#endif	/* QUEUEFCNS_H */

