LEDDigitalCyrWheel
==================
This is my teensy 3.1 code for my LED cyr wheel v2 (by corbin dunn). I'm using individually addressable LEDs...so I called it "digital cyr wheel".

This project depends on a specific structure and some other projects. I use Mac OS; it may compile on other platforms, but I haven't tested it.

* Install Arduino; it must be installed in /Applications. I'm using 1.0.5

* Update submodules by doing this from the terminal:
 git submodule init
 git submodule update


* Optional: (TODO: upload my version): Libraries/OctoWS2811 -- much faster than NeoPixel, and originally my code could flip back and forth from using it and NeoPixel, but I haven't tested it in a while.


Once you have that setup, you should be able to open LEDDigitalCyrWheel.xcodeproj. To compile (see if that works first) select Build from the drop down menu and see if it compiles. Just select that target in the drop down in the toolbar and hit cmd-b to build.  If it doesn't, send me the errors and screen shots.

To upload, select Upload and hit cmd-b; this will launch the Teensy uploader, and you should see it uploading code. I usually just use Upload as it also builds.

The file to be aware of is Makefile. APP_LIBS_LIST are the Arduino built in libraries I use, for instance:
APP_LIBS_LIST = SPI Wire Wire/utility EEPROM

And you can tweak it if you use other libraries. Normally the Arduino IDE figures this out for you, but w/this Xcode project setup you have to do it yourself.

The other line to edit is USER_LIBS_LIST. It references the libraries in Ardunio/Libraries that I'm using. It says this currently:

USER_LIBS_LIST = Adafruit_NeoPixel SDFat SDFat/utility SD L3G LSM303

If I wanted to flip to 


