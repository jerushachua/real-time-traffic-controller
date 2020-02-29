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
    uint16_t queue_value = 0;

	while(1)
	{
		ADC_SoftwareStartConv(ADC1); 
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
		adc_value = ADC_GetConversionValue(ADC1); 

		// The ADC ranges from 0 to 4096. Divide the raw ADC value by 512 to get 8 possible values.
		speed_adc_value = adc_value / 512; 
		if(speed_adc_value >= 8) 
		{
			speed_adc_value = 7;
		}

		// If either queue has a readable value, pop both off and compare to the current value. Update value if needed. 
		if ( (xQueueReceive( xADCQueue, &queue_value, 1000 ) == pdTRUE) || (xQueueReceive( xFlowQueue, &queue_value, 1000 )) )
		{
			printf("Old ADC speed from queue -- pop --> %u \n", queue_value); 

			// Update the Flow Queue and ADC Queue.
			printf("Updating flowrate:  %u, (ADC Value: %u). \n", speed_adc_value, adc_value );

			if (xQueueSend( xADCQueue, &speed_adc_value, 1000) == pdTRUE )
			{
				printf("%u -- push --> ADC Queue. \n", speed_adc_value);
			}
			else
			{
				printf("Failed to send ADC value to ADC Queue (traffic flow task). \n");
			}

			if (xQueueSend( xFlowQueue, &speed_adc_value, 1000) == pdTRUE )
			{
				printf("%u -- push --> Flow Queue. \n", speed_adc_value);
			}
			else
			{
				printf("Failed to send ADC value to Flow Queue (traffic flow task). \n");
			}
		}
		else
		{
			// Queue is empty, push items onto queues.
			printf("Queue is empty, push flowrate onto queues:  %u, (ADC Value: %u). \n", speed_adc_value, adc_value );

			if (xQueueSend( xADCQueue, &speed_adc_value, 1000) == pdTRUE )
			{
				printf("%u -- push --> ADC Queue. \n", speed_adc_value);
			} else
			{
				printf("Failed to send ADC value to ADC Queue (traffic flow task). \n");
			}

			if (xQueueSend( xFlowQueue, &speed_adc_value, 1000) == pdTRUE )
			{
				printf("%u -- push --> Flow Queue. \n", speed_adc_value);
			}
			else
			{
				printf("Failed to send ADC value to Flow Queue (traffic flow task). \n");
			}
		}

        vTaskDelay(200);
	}
} 
