#ifndef APPLICATIONS_SYSTEMCALLAPI_H_
#define APPLICATIONS_SYSTEMCALLAPI_H_

#include <stdbool.h>

#define LED_0   0
#define LED_1   1

int sysCalls_enableLed(bool turnOn, int led);

#endif /* APPLICATIONS_SYSTEMCALLAPI_H_ */
