/*
 * Traffic Display file. 
 *
 * Traffic display task: This task controls cars moving across the LED display. 
 * There are different cases for when the light is green or red. 
 * 
 */


#include "traffic_display.h"

void TrafficDisplayTask ( void *pvParameters )
{
	//get value from traffic creator
	uint16_t car_value = 0; 
	uint16_t light_colour = 1; 
    uint16_t cur_prelight_traffic[8] = {0}; 
    uint16_t next_prelight_traffic[8] = {0};

	while(1)
	{
		// Update local car and light variables
		if ( xQueueReceive( xCarQueue, &car_value, 1000 ) == pdTRUE) // testing queue receive
		{
			printf("Car Queue -- pop --> %u \n", car_value);
		}
		else
		{
			printf("Failed to receive car value from Car Queue (traffic display task). \n ");
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

		// Light is green. 
		if(light_colour == 1)
		{
			printf("Light is green. \n ");

			SR_PreLight(car_value); 
			SR_PostLight(cur_prelight_traffic[7]); 

			next_prelight_traffic[0] = car_value; 

			// shift the new car onto the list of cars
			for (uint16_t i = 1; i != 8; i++) 
			{
				next_prelight_traffic[i] = cur_prelight_traffic[i-1];
			}
		}

		// Light is red. 
		else if(light_colour == 0)
		{
			printf("Light is red. \n ");

			uint16_t gap = 0;

			/* 
			 * The light is red. This means we need to shift the cars up to the stop line.
			 * The cars should be bumper to bumper, without any gaps in the LEDs. 
			 * The "gaps" are represented by zeros. 
			 * 
			 * Search the traffic array for zeros. If there are any, shift all the cars up by one spot. 
			 * Keep track of whether or not a gap is found 
			 * 
			 */ 
			for (uint16_t i = 7; i != 0; i--) 
			{
	            if(cur_prelight_traffic[i] == 0) 
	            {
	            	gap = 1;
	            	next_prelight_traffic[0] = car_value;
	            } 

				// If a gap was found, shift all the cars up by one spot. 
	            if(gap == 1)
	            {
	            	next_prelight_traffic[i] = cur_prelight_traffic[i-1];
	            } else {
	            	next_prelight_traffic[i] = cur_prelight_traffic[i];
	            }
			}

			// Now all cars have been moved appropriately. Send the data to the shift registers. 
			for (int16_t i = 7; i >= 0 ; i--) 
			{
				SR_PreLight(next_prelight_traffic[i] );
			}
			SR_PostLight(0); // bug: if light is red, all cars POST light disappear. 
		} 

		// Move all the cars forward by one. 
		for(uint16_t i = 0; i != 8; i++)
		{
			cur_prelight_traffic[i] = next_prelight_traffic[i];
		}

		vTaskDelay(500);
	}
}
