// #include <p18f46k22.h>

#define QUEUE_SIZE 10

float outVal;
float currD;
int front;
int rear;
int c;
int currSize;

void reset(float *data) {
    c = 0;
    
    while(c<=QUEUE_SIZE+2) {
        data[c] = 0;
        c++;
    }
}


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

float dequeue(float *data) {
    if( ((int)data[QUEUE_SIZE+2])==0 ) {
        return 0.0;
    }
    outVal = data[(int) data[QUEUE_SIZE]];
    data[QUEUE_SIZE+2] = data[QUEUE_SIZE+2]-1;
    if (data[QUEUE_SIZE+2]<0)
        data[QUEUE_SIZE+2] = 0;
    data[QUEUE_SIZE] = (int) (data[QUEUE_SIZE]+1)%QUEUE_SIZE;
    return outVal;
}

void enqueue(float *data, float newData) {
    data[(int) data[QUEUE_SIZE+1]] = newData;
    data[QUEUE_SIZE+2] = data[QUEUE_SIZE+2]+1;
    
    if (data[QUEUE_SIZE+2]>QUEUE_SIZE) {
        data[QUEUE_SIZE] = ((int) (data[QUEUE_SIZE]+1))%QUEUE_SIZE;
        data[QUEUE_SIZE+2] = QUEUE_SIZE;
    }
    
    data[QUEUE_SIZE+1] = (int) ((int) (data[QUEUE_SIZE+1]+1))%QUEUE_SIZE;
}

float peek (float *data) {
    return data[(int) data[QUEUE_SIZE]];
}

float peekEnd (float *data) {
    return data[(int) data[QUEUE_SIZE+1]];
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
    currD = 0.25*data[(rear+QUEUE_SIZE)%QUEUE_SIZE];
    currD += 0.125*data[(rear+QUEUE_SIZE-1)%QUEUE_SIZE];
    currD -= 0.125*data[(rear+QUEUE_SIZE-3)%QUEUE_SIZE];
    currD -= 0.25*data[(rear+QUEUE_SIZE-4)%QUEUE_SIZE];
    currD = currD*currD;
    return currD;
}

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

unsigned int getQTDelay(float *data) {
    return 50;
}

unsigned int getRRInterval(float t1, float t2) {
    return t1-t2;
}

