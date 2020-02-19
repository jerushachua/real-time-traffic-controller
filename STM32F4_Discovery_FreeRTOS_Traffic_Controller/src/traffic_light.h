/*
 * Traffic Light header file. 
 *
 */

#ifndef TRAFFICLIGHT_H_
#define TRAFFICLIGHT_H_

#include "STM_config.h"

//Timer declarations
void vGreenLightTimerCallback( xTimerHandle xTimer );
void vYellowLightTimerCallback( xTimerHandle xTimer );
void vRedLightTimerCallback( xTimerHandle xTimer );

void TrafficLightTask( void *pvParameters );

xTimerHandle xRedLightSoftwareTimer;
xTimerHandle xYellowLightSoftwareTimer;
xTimerHandle xGreenLightSoftwareTimer;

#endif 
