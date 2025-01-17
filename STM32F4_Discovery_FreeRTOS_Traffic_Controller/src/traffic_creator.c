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
		if (xQueueReceive( xADCQueue, &flowrate, 1000 ) == pdTRUE )
		{
			printf("ADC queue -- pop --> %u \n", flowrate);
		}
		else
		{
			printf("Failed to receive value from ADC Queue (traffic creator task).\n" );
		}

		/*
		 * compute the value for the display (0/1)
		 * flowrate is between 0 and 7
		 * generate random number range 0 to 64, inclusive
		 * if the random number is below 64 / flowrate + 1, create a car
		 * if the value from traffic flow task is high, there is a higher probability of a car being created
		 *  rand num % 64 < 8  * flowrate + 1
		 *  0 to 63 range < 8 * ( 1 to 8 range) 
		 *  then at max flow rate, the probability of a car being created is near 100% 
		 *  0 to 63 range < 8*8 = 64 
		*/
		car_value = ( rand() % 64 ) < ( 8 * (flowrate+1) * 0.80 ); // 80 percent chance of car at max flow.

		if ( xQueueSend( xCarQueue, &car_value, 1000) == pdTRUE )
		{
			printf("%u -- push --> Car Queue.\n", car_value); 
		}
		else
		{
			printf("Failed to send to Car Queue (traffic creator task). \n");
		}

		vTaskDelay(200);
	}
} 
