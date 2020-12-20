# nanodds
DDS for SSB 6.1 transceiver based on Si5351A, ST7735 and ATMega1284p


## Current WIP

Memory
    - Save state
    - List cells
    - Load
    
    

## Planned function

Buttons

- Encoder
  * Short press - enable/disable RIT (transmits on the main frequency).
  * Long press
    * If encoder is rotated - change step of frequency
- TX
- Mode
  * Single press - switches type of modulation
  * Press & hold when is booting - opens Settings mainMenu 
  (use Encoder to navigate and change the values, 
  use Encoder button to activate the mainMenu item and rotate to change, 
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

### Programming

0. Build

```
pio run --target=upload -e Debug
```

1. Fuses

```bash

avrdude -v -pm1284p  -c avrispmkII -P usb:16:79 -B 10 -U lfuse:w:0x8f:m -U hfuse:w:0xd6:m -U efuse:w:0xfd:m

```

2. Actual software

```bash
avrdude -p m1284p -c avrispmkII -P usb:16:79 -e -U flash:w:.pio/build/Debug/firmware.elf:e -v -B 10
```


Logic
    State - active state
    AppSettings - global rarely updated changes


Per http://www.rfcafe.com/references/electrical/spectral-inv.htm
we have DDS always running above receiving RF signal therefore
`LO1 frequency = RF + IF` which results in spectral inversion.
To make sound undistorted we must perform second spectral inversion by setting LO2 at the upper edge of the filter.
