# tuerwaechter
Türwächter

/*	Information

	This AT90USB162 was on a PCB salvaged from a "Grundig Footswitch 540 USB", which was part of a voice recorder.
 	I disassambled the PCB out of the housing and reversed engineered it.
 	The fully circuit layout will come later.
  	After that i shrank the PCB with a saw for a better mobility.
   	With the smaller layout I lost the connector pads for PB4, PB5, PB6 and PB7.
    	So I had to open the trace to free the copper.

	- The AT90USB162 runs at 8 MHz
	- The connections are possible on PB4 PB5 PB6 PB7
	- All these ports use pull up resistors, no internal!
	- The At90USB162 runs with 3.3V
	- The proprietary connector is connected to D+ & D- and can be used to power the circuit


	Functions

	The AVR controlls the door from the fridge.
	If the door is open, the AVR recognizes the new condition.
    	If the door is open for a longer time, there is an alarm.
     	There are 2 LEDs.
      	One shows the operating status.
       	One is for "door open" and "alarm" flashes both LEDs.

 	- Extra 1: Energysaving and sleep mode
	- Extra 2: Counter, how often the door was open -> Display

	Aufbau
 
	PB 4 - Reedcontact -> Is an opener, cause the door is constantly closed. If you need a closer you have to change the operator <!>.
	PB 5 - green LED
	PB 6 - red LED or buzzerSummer
	
*/
