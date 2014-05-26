LEDDigitalCyrWheel
==================
This is my teensy 3.1 code for my LED cyr wheel v2 (by corbin dunn). I'm using individually addressable LEDs...so I called it "digital cyr wheel".

This project depends on a specific structure and some other projects. I use Mac OS; it may compile on other platforms, but I haven't tested it.

* Install Arduino; it must be installed in /Applications. I'm using 1.0.5
* Libraries I'm using. These MUST be installed in your Libraries location. To find that out, in Arduino.app open the preferences and see what the "Sketchbook location" says. Mine is set to '/corbin/Documents/Arduino'. My Libraries directory is then '/corbin/Documents/Arduino/Libraries'. Clone each library fork I am using into this directory:
* Libraries/Adafruit_NeoPixel: https://github.com/corbinstreehouse/Adafruit_NeoPixel
* Libraries/SdFat:
* Libraries/LSM303:
* Libraries/SD: (am I using this anymore?): 
* Libraries/L3G: 


* Optional: (TODO: upload my version): Libraries/OctoWS2811 -- much faster than NeoPixel, and originally my code could flip back and forth from using it and NeoPixel, but I haven't tested it in a while.
