/* 
 * queue_test.c --- 
 * 
 * Author: Bill Zheng, Daniel Jeon, Dhruv Chandra, Walker Ball
 * Created: 10-11-2023
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "queue.h"

#define MAXREG 10

typedef struct car {
	char plate[MAXREG];
	double price;
	int year;
} car_t;

car_t* make_car(char* cplate, double price, int year){                          
  car_t* cc;                                                                    
                                                                                
  if(!(cc = (car_t*)malloc(sizeof(car_t)))){                                    
    printf("[Error: malloc failed allocating car]\n");                          
    return NULL;                                                                
  }                                                               
  strcpy(cc->plate, cplate);                                                    
  cc->price = price;                                                            
  cc->year = year;                                                              
  return cc;                                                                    
}

void print_car(void *cp){
	car_t* car=(car_t*)cp;
	if (car==NULL){
		printf("List is empty.");
		return;
	}
  printf("Car plate=%s, Price=%.2lf, Year=%d\n",
				 car->plate, car->price, car->year);
	
}

void double_price(void *cp){
	car_t* car = (car_t*)cp;
	car->price *= 2.0;
}

int main(void){

	//Testing qopen()
	queue_t *queue_first = qopen();
	printf("Successfully opened a new queue using qopen().\n");

	//Testing qclose()
	qclose(queue_first);
	printf("Successfully closed queue using qclose().\n");

	queue_t *queue = qopen();
	printf("Successfully opened a new queue using qopen().\n");
	
	printf("Non-empty queue tests:\n");
	car_t *c1 = make_car("c1",20000.0,2003);
	//car_t *c2 = make_car("c2",40000.0,2020);

	qput(queue, c1);
	qapply(queue, print_car);
	printf("Successfully put c1 into queue.\n");
	qclose(queue);
	return 0;
}
