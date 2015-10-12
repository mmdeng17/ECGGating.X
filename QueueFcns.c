// #include <p18f46k22.h>

#define QUEUE_SIZE 25

double outVal;
int front;
int rear;
int c;
int currSize;

unsigned char isFull(double *data) {
    if ((int) (data[QUEUE_SIZE+1]+1)%QUEUE_SIZE==data[QUEUE_SIZE])
        return 1;
    else
        return 0;
}

unsigned char isEmpty(double *data) {
    if (data[QUEUE_SIZE]==data[QUEUE_SIZE])
        return 1;
    else
        return 0;
}

void enqueue(double *data, double newData) {
    data[(int) data[QUEUE_SIZE+1]] = newData;
    data[QUEUE_SIZE+1] = (int) (data[QUEUE_SIZE+1]+1)%QUEUE_SIZE;
}

int dequeue(double *data) {
    double outVal = data[(int) data[QUEUE_SIZE]];
    data[QUEUE_SIZE] = (int) (data[QUEUE_SIZE]+1)%QUEUE_SIZE;
    return outVal;
}

int peek (double *data) {
    return (int) data[(int) data[QUEUE_SIZE]];
}

unsigned int getSize (double *data) {
    outVal = 0;
    if (data[QUEUE_SIZE]<=data[QUEUE_SIZE+1])
        outVal = data[QUEUE_SIZE+1]-data[QUEUE_SIZE];
    else
        outVal = QUEUE_SIZE-data[QUEUE_SIZE]+data[QUEUE_SIZE+1];
    
    return (int) outVal;
}

float getAvg (double *data) {
    currSize = getSize(data);
    
    if (currSize==0)
        return 0;
    
    front = (int) data[QUEUE_SIZE];
    rear = (int) data[QUEUE_SIZE+1];
    outVal = 0;
    c = 0;
    
    while(front!=rear) {
        c++;
        outVal+= data[front];
        front = (front+1)%QUEUE_SIZE;
    }
    
    return outVal/currSize;
}