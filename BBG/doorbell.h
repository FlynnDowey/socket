#ifndef DOORBELL_H_
#define DOORBELL_H_

#include <stdbool.h>
void Doorbell_init(void);
void Doorbell_cleanup(void);

bool Doorbell_isPressed(void);
void Doorbell_turnLedOFF(void);
#endif