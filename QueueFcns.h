/* 
 * File:   QueueFcns.h
 * Author: BME464
 *
 * Created on October 12, 2015, 11:08 AM
 */

#ifndef QUEUEFCNS_H
#define	QUEUEFCNS_H

void reset(float *data);
float peekQ(float *data,unsigned int ind);
void enqueueQ(float *data, unsigned int *ind, float newVal);

float getDeriv(float *data, int rear);
unsigned char isQRS(float *data, float thresh, unsigned int rear);
float getThresh(float *valData,unsigned int valInd);
float getRRInterval(float *peakData, unsigned int peakInd);

#endif	/* QUEUEFCNS_H */