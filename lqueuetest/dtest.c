/*
    Daniel Jeon, Bill Zhang, Walker Ball, Dhruv Chandra
    Fall 2023
    Dual-Threaded Test
*/
#include <stdio.h>
#include <stdlib.h>
#include <lqueue.h>
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

/* sample search function for elements in the locked queue */
bool searchfn(void * elementp, const void* searchkeyp) {
    car_t* car = (car_t *) elementp; //should I use car pointers or car structures themselves? 
    return strcmp(car->plate, (const char*) searchkeyp) == 0;
}

/* doubles the price of the first car */
void *tfunc1(void * argp) {
    lqueue_t * lqp = (lqueue_t *) argp;
    locklqueue(lqp);
    car_t * car = lqsearch(lqp, searchfn, "HMN1980");
    double_price(car);

    unlocklqueue(lqp);
    return (void*) 0;
}

/* decreases the year of the second car */
void *tfunc2(void * argp) {
    lqueue_t * lqp = (lqueue_t *) argp;
    locklqueue(lqp);
    car_t * car = lqsearch(lqp, searchfn, "JFK8790");
    decrease_year(car);

    unlocklqueue(lqp);
    return (void*) 0;
}

int main(void) {
    pthread_t tid1;
    pthread_t tid2;
    
    //open a locked queue
    lqueue_t *lqp = lqopen();
    printf("Successfully opened a new locked-queue using lopen().\n");

    car_t *car1 = make_car("HMN1980", 49000, 2009);
    printf("Successfully made car1 structure.\n");
    car_t *car2 = make_car("JFK8790", 30000, 2017);
    printf("Successfully made car2 structure.\n");

    printf("\n");
    printf("INITIAL DATA:\n");
    print_car(car1);
    print_car(car2);
    printf("\n");

    lqput(lqp, car1);
    printf("Successfully put car1 into the locked-queue.\n");
    lqput(lqp, car2);
    printf("Successfully put car2 into the locked-queue.\n");

    if(pthread_create(&tid1,NULL,tfunc1,lqp)!=0) {
        exit(EXIT_FAILURE);
    }
    printf("Successfully created pthread tid1.\n");
    if(pthread_create(&tid2,NULL,tfunc2,lqp)!=0) {
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

    lqclose(lqp);
    exit(EXIT_SUCCESS);
}