/*
 * Helper to create traffic. 
 * 
 * Traffic creator task: This task generates random traffic at a rate that is
 * defined from the potentiometer value. This value is received from the traffic
 * flow adjustment task. The created traffic is sent to the task which displays 
 * the cars on the road.
 *
 */

#include "traffic_creator.h"

void TrafficCreatorTask ( void *pvParameters )
{
	uint16_t flowrate = 4;
	uint16_t car_value = 0;

	while(1)
	{
		printf("Starting flowrate:  %u. \n", flowrate );
		if( xSemaphoreTake( xMutexFlow, ( TickType_t ) 10 ) == pdTRUE ) 
		{
			flowrate = g_flowrate;
			xSemaphoreGive( xMutexFlow );
			printf("Updated flowrate:  %u. \n", flowrate );
		}
		else
		{
			printf("xMutexFlow unavailable. \n");
		}

		/*
		 * compute the value for the display (0/1)
		 * received should be a value 1-8
		 * generate random number range[0:100]
		 * if the random number is below 100/(8 - value from traffic flow task) create a car
		 * if the value from traffic flow task is high, there is a higher probability of a car being created
		*/
		car_value = (rand() % 100 ) < 100/(8 - flowrate);

		printf("Car value updated to:  %u \n", car_value);


		if( xSemaphoreTake( xMutexCars, ( TickType_t ) 10 ) == pdTRUE ) // get flowrate mutex to update with new traffic flowrate
		{
			g_car_value = car_value;
			xSemaphoreGive( xMutexCars );
			printf("Updated car value:  %u. \n", car_value );
		}
		else
		{
			printf("xMutexCars unavailable.  \n");
		}

		vTaskDelay(500);
	}
} 
