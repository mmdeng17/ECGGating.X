// #include <p18f46k22.h>

#define QUEUE_SIZE 10

float outVal;
float currD;
int front;
int rear;
int c;
int currSize;

// Function to reset all data in queue to 0;
void reset(float *data) {
    c = 0;
    while(c<QUEUE_SIZE) {
        data[c] = 0;
        c++;
    }
}

// Function to peek at last value in the queue
float peekQ(float *data,unsigned int ind) {
    return data[(ind+QUEUE_SIZE-1)%QUEUE_SIZE];
}

// Function to add value to queue and update queue index
float enqueueQ(float *data, unsigned int *ind, float newVal) {
    data[*ind] = newVal;
    *ind = (*ind+1)*QUEUE_SIZE;
}

// Function to calculate deriv^2 in ECG
//  5-step backward difference
float getDeriv(float *data,int rear) {
    currD = 0.25*data[(rear+QUEUE_SIZE)%QUEUE_SIZE];
    currD += 0.125*data[(rear+QUEUE_SIZE-1)%QUEUE_SIZE];
    currD -= 0.125*data[(rear+QUEUE_SIZE-3)%QUEUE_SIZE];
    currD -= 0.25*data[(rear+QUEUE_SIZE-4)%QUEUE_SIZE];
    currD = currD*currD;
    return currD;
}

// Function to check for QRS peak
//  checks entire queue for any values greater than threshold
//  returns 1 if value exists
unsigned char isQRS(float *data, float thresh, unsigned int rear) {
    rear = (rear+QUEUE_SIZE-1)%QUEUE_SIZE;
    c = 0;
    while(c<QUEUE_SIZE) {
        if (data[rear]>=thresh)
            return 1;
        rear = (rear-1+QUEUE_SIZE)%QUEUE_SIZE;
        c++;
    }
    return 0;
}

// Function to calculate threshold based on last 5 peaks
//  Gaussian window on last 5 peaks, 75% value
float getThresh(float *valData,unsigned int valInd) {
    float tmp = 0;
    tmp += 0.643*valData[(valInd+QUEUE_SIZE-1)%QUEUE_SIZE];
    tmp += 0.236*valData[(valInd+QUEUE_SIZE-2)%QUEUE_SIZE];
    tmp += 0.087*valData[(valInd+QUEUE_SIZE-3)%QUEUE_SIZE];
    tmp += 0.031*valData[(valInd+QUEUE_SIZE-4)%QUEUE_SIZE];
    tmp += 0.003*valData[(valInd+QUEUE_SIZE-5)%QUEUE_SIZE];
    return 0.75*tmp;
}

// Function to calculate heart rate interval based on last 5 peaks
//  Gaussian window on last 5 peaks
float getRRInterval(float *peakData, unsigned int peakInd) {
    float tmp = 0;
    tmp += 0.646*peakData[(peakInd+QUEUE_SIZE-1)%QUEUE_SIZE];
    tmp -= 0.410*peakData[(peakInd+QUEUE_SIZE-2)%QUEUE_SIZE];
    tmp -= 0.149*peakData[(peakInd+QUEUE_SIZE-3)%QUEUE_SIZE];
    tmp -= 0.056*peakData[(peakInd+QUEUE_SIZE-4)%QUEUE_SIZE];
    tmp -= 0.031*peakData[(peakInd+QUEUE_SIZE-5)%QUEUE_SIZE];
}

