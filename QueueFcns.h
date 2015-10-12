/* 
 * File:   QueueFcns.h
 * Author: BME464
 *
 * Created on October 12, 2015, 11:08 AM
 */

#ifndef QUEUEFCNS_H
#define	QUEUEFCNS_H

unsigned char isFull(double *data);
unsigned char isEmpty(double *data);
void enqueue(double *data, double newData);
double dequeue(double *data);
double peek(double *data);

unsigned int getSize(double *data);
double getAvg(double *data);

#endif	/* QUEUEFCNS_H */