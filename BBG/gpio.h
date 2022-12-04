#ifndef GPIO_H_
#define GPIO_H_
#include <stdbool.h> //used for bool functions

// enum, get and set functions referenced from Exploring BeagleBone 2nd edition, Derek Molloy.
// see gpio.c for code.
enum GPIO_DIRECTION { GPIO_IN,
    GPIO_OUT };
enum GPIO_EDGE { GPIO_RISING,
    GPIO_BOTH };
enum GPIO_ACTIVE_LOW { GPIO_LOW,
    GPIO_HIGH };
enum GPIO_VALUE { GPIO_ONE,
    GPIO_ZERO };

// Struct to hold the gpio pin number and path to file
typedef struct GPIO {
    int number;
    char* path;
} gpio_t;

bool GPIO_setDirection(enum GPIO_DIRECTION, struct GPIO*);
// frunction that sets the direction of the gpio pin to "in" or "out"
// returns true if GPIO_DIRECTION is defined in enum otherwise false.

bool GPIO_setActiveLow(enum GPIO_ACTIVE_LOW, struct GPIO*);
// funtion that sets the activelow file for the gpio to 1 or 0
// returns true if the GPIO_ACTIVE_LOW is defined in enum otherwise false.

bool GPIO_setEdge(enum GPIO_EDGE, struct GPIO*);
// function that sets the edge of the gpio pin to rising or both.
// You can modify the enum GPIO_EDGE to include other configurations.
// returns true if GPIO_EDGE is defined in enum otherwise false.

int GPIO_waitForGpioEdge(gpio_t* gpio); // modifed from ENSC 351 guides file
// functions that creates an epoll instance to the specified gpio pin.
// Polls for a button interrupt.
// returns -1 if epoll cannot be created, 1 if timer expires otherwise 0.

int GPIO_getValue(struct GPIO*);
// function reads the value of the gpio pin.
// returns 1 or 0

bool GPIO_setValue(enum GPIO_VALUE, gpio_t* gpio);

#endif