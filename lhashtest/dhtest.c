/*
    Daniel Jeon, Bill Zhang, Walker Ball, Dhruv Chandra
    Fall 2023
    Dual-Threaded Test for lhash.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <lhash.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

/* car structure for example */
typedef struct car {
	char plate[7];
	double price;
	int year;
} car_t;

/* makes a car structure */
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

/* prints the car structure */
void print_car(void *cp){
	car_t* car=(car_t*)cp;
	if (car==NULL){
		printf("List is empty.");
	}
    printf("Car plate=%s, Price=%.2lf, Year=%d\n", car->plate, car->price, car->year);
}

/* doubles the price of the cars (for thread 1) */
void double_price(void *cp){
	car_t* car = (car_t*)cp;
	car->price *= 2.0;
}

/* decreases the model's year (for thread 2) */
void decrease_year(void *cp) {
    car_t* car = (car_t*) cp;
    car->year -= 1;
}

/* sample search function for elements in the locked hash */
bool searchfn(void * elementp, const void* searchkeyp) {
    car_t* car = (car_t *) elementp; 
    return strcmp(car->plate, (const char*) searchkeyp) == 0;
}

/* doubles the price of the first car */
void *tfunc1(void * argp) {
    lhash_t * lht = (lhash_t *) argp;
    locklhash(lht);
    car_t * car = lhsearch(lht, searchfn, "HMN1980", 7);
    double_price(car);

    unlocklhash(lht);
    return (void*) 0;
}

/* decreases the year of the second car */
void *tfunc2(void * argp) {
    lhash_t * lht = (lhash_t *) argp;
    locklhash(lht);
    car_t * car = lhsearch(lht, searchfn, "JFK8790", 7);
    decrease_year(car);

    unlocklhash(lht);
    return (void*) 0;
}

int main(void) {
    pthread_t tid1;
    pthread_t tid2;
    
    //open a locked hash with five queues
    lhash_t *lht = lhopen(5);
    printf("Successfully opened a new locked-hash using lhopen().\n");

    car_t *car1 = make_car("HMN1980", 49000, 2009);
    printf("Successfully made car1 structure.\n");
    car_t *car2 = make_car("JFK8790", 30000, 2017);
    printf("Successfully made car2 structure.\n");

    printf("\n");
    printf("INITIAL DATA:\n");
    print_car(car1);
    print_car(car2);
    printf("\n");
    
    lhput(lht, car1, "HMN1980", 7);
    printf("Successfully put car1 into the locked-hash.\n");
    lhput(lht, car2, "JFK8790", 7);
    printf("Successfully put car2 into the locked-hash.\n");

    if(pthread_create(&tid1,NULL,tfunc1,lht)!=0) {
        exit(EXIT_FAILURE);
    }
    printf("Successfully created pthread tid1.\n");
    if(pthread_create(&tid2,NULL,tfunc2,lht)!=0) {
        exit(EXIT_FAILURE);
    }
    printf("Successfully created pthread tid2.\n");

    if (pthread_join(tid1, NULL) != 0) {
        exit(EXIT_FAILURE);
    }
    printf("Successfully rejoined the pthread tid1.\n");
    if (pthread_join(tid2, NULL) != 0) {
        exit(EXIT_FAILURE);
    }
    printf("Successfully rejoined the pthread tid2.\n");

    printf("\n");
    printf("END RESULT:\n");
    print_car(car1);
    print_car(car2);
    printf("\n");

    lhclose(lht);
    exit(EXIT_SUCCESS);
}