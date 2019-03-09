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
- BAND
  * Short press - cycles through list of bands
  *  
- STEP
  * Changes step from 1 Hz to 1 MHz in power of 10th. 

- MEM (MENU)

```
Use MEM to exit w/o Save *1s
--
[Memory]
    Save selectedActionIndex as new cell
    [3.760300, LSB]
        Load
        Overwrite with Current
            Y/N
        Erase
            Y/N
    Erase All
        N/Y
    <<
[CW]
    Words Per Minute <50>
    Iambic Keyer      [x]
    Mode              <A> 
    Reverse Pads      < *- | -* >    
    CW Tone         <800>
    <<
[SWR]
    Analyze Band
    <<
[Settings]
    Intermediate Frequency
        <8000000> MHz
    SSB Offset
        <1400> Hz
    DDS Calibration
        <-0> Hz
    [SWR Calibration]
        Set 50 Ohm point
        Set 100 Ohm point
        <<
    [Language]
        English
        Русский (Russian)
        Espaniol (Spanish)
        <<
    [Pano]
        <Whole Band|Part of the Band>
        Swipe size
            +/- <10> kHz
        <<
    Allow TX after startup []
    
    <<
About
<<    
```

### Ideas
    
SWR Analyse should scan the band with minimal output power and plot the SWR graph.
Display Max, Min SWR & Frequencies.

Support of multiple languages. 

 
Watch demo on Youtube
 
[![DDSVIDEO](http://img.youtube.com/vi/lzqhjerMn1Q/0.jpg)](https://www.youtube.com/playlist?list=PLTqbxXPJuZrR78shjXX4a-I0lrquxQZ_2)

## Dependencies

* PlatformIO.org
* https://github.com/PaulStoffregen/Encoder
* https://github.com/adafruit/Adafruit-ST7735-Library
* https://github.com/etherkit/Si5351Arduino
