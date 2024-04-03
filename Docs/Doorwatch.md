/*
	Doorwatch (german: Tuerwaechter)

	Information

	This AT90USB162 was on a PCB salvaged from a "Grundig Footswitch 540 USB",
 	which was part of a voice recorder.
 	I disassambled the PCB out of the housing and reversed engineered it.
 	The fully circuit layout will come later.
  	After that, i shrank the PCB with a saw for a better mobility.
   	With the smaller layout I lost the connector pads for PB4, PB5, PB6 and PB7.
    	So I had to open the trace to free the copper.

	- The AT90USB162 runs at 8 MHz
	- The connections are possible on PB4 PB5 PB6 PB7
 	  Other PINs will possible to, if you are able to solder SMD microcontrollers
	- All these ports use pull up resistors, no internal are needed
	- The At90USB162 runs with 3.3V
	- The proprietary connector is connected to D+ & D- can be used to power the circuit


	Functions

	The AVR controlls the door from the fridge.
	If the door is open, the AVR recognizes the new condition.
    	If the door is open for a longer time, there is an alarm.
     	There are 2 LEDs.
      	The green LED on PB 5 shows the operating status.
       	The red LED on PB 6 is used for the "door open" and "alarm".
	"Door open" flashes the red LED.
 	"Alarm" flashes green and red together.
  	If the door is closed the AVR goes to sleep mode.
   	Every pin change, wakes him up.


	Setup
 
	PB 4 - Reedcontact -> Is an opener, cause the door is constantly closed. If you need a closer you have to change the operator <!>.
	PB 5 - green LED
	PB 6 - red LED or buzzerSummer


 	Wishlist

	- Control doorwatch with a remote control - activate, deactivate, etc
  	- Mute alarm with a remote control
   	- Logging opening times on a CF Card
*/
