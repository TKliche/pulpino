/*
  wiring_analog.c - analog input and output
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  Modified 28 September 2010 by Mark Sproul

  $Id: wiring.c 248 2007-02-03 15:36:30Z mellis $
*/

#include "wiring_private.h"
#include "pins_arduino.h"

uint8_t PWM_count;
struct{
uint8_t PWM_enabled;
uint8_t PWM_value;
}PWM_status_arr[NUM_DIGITAL_PINS];



//AnalogRead is not existed in this version as PULPino currently doesn't have Analog to digital converter


//As there is no Hardware PWM in PULPino this is an implementation of software PWM with predetrmined frequency in Precesion
void analogWrite(uint8_t pin, int val)
{
	// We need to make sure the PWM output is enabled for those pins
	// that support it, as we turn it off when digitally reading or
	// writing with them.
	pinMode(pin, OUTPUT);
	if (val <= 0)
	{
		PWM_status_arr[pin].PWM_enabled=0;	
		digitalWrite(pin, LOW);
	}
	else if (val >= 255)
	{
		PWM_status_arr[pin].PWM_enabled=0;
		digitalWrite(pin, HIGH);
	}
	else
	{
		PWM_status_arr[pin].PWM_enabled=1;
		PWM_status_arr[pin].PWM_value=val>>PWM_PRE;
	}			
}



void ISR_TB_CMP(void) 
{
	uint8_t pin;
	if (PWM_count==PWM_MAX_COUNT)
	{
		for(pin=0; pin<NUM_DIGITAL_PINS; pin++)
			if(PWM_status_arr[pin].PWM_enabled)
				digitalWrite(pin, HIGH);
		PWM_count=0;	
	}
	else
	{
		for(pin=0; pin<NUM_DIGITAL_PINS; pin++)		
			if((PWM_status_arr[pin].PWM_enabled) && (PWM_status_arr[pin].PWM_value==PWM_count))
				digitalWrite(pin, LOW);
		PWM_count++;
	}

	ICP|= (1<<31);		//clear Timer B compare match interrupt pending
}

