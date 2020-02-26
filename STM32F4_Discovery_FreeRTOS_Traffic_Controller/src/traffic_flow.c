/*
 * Traffic Flow Task. 
 * 
 * Traffic flow adjust task: The traffic flow that enters the intersection is set
 * by a potentiometer. This task reads the value of the potentiometer. The low 
 * resistance of the potentiometer corresponds to light traffic and a high 
 * resistance corresponds to heavy traffic. The reading by this task is used by 
 * other tasks.
 */


#include "traffic_flow.h"

void TrafficFlowAdjustTask ( void *pvParameters )
{
    uint16_t adc_value = 0; // Start with adc_value as 0, until it is read
    uint16_t speed_adc_value = 0;
    uint16_t current_speed_value = 0;
    uint16_t change_in_speed;
    uint16_t queue_value = 0;

	while(1)
	{
		ADC_SoftwareStartConv(ADC1); 
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
		adc_value = ADC_GetConversionValue(ADC1); 

		// The ADC ranges from 0 to 4096. Divide the raw ADC value by 512 to get 8 possible values.
		speed_adc_value = adc_value/512; 
		if(speed_adc_value >= 8)
		{
			speed_adc_value = 7;
		}

		/*
		if ( xQueueReceive( xADCQueue, &queue_value, 1000 ) == pdTRUE )
		{
			printf("Received old flow value from queue: %u.\n", queue_value);
		} */

        change_in_speed = abs(speed_adc_value - current_speed_value);

	    if(change_in_speed !=  0) 
	    {
	    	current_speed_value = speed_adc_value; 

			if( xSemaphoreTake( xMutexFlow, ( TickType_t ) 10 ) == pdTRUE ) 
		    {
				global_flowrate = speed_adc_value; 
				xSemaphoreGive( xMutexFlow ); 

				// testing queue sending
				if (xQueueSend( xADCQueue, &speed_adc_value, 1000) == pdTRUE )
				{
					printf("Updated flowrate:  %u, (ADC Value: %u). \nSent to ADC Queue. \n", speed_adc_value, adc_value );
				}
				else
				{
					printf("Failed to send ADC value to ADC Queue (traffic flow task). \n");
				}

				if (xQueueSend( xFlowQueue, &speed_adc_value, 1000) == pdTRUE )
				{
					printf("Updated flowrate:  %u, (ADC Value: %u). \nSent to Flow Queue. \n", speed_adc_value, adc_value );
				}
				else
				{
					printf("Failed to send ADC value to Car Queue (traffic flow task). \n");
				}

		    }
			else{
				printf("xMutexFlow unavailable \n");
			}
	    } 

        vTaskDelay(200);
	}
} 
