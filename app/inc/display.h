#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "main.h"

typedef enum {
    DISPLAY_CONNECTION_GPIO_4BITS,
    DISPLAY_CONNECTION_GPIO_8BITS
} displayConnection_t;

void displayInit(displayConnection_t connection);
void displayCharPositionWrite(uint8_t x, uint8_t y);
void displayStringWrite(const char *str);

#endif
