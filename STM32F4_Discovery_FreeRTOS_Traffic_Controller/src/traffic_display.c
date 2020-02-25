/*
 * Traffic Display file. 
 *
 * Traffic light task: This task controls the timing of the traffic light. 
 * The timing of the light is directly affected by the flow of traffic. 
 * 
 */


#include "traffic_display.h"

void TrafficDisplayTask ( void *pvParameters )
{
	//get value from traffic creator
	uint16_t car_value = 0; 
	uint16_t light_colour = 1; 
    uint16_t currentactiveprelighttraffic[8] = {0}; 
    uint16_t newactiveprelighttraffic[8] = {0};

	while(1)
	{
		// Update local car and light variables
		if( xSemaphoreTake( xMutexCars, ( TickType_t ) 10 ) == pdTRUE ) 
		{
			car_value = global_car_value; 
			xSemaphoreGive( xMutexCars ); 
		}
		else
		{
			printf("xMutexCars unavailable. \n");
		}

		if( xSemaphoreTake( xMutexLight, ( TickType_t ) 0 ) == pdTRUE ) 
	    {
			light_colour = global_light_colour;
			xSemaphoreGive( xMutexLight ); 
			printf("Traffic light colour currently %u. (1 is green, 0 is red) \n", light_colour);
	    }
		else
		{
			printf("xMutexLight unavailable.  \n");
		}


		if(light_colour == 1)
		{
			printf("Light is green, shifting normally. \n ");

			SR_PreLight(car_value); 
			SR_PostLight(currentactiveprelighttraffic[7]); 

			newactiveprelighttraffic[0] = car_value; 

			// shift the new car onto the list of cars
			for (uint16_t i = 1; i != 8; i++) 
			{
				newactiveprelighttraffic[i] = currentactiveprelighttraffic[i-1];
			}
		}
		else if(light_colour == 0)
		{
			printf("Light is red, doing fast shift. \n ");

			uint16_t encounteredzero = 0;

			// TODO: refactor this 
			for (uint16_t i = 7; i != 0; i--)                                          // Search through the traffic list by decrementing, looking for the first 0
			{
	            if(currentactiveprelighttraffic[i] == 0)                               // Find a zero in the active traffic. If it exists, set the encounteredzero flag
	            {
	            	encounteredzero = 1;                                               // Set encounteredzero flag high
	            	newactiveprelighttraffic[0] = car_value;                           // A zero exists in the active traffic (traffic isn't full), so shifting new car on.
	            }

	            if(encounteredzero == 1)                                               // If zero is found, shift the remaining values normally.
	            {
	            	newactiveprelighttraffic[i] = currentactiveprelighttraffic[i-1];
	            }
	            else                                                                   // Zero not found yet, so no shifting occurs as cars don't move on a red light
	            {
	            	newactiveprelighttraffic[i] = currentactiveprelighttraffic[i];
	            }
			}


			for (int16_t i = 7; i >= 0 ; i--) 
			{
				SR_PreLight(newactiveprelighttraffic[i] );
			}
			SR_PostLight(0);
		}

		// move all the lights forward
		for(uint16_t i = 0; i != 8; i++)
		{
			currentactiveprelighttraffic[i] = newactiveprelighttraffic[i];
		}

		vTaskDelay(100);
	}
}
