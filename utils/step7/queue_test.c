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
  cc->next = NULL;                                                              
  strcpy(cc->plate, cplate);                                                    
  cc->price = price;                                                            
  cc->year = year;                                                              
  return cc;                                                                    
}

void print_car(car_t *cp){

	if (cp==NULL){
		printf("List is empty.");
		return;
	}
  printf("Car plate=%s, Price=%.2lf, Year=%d\n",
				 cp->plate, cp->price, cp->year);
	
}

void double_price(car_t *cp){
	cp->price *= 2.0;
}

int main(){

	//change for queue!
	car_t *c1 = make_car("c1",20000.0,2003);

	car_t *c2 = make_car("c2",40000.0,2020);
	
	queue_t * queue = qopen();
	qput(queue, c1);
	qput(queue, c2);
	qclose(queue);
	return 0;
}
