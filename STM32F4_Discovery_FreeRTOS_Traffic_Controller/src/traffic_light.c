/*
 * Traffic Light controller. 
 * 
 * The light callbacks signal when it's time to change a light. 
 * We use a mutex to ensure that only one function can access and 
 * modify the traffic light at any given time. 
 * 
 * Traffic light task: This task controls the timing of the traffic light. 
 * The timing is directly affected by the rate of flow of traffic. 
 *
 */


#include "traffic_light.h"


void vGreenLightTimerCallback( xTimerHandle xTimer ) 
{
	printf("Green light off, yellow light on. \n");
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN_PIN); 
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_YELLOW_PIN); 

	if( xSemaphoreTake( xMutexLight, ( TickType_t ) 0 ) == pdTRUE ) 
    {
		global_light_colour = 0; 
		xSemaphoreGive( xMutexLight ); 
		printf("Updated light colour to red. \n");
    }
	else
	{
		printf("xMutexLight unavailable when trying to change to green light. \n");
	}

	xTimerStart( xYellowLightSoftwareTimer, 0 ); 
}


void vYellowLightTimerCallback( xTimerHandle xTimer ) 
	{
	printf("Yellow light off, red light on. \n");
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_YELLOW_PIN); 
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_RED_PIN); 

	xTimerStart( xRedLightSoftwareTimer, 0 ); 
}


void vRedLightTimerCallback( xTimerHandle xTimer )
{
	printf("Red light off, green light on. \n");
	GPIO_ResetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_RED_PIN); 
	GPIO_SetBits(TRAFFIC_LIGHT_PORT, TRAFFIC_LIGHT_GREEN_PIN); 

	if( xSemaphoreTake( xMutexLight, ( TickType_t ) 0 ) == pdTRUE ) 
    {
		global_light_colour = 1;	
		xSemaphoreGive( xMutexLight ); 
		printf(" Updated light colour to green. \n");
    }
	else
	{
		printf("xMutexLight unavailable when trying to change to red light. \n");
	}

	xTimerStart( xGreenLightSoftwareTimer, 0 ); 
}


void TrafficLightTask ( void *pvParameters )
{

	// Flow rate ranges from 0 to 7. 
	uint16_t new_speed_value = 4;           // Set default speed value to 4. Update as the ADC is read.
	uint16_t current_speed_value = 0;       // Set to 0 to force an update of the timers. 

	while(1)
	{
		// Update local flow/speed variable
		if( xSemaphoreTake( xMutexFlow, ( TickType_t ) 10 ) == pdTRUE ) 
	    {
			new_speed_value = global_flowrate; 
			xSemaphoreGive( xMutexFlow ); 
			printf("Traffic Light Task: updated local flowrate:  %u.\n", new_speed_value );

			if (xQueueReceive( xFlowQueue, &flowrate, portMAX_DELAY ) ) // testing queue receive
			{	
				printf("Successfully received flowrate from QUEUE (traffic light task).\n");
			}
	    }
		else
		{
			printf("Traffic Light Task: xMutexFlow unavailable \n");
		}

		// The length of the light timers depends on the current flow rate. 
		if(current_speed_value !=  new_speed_value) 
		{
			if(xTimerIsTimerActive( xGreenLightSoftwareTimer ))
			{
				xTimerStop(xGreenLightSoftwareTimer, 0); 
				xTimerChangePeriod(xGreenLightSoftwareTimer, (5000 + 3000 * (8-new_speed_value))  / portTICK_PERIOD_MS, 0 );  
				xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 1500 * (8-new_speed_value)) / portTICK_PERIOD_MS, 0 ); 
				xTimerStop(xRedLightSoftwareTimer, 0); 
			}
			else if(xTimerIsTimerActive( xYellowLightSoftwareTimer ))
			{
				xTimerChangePeriod(xGreenLightSoftwareTimer, (5000 + 3000 * (8-new_speed_value))  / portTICK_PERIOD_MS, 0 ); 
				xTimerStop(xGreenLightSoftwareTimer, 0); 
				xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 1500 * (8-new_speed_value)) / portTICK_PERIOD_MS, 0 ); 
				xTimerStop(xRedLightSoftwareTimer, 0); 
			}
			else if(xTimerIsTimerActive( xRedLightSoftwareTimer ))
			{
				xTimerStop(xRedLightSoftwareTimer, 0);
				xTimerChangePeriod(xGreenLightSoftwareTimer, (5000 + 3000 * (8-new_speed_value))  / portTICK_PERIOD_MS, 0 ); 
				xTimerStop(xGreenLightSoftwareTimer, 0); 
				xTimerChangePeriod(xRedLightSoftwareTimer, (3000 + 1500 * (8-new_speed_value)) / portTICK_PERIOD_MS, 0 ); 
			}
		} 

		current_speed_value = new_speed_value; 
		vTaskDelay(100);
	}
} 
