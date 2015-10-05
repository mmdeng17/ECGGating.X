#include <stdio.h>
#include <stdlib.h>
#define QUEUE_SIZE 500

typedef enum {false, true} boolean;

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

boolean isFull() {
	if(elementsNumber == QUEUE_SIZE) {
		return true;
	} 
	else {
		return false;
	}
}

boolean isEmpty() {
	if(elementsNumber == 0) {
		printf("Queue is empty!!!\n");
		return true;
	} 
	else {
		return false;
	}
}

void enqueue(char c) {
	rear = ++rear % QUEUE_SIZE;
	queue[rear] = c;
	elementsNumber++;
}

char dequeue() {
	char c = queue[front];
	front = ++front % QUEUE_SIZE;
	elementsNumber--;
	return c;
}
