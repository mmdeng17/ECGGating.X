/* 
 * File:   QueueFcns.h
 * Author: BME464
 *
 * Created on October 12, 2015, 11:08 AM
 */

#ifndef QUEUEFCNS_H
#define	QUEUEFCNS_H

unsigned char isFull(float *data);
unsigned char isEmpty(float *data);
void enqueue(float *data, float newData);
float dequeue(float *data);
float peek(float *data);

unsigned int getSize(float *data);
float getAvg(float *data);
unsigned char isQRS(float *data, float thresh);

#endif	/* QUEUEFCNS_H */