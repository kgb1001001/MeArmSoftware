# MeArmSoftware
Arduino software for the MeArm robot arm (using an Arduino Wifi Shield and AdaFruit PWM Servo Board)

![Mearm build with Arduino Shields](./images/MeArmBuildwithArduinoShields.jpg)

One of the frustrations of using the open source [MeArm robot arm](https://www.thingiverse.com/thing:360108) is the lack of support of software for Arduino -- the community as a whole seems to have gravitated toward Raspberry Pi as the default choice for controlling the MeArm -- however for the last several months Raspberry Pis have been as scarce as hens teeth (there are literally sniping bots donated by the community that grab them off of sites like Amazon and Adafruit as soon as they appear in the online catalogs as available).  Meanwhile, Arduinos are readily available, but the Arduino community itself has fractured and spread, with the result being that there are literally dozens of boards, form factors and combinations of these.

I have been trying for several weeks to put together a simple REST-based server for the boards that I had available - a standard [Arduino Uno Rev3](https://store-usa.arduino.cc/products/arduino-uno-rev3), an [Arduino Wifi Shield](https://www.sparkfun.com/products/retired/11287) and the [Adafruit PWM Servo shield](https://www.adafruit.com/product/1411?gclid=Cj0KCQiA7bucBhCeARIsAIOwr--_LZ_OsnZebVt89jlr0gecEERaLyTYsC1oAsb4bOj-4omWjSup22YaAlG3EALw_wcB)

I finally managed to pull it all together - along the way also trying other combinations that I will eventually post into this repository, including replacing the Arduino Uno and Wifi Shield with the currently supported Uno Wifi Rev 2 (which, of course, uses a different chipset, and thus an entirely different set of libraries).

The project was built using libraries and resources from several sources.  The most notable include

- aREST - the REST library for Arduino by Marco Schwartz (https://github.com/marcoschwartz/aREST)
- The Arduino PWM library for the Adafruit PWM servo shield

**Using the software**

Simply open the .ino file in the Arduino editor (either the web version or the isntallable version), edit the file to include your Wifi SSID and password, select the right board type (Arduino Uno) and port and upload the file.

**Tweaking the operation**

Each build of the MeArm is different, with different placement and travel of the servos.  You will need to play with the range of motion on each servo and tweak the fixed parameters for the upper and lower ranges of the four servos (BASE_SERVOMIN, BASE_SERVOMAX, GRIPPER_SERVOMIN, GRIPPER_SERVOMAX, XSERVOMIN, XSERVOMAX, YSERVOMIN, and YSERVOMAX).  These parameters are supposedly from 0 to 4096, but in practice I've only seen them vary between 10 and 400.
