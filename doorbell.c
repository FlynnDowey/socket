#include "doorbell.h"
#include "general.h"
#include "gpio.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static char* BTN_INIT_COMMAND = "config-pin p9_28 gpio";
static char* LED_INIT_COMMAND = "config-pin p9_27 gpio";

#define BTN_PATH "/sys/class/gpio/gpio113"
static const int BTN_NUM = 70;

#define LED_PATH "/sys/class/gpio/gpio115"
static const int LED_NUM = 71;

static bool isDone = false;
static bool isPressed = false;
static pthread_t idDoor;
static gpio_t btn = { BTN_NUM, BTN_PATH };
static gpio_t led = { LED_NUM, LED_PATH };

static void* doorbell(void* arg);

void Doorbell_init(void)
{
    General_runCommand(BTN_INIT_COMMAND);
    General_runCommand(LED_INIT_COMMAND);

    GPIO_setDirection(GPIO_IN, &btn);
    GPIO_setDirection(GPIO_OUT, &led);

    GPIO_setActiveLow(GPIO_HIGH, &btn);
    GPIO_setActiveLow(GPIO_LOW, &led);

    GPIO_setEdge(GPIO_RISING, &btn);

    pthread_create(&idDoor, NULL, doorbell, NULL);
}
void Doorbell_cleanup(void)
{
    isDone = true;
    pthread_join(idDoor, NULL);
}

static void* doorbell(void* arg)
{
    while (!isDone) {
        int val = GPIO_waitForGpioEdge(&btn);
        if (val) {
            isPressed = true;
            printf("pressed.\n");
            General_runCommand("echo 1 > /sys/class/gpio/gpio115/value");
            sleep(1);
            isPressed = false;
        }
    }
    return NULL;
}

bool Doorbell_isPressed(void)
{
    return isPressed;
}

void Doorbell_turnLedOFF(void)
{
    General_runCommand("echo 0 > /sys/class/gpio/gpio115/value");
}
