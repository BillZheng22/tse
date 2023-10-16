/* 
 * Author: Bill Zheng, Daniel Jeon, Dhruv Chandra, Walker Ball
 * Created: 10-09-2023
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "list.h"

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

int main() {
    // Initialize your list or list-related resources here	
	printf("Non-empty list tests:\n");
	car_t *c1 = make_car("c1",20000.0,2003);

	car_t *c2 = make_car("c2",40000.0,2020);

	lput(c1);
	lapply(print_car);
	
	printf("Successfully put c1 into the initially empty list.\n");

	lput(c2);
	lapply(print_car);

	printf("Successfully put c2 at the start of the list.\n");

	lapply(double_price);
	lapply(print_car);

	printf("Successfully applied the double_price function to all cars in the list.\n");	

	car_t gotten_car = *lget();
	
	printf("Successfully got car with plate %s\n", gotten_car.plate);	

	lapply(print_car);
	
	printf("c2 has now been removed from the list after the lget function.\n");
	
	car_t *c3 = make_car("c3",10000.0,2010);

	car_t *c4 = make_car("c4",5000.0,2000);

	lput(c3);
	lput(c4);

	lapply(print_car);

 	printf("Put two more cars, c3 and c4, into the list.\n");
	
	char *car3 = "c3";
	
	car_t *removed_car = lremove(car3);
	printf("Successfully removed car with plate %s from the middle of the list.\n", (*removed_car).plate);
	
	lapply(print_car);
	
	car_t *removed_car_1 = lremove("c1");
	lapply(print_car);
	printf("Successfully removed car with plate %s from the end of the list.\n", (*removed_car_1).plate);

	car_t *removed_car_4 = lremove("c4");
	lapply(print_car);
 	printf("Successfully removed car with plate %s from the beginning of the list.\n", (*removed_car_4).plate);

	printf("List is now empty.\n");
	
	printf("Empty list tests:\n");
	
	//lapply on empty list test.
	lapply(print_car);
	printf("Test: Successfully lapplied on an empty list. (Passed)\n");

	//lget from empty list test.
	car_t *empty_gotten_car = lget();

  if (empty_gotten_car == NULL) {
		printf("Test: Successfully got from an empty list. (Passed)\n");
	} else {
		printf("Test: Failed to handle lget from an empty list. (Failed)\n");
	}

	//lremove from empty list test.
	car_t *empty_removed_car = lremove("123");

  if (empty_removed_car == NULL) {
		printf("Test: Successfully removed from an empty list. (Passed)\n");
	} else {
		printf("Test: Failed to handle lremove from an empty list. (Failed)\n");
	}
	
	free(c1);
	free(c2);
	free(c3);
	free(c4);
	return 0;
}
