/*
09/24/2021
Case printed, circuit board done except adding flashing pins. Only took one prototype due to inverted camera image
Works!
ToDo
  1: Swap left and right in Gui - NOTE, Now looks like swap up/down not left/right
  2: Add OTA to sketch. Use Andreas's template putting ota in as separate file and add telnet debug - NOT WORKING, SEE NOTES
    a: https://github.com/SensorsIot/ESP32-OTA
  3: add Gui buttons and code for
    a: Full sweep left
    b: Full sweep right
    c: Full sweep up
    d: Full sweep down
    e: Continuouse sweep all directions while button pressed
    f: Look into adding circular control on gui

  4: Add tabs to Arduino editor for things like notes, passwords.h etc - DONE!
  5: Find out what port the camera is using and add port forwarding to Quacky. Look at using https and/or a different port
  6: Add "take/save pic" and "save clip" options/buttons
  7: Either add credentials.h file or WiFi manager for portability - Not really usefull for a couple deployments. Just recompile
    and upload
  
  8: Add turn flash on/off

 Wide Angle
  Wide Angle lenses seem to use a 90 degree orientation change
    1: Design and print new case and new circuit board - Is this really needed?

Andreas on OTA, telnet serial debugging, RTOS and more!
https://www.youtube.com/watch?v=1pwqS_NUG7Q

09/26
OTA and telnet not working well, so abandoning for now (comment out)
now it looks like I need to swap up/down and leave left/right alone
Swapped up/down - DONE!
*/

/*


 
Adafruit Arduino - Lesson 14. Sweep


#include <Servo.h> 

int servoPin = 9;
 
Servo servo;  
 
int angle = 0;   // servo position in degrees 
 
void setup() 
{ 
  servo.attach(servoPin); 
} 
 
 
void loop() 
{ 
  // scan from 0 to 180 degrees
  for(angle = 0; angle < 180; angle++)  
  {                                  
    servo.write(angle);               
    delay(15);                   
  } 
  // now scan back from 180 to 0 degrees
  for(angle = 180; angle > 0; angle--)    
  {                                
    servo.write(angle);           
    delay(15);       
  } 
} 
Servo motors are controlled by a series of pulses and to make it easy to use them, an Arduino library has been created so that you can just instruct the servo to turn to a particular angle.

The commands for using a servo are like built-in Arduino commands, but because you are not always going to be using a servo in your projects, they are kept in something called a library. If you are going to use commands in the servo library, you need to tell the Arduino IDE that you are using the library with this command:

KL - added "sweep up/down right/left buttons

M - attempt at adding rotate image functions with buttons on the web interface. Stolen from

https://diyprojects.io/5-tips-esp32-cam-fixed-ip-address-ap-mode-image-rotation-90-automatic-recovery-wifi-connection-html-code-storage/#.YW96YVXMJhE

N - attempt to add sliders from Rui's tutorial 
https://randomnerdtutorials.com/esp32-servo-motor-web-server-arduino-ide/

11/22/21
Pan_Tilt_SweeepP build off of PanTilt_SweepR_save NOT save2
attempt at building OTA back in as it seems to work, just takes a while

11/23/21
OTA working but it took a bit of hacking  - have to change boards.txt - follow this thread
https://arduino.stackexchange.com/questions/75198/why-doesnt-ota-work-with-the-ai-thinker-esp32-cam-board
 Synopsys, in C:\Users\MyUser\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\boards.txt

 save old board.txt and edit to
esp32cam.upload.maximum_size=3145728
esp32cam.build.partitions=huge_app
to

esp32cam.upload.maximum_size=1966080
esp32cam.build.partitions=min_spiffs
The various options are detailed in the same file with keys named esp32.menu.PartitionScheme., and for some boards these options are configurable be the user. To add this, remove (or comment out with a # character) the two lines found above so they look like:

# esp32cam.upload.maximum_size=3145728
# esp32cam.build.partitions=huge_app
and add the required menu selections for your application- e.g.

esp32cam.menu.PartitionScheme.huge_app=Huge APP (3MB No OTA/1MB SPIFFS)
esp32cam.menu.PartitionScheme.huge_app.build.partitions=huge_app
esp32cam.menu.PartitionScheme.huge_app.upload.maximum_size=3145728
esp32cam.menu.PartitionScheme.min_spiffs=Minimal SPIFFS (Large APPS with OTA)
esp32cam.menu.PartitionScheme.min_spiffs.build.partitions=min_spiffs
esp32cam.menu.PartitionScheme.min_spiffs.upload.maximum_size=1966080

close and restart IDE. It took me several times until it 'took'

11/24
Spacing and typesetting fixed with hack of adding multiple <br> </br> tags. Need to learn a css solution
Next
1 Change format so servo controls are on one line, camera controls (snap pic, save etc) on next line
2: For  step, add "press and hold" to step functions for continuous rotation
3: sweep might need a "center"
4: Add "long press" to step buttons and perhaps selector for how many steps for each press

 11/25 Pan_Tilt_SweepSB.ino: Attempts at adding continuous servo movement during mouse down event

 12/2 
 1: TelnetStream "kinda" working but for now resorting to using the ftdi for both power and serial but still using OTA for updating
    Also started looking at these librarys
    https://github.com/JoaoLopesF/RemoteDebug/blob/master/examples/RemoteDebug_Basic/RemoteDebug_Basic.ino

    https://github.com/LennartHennigs/ESPTelnet

    https://platformio.org/lib/show/6236/TelnetSpy/examples

    
 2: Added both serial and telnet printlns for tracking servo movement
 3: "hold down mousefunction" works but not detecting the mouse up event - suspect the while loop is blocking. May try yields or freetos
 4: Add "snapshot" button and SD functionality

12/4
1: Fixed init_servos which fixes home_servos with if/else < or > 90
2: defined servo_speed as the delay between servo steps. Currently set to 25

12/5
Seems to working nicely, changing ver to SBD
1: Trying to add Telnet functionality
  a: Uncommented line at top of .ino '#define ESP32_RTOS' and it still seems to be working
  though be me crashing intermittanlty - under watch.
  UPDATE: Yes, crashing, backtrace

  
  ecoding 16 results
0x40156537: __cxxabiv1::__terminate(void (*)()) at /builds/idf/crosstool-NG/.build/src/gcc-5.2.0/libstdc++-v3/libsupc++/eh_terminate.cc line 112
0x4008e414: invoke_abort at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp32/panic.c line 715
0x4008e68d: abort at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/esp32/panic.c line 715
0x40156537: __cxxabiv1::__terminate(void (*)()) at /builds/idf/crosstool-NG/.build/src/gcc-5.2.0/libstdc++-v3/libsupc++/eh_terminate.cc line 112
0x4015657e: std::terminate() at /builds/idf/crosstool-NG/.build/src/gcc-5.2.0/libstdc++-v3/libsupc++/eh_terminate.cc line 112
0x40155c57: __cxa_throw at /builds/idf/crosstool-NG/.build/src/gcc-5.2.0/libstdc++-v3/libsupc++/eh_throw.cc line 87
0x40155f8e: operator new(unsigned int) at /builds/idf/crosstool-NG/.build/src/gcc-5.2.0/libstdc++-v3/libsupc++/new_op.cc line 54
0x40155cf5: operator new[](unsigned int) at /builds/idf/crosstool-NG/.build/src/gcc-5.2.0/libstdc++-v3/libsupc++/new_opv.cc line 32
0x400d3355: WiFiUDP::parsePacket() at C:\Users\TrentBrown\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\cores\esp32/Udp.h line 46
0x400d4575: ArduinoOTAClass::handle() at C:\Users\TrentBrown\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\libraries\ArduinoOTA\src/ArduinoOTA.cpp line 110
0x400d1bce: loop() at C:\Data\Programming\ESP32\RUI\Trent\Pan_Tilt_SweepSBD/Pan_Tilt_SweepSBD.ino line 673
0x400d761d: loopTask(void*) at C:\Users\TrentBrown\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\cores\esp32/main.cpp line 23
0x4009031e: vPortTaskWrapper at /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/freertos/port.c line 355 (discriminator 1)

  b: Uncommented line '#define Telnet', video does not load but servos work fine. Have to upload
  new code soon after boot.
  c: Need to try other telnet libraries and/or post on Andreas's site questions and there were 
  issues posted there

 TODO:
 1: Add either "press and hold" or some type of slider
 2: Calm down the home function, its to active - FIXED with if/else </> 90
 3: Slow down the sweep fucnction - FIXED with #define servo_speed
 4: Add a "restart" button and cli option in
 5: Start adding menu to telnet debug, options may be
    a) restart (R)
    b) exit (E)
    c) change servo speed (S)
    D) take snapshot (S)

6:  Try adding rtos back in but actually use it. Start with each moter runing in separate
thread, make them move 'at the same time'. Could be fun.


 Thoughts:
Change init ServoxPos from 0 to 90, should be the min dist to travel - done
Look at using servoEasing (already installed so check examples), could relieve a lot jerkiness
 */
