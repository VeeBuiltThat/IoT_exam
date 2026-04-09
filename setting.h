#ifndef SETTING_H
#define SETTING_H

#include <Arduino.h>
#include <stddef.h>
#include <RF24.h>

extern RF24 Radio;

void configureNrf(RF24 &radio);

#endif
