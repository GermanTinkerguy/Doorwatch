Project Repository Structure

Hardware    — contains the KiCAD Project, Schematics & Layout and Project Libraries
Firmware    — contains any software developed for the Hardware
Docs        — contains any data sheets or documentations
Production  — contains the gerber files, BOM or anything required by the fabrication houses
Simulation  — contains any simulation files and generated results
CAD         — contains the 3D models and mechanical designs for enclosures or support

***

Information:
A doorwatch using a salvaged AT90USB162.
The Status LED at PIN 5 is flashing every second.
If the reedcontact at PIN 4 is open, the alarm LED at PIN 6 will flash.
If the reedcontact stays close, the Controller will fall asleep in the last part.
Because of the activated PIN Interrupt at all PIN B the controller will wake up, if the reedcontact is open.

***

Functions:
- Code written in english.

***

To Do's:
- Optimization of the code.

***

Known Issues:
- The microcontroller doesn't react to a PCINT directly after falling asleep because of the PIN float. It needs 3-4 seconds until it realizes the PCINT.
- Change delay to millis.
