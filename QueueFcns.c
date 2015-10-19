// #include <p18f46k22.h>

#define QUEUE_SIZE 10

float outVal;
float currD;
int front;
int rear;
int c;
int currSize;

unsigned char isFull(float *data) {
    if ((int) data[QUEUE_SIZE+2]==QUEUE_SIZE)
        return 1;
    else
        return 0;
}

unsigned char isEmpty(float *data) {
    if ((int) data[QUEUE_SIZE+2]==0)
        return 1;
    else
        return 0;
}

void enqueue(float *data, float newData) {
    data[(int) data[QUEUE_SIZE+1]] = newData;
    data[QUEUE_SIZE+2] = data[QUEUE_SIZE+2]+1;
    if (data[QUEUE_SIZE+2]>QUEUE_SIZE)
        data[QUEUE_SIZE+2] = QUEUE_SIZE;
    data[QUEUE_SIZE+1] = (int) (data[QUEUE_SIZE+1]+1)%QUEUE_SIZE;
}

float dequeue(float *data) {
    outVal = data[(int) data[QUEUE_SIZE]];
    data[QUEUE_SIZE+2] = data[QUEUE_SIZE+2]-1;
    if (data[QUEUE_SIZE+2]<0)
        data[QUEUE_SIZE+2] = 0;
    data[QUEUE_SIZE] = (int) (data[QUEUE_SIZE]+1)%QUEUE_SIZE;
    return outVal;
}

float peek (float *data) {
    return data[(int) data[QUEUE_SIZE]];
}

unsigned int getSize (float *data) {
    return (int) data[QUEUE_SIZE+2];
}

float getAvg (float *data) {
    currSize = getSize(data);
    
    if (currSize==0)
        return 0;
    
    front = (int) data[QUEUE_SIZE];
    rear = (int) data[QUEUE_SIZE+1];
    outVal = 0;
    c = 0;
    
    front = (int) data[QUEUE_SIZE];
    while(c<data[QUEUE_SIZE+2]) {
        outVal = ((float) c)/(c+1)*outVal + 1.0/(c+1)*data[front%QUEUE_SIZE];
        front++;
        c++;
    }
    
    return outVal;
}

float getDeriv(float *data,int rear) {
    if (getSize(data)<5)
        return 0;
    else {
        currD = 0.25*data[(rear+QUEUE_SIZE)%10];
        currD += 0.125*data[(rear+QUEUE_SIZE-1)%10];
        currD -= 0.125*data[(rear+QUEUE_SIZE-3)%10];
        currD -= 0.25*data[(rear+QUEUE_SIZE-3)%10];
        currD = currD*currD;
        return currD;
    }
}

unsigned char isQRS(float *data, float thresh) {
    if (isFull(data)!=1)
        return 0;
    else {
        rear = (int) data[QUEUE_SIZE];
        c = 0;
        while(c<(data[QUEUE_SIZE+2]-4)) {
            if (data[rear]>=thresh)
                return 1;
            rear = (rear-1+QUEUE_SIZE)%QUEUE_SIZE;
            c++;
        }
        return 0;
    }
}

unsigned int getQTDelay(float *data) {
    return 50;
}

