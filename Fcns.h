#ifndef _FCNS_H
#define _FCNS_H

#include <delays.h>
#include "Lcd.h"

unsigned int readAVin(void);
void DACVolt(unsigned int volt);

unsigned char isRB6(void);
unsigned char isRB7(void);
void SettingsMode(unsigned int *a1, unsigned int *a2, unsigned int *a3, unsigned int *a4);

#endif
