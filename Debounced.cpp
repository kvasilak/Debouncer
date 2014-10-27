/*
  Debouncer.c- Debounced Digital inputs
  Copyright (c) 2013 Keith Vasilakes.  All right reserved.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
 
 /*
 Why a big ol uint32?
uint16 rolls over in 1 minute, way too short
uint32 rolls over in 49 days, pretty long, should cover everything
The best way is t do a thought experiment or excel spreadsheet showing what happens when you roll over using the method shown.
but it's like this...

Timer is at fff7
you want to wait F ms
Using this timeout function;
(using 16 bit unsigned for simplicity)

period is f
start is fff7

current is fff7
current - start is 0 which is  < f

current is ffff
current - start is 8 which is  < f

current is 0000
current - start is 9 which is  < f

current is 0006
current - start is f which is  == f

current is 0007
current - start is 10 which is  > f

current is 0008
current - start is 11 which is  > f
*/

#include <Arduino.h> 
#include "Debounced.h"

//return true if it's been period ms since start
static bool IsTimedOut(unsigned long period, unsigned long start)
{
  unsigned long current = millis();
  return((current - start) > period);
}

Debounced::Debounced() :
PinLevel(0),
PinNumber(-1),
DebounceStart(0),
State(CHECKING)
{
}

//connect and read the initial values
void Debounced::attach(int pin)
{
	PinNumber = pin;
	PinLevel = digitalRead(pin);
}

//update the current pin level
//call in the loop before using any IO
void Debounced::Update()
{
	int newlevel;
	
	if(PinNumber <0)
	{
		return; //assign me a pin Dude!
	}

	switch(State)
	{
	case CHECKING:
		newlevel = digitalRead(PinNumber);

		if(newlevel != PinLevel)
		{
			DebounceStart = millis();
			State = BOUNCING;
		}
		break;

	case BOUNCING:
		if(IsTimedOut(10, DebounceStart) )
		{
			//check again
			newlevel = digitalRead(PinNumber);

			//still different, update it
			if(newlevel != PinLevel)
			{
				PinLevel = newlevel;

				State = CHECKING;
			}
		}
	}
}
