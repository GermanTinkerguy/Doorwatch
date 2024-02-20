# tuerwaechter
Türwächter

/*	Information

	- The AT90USB162 runs at 8 MHz
	- The connections are possible on PB4 PB5 PB6 PB7
	- All these ports use pull up resistors
	- The At90USB162 runs with 3.3V
	- The proprietary connector is connected to D+ & D- and can be used to power the circuit


	Funktionon

	Der AVR soll als Kühlschrankwächter fungieren.
	Er überprüft ob die Kühlschranktür geöffnet ist
	und wenn sie eine längere Zeit offen bleibt, geht ein Alarm los,
	Die LED dient einzig unddn allein als Statusanzeige.
	Blinkt alle 10 Sekunde -> Betriebsbereit
	Blinkt alle  1 Sekunde -> Alarm

	Extra 1: Stromsparmodus soll aktiviert werden damit er alle 10 Sekunden einschläft und den Kontakt überprüft

	Extra 2: Speicherung der Anzahl, wie oft die Tür geöffnet wurde. 2 mal kurz, Tür öffnen

	PB4 Reedkontakt
	PB5 LED
	PB6	Summer
	
*/
