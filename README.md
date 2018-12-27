# nanodds
DDS for SSB 6.1 transceiver based on Si5351A, ST7735 and ATMega1284p

## Planned function

Buttons

- Encoder
  * Short press - enable/disable RIT (transmits on the main frequency).
  * Long press
    * If encoder is rotated - change step of frequency
- TX
- Mode
  * Single press - switches type of modulation
  * Press & hold when is booting - opens Settings menu 
  (use Encoder to navigate and change the values, 
  use Encoder button to activate the menu item and rotate to change, 
  second click will return focus)

- VFO
  * Short press - switches between A/B VFO
  * Long press - enables SPLIT. Allows to transmit on alternative frequency. For example if you are receiving at VFO A, you will be transmitting at VFO B frequency.
- MEM
  * Short press - opens menu with saved frequencies (select the frequency to load)
  * Long press - opens menu with cells to save     
- BAND
 * Short press - cycles through list of bands
 *  
- STEP
 * Changes step from 1 Hz to 1 MHz in power of 10th. 
 
Watch demo on Youtube
 
[![DDSVIDEO](http://img.youtube.com/vi/lzqhjerMn1Q/0.jpg)](https://www.youtube.com/playlist?list=PLTqbxXPJuZrR78shjXX4a-I0lrquxQZ_2)

## Dependencies

* https://github.com/PaulStoffregen/Encoder
* https://github.com/adafruit/Adafruit-ST7735-Library
* https://github.com/etherkit/Si5351Arduino
