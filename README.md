# tuerwaechter
Türwächter

/*	Information

	This AT90USB162 was on a PCB salvaged from a "Grundig Footswitch 540 USB", which was part of a voice recorder.
 	I disassampled  the PCB and reversed engineered it.
 	The fully circuit layout will come later.

	- The AT90USB162 runs at 8 MHz
	- The connections are possible on PB4 PB5 PB6 PB7
	- All these ports use pull up resistors, no internal!
	- The At90USB162 runs with 3.3V
	- The proprietary connector is connected to D+ & D- and can be used to power the circuit


	Funktionen

	- Der AVR soll als Kühlschrankwächter fungieren.
	  Er überprüft ob die Kühlschranktür geöffnet ist
	  und wenn sie eine längere Zeit offen bleibt, geht ein Alarm los,
	- Die LED dient einzig und allein als Statusanzeige.
	  Blinkt alle 10 Sekunde -> Betriebsbereit
	  Blinkt alle  1 Sekunde -> Alarm
   
	- Extra 1: Stromsparmodus soll aktiviert werden damit er alle 10 Sekunden einschläft und den Kontakt überprüft

	- Extra 2: Speicherung der Anzahl, wie oft die Tür geöffnet wurde. 2 mal kurz, Tür öffnen

	Aufbau
 
	PB 4 - Reedkontakt -> Ist ein Öffner kein Schließer!!! Für einen Schließer wird der Operator ! benötigt
	PB 5 - LED
	PB 6 - Summer
	
*/
