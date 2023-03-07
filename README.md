## Lab Stirrer Controller

### [PlatformIO] Heating Informer for a laboratory stirrer on based Arduino UNO.

·Temperature sensor:
    DS18B20, resistor 10kΩ, pin 4.  Lcd view first line: 1)Last temperature; 2)Heating-cooling speed.
    
·Rotate sensor:
    Hall sensor 44E, resistor 4,7kΩ, pin 2. Lcd view second line: 1)the last rotation time in ms; 2) Average rotation per second.
    
·Rgb led:
    Blue - updating temperature. Green - cooling, Red - heating.
    
<img src="https://github.com/RomanSereda/LabMixerController/blob/master/Sketch.png" width="500">

View of the screen when the mixer is working:

<img src="https://github.com/RomanSereda/LabMixerController/blob/master/LCDView.jpg" width="300">
