/*
 * Functions for controlling the shift registers. 
 * 
 */

#include "shift_register.h"


void SR_PreLight( uint16_t value )
{
	if (value == 0) GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_1_PIN);	    
	else GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_1_PIN);        
	GPIO_SetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);        
	GPIO_ResetBits(SHIFT_REG_1_PORT, SHIFT_REG_CLK_1_PIN);
}


void SR_PostLight( uint16_t value )
{
	if (value == 0) GPIO_ResetBits(SHIFT_REG_2_PORT, SHIFT_REG_2_PIN);	    
	else GPIO_SetBits(SHIFT_REG_2_PORT, SHIFT_REG_2_PIN);        
	GPIO_SetBits(SHIFT_REG_2_PORT, SHIFT_REG_CLK_2_PIN);       
	GPIO_ResetBits(SHIFT_REG_2_PORT, SHIFT_REG_CLK_2_PIN);     
}
