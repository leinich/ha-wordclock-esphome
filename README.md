# ha-wordclock-esphome
## General
This project is just for documentation purposes and our attempt (together with a friend) to build an Wordclock, which is thightly bound to homeassistant usiung esphome.
It uses the time and fastled plattform of esphome to controll the clock.
### Features:
- Get time from HA instance
- Controll color and brightness from ha
- Controll status leds from ha with service calls
- uses esphome for OTA/API

![Image of final wordclock](https://github.com/leinich/ha-wordclock-esphome/blob/main/images/Front.png?raw=true)

## Shopping list:
- Mirror 30x30 (IKEA LOTS - 7.99EUR/4Pack)
- Woodframe
- Wood for Back and inlay
- ESP8266
- 125x WS2812B LEDS
- Glue, wires and solder

## Maschines needed: (All files are attached in ZIP Package)
- Laser engraver

## Installation:
- Mount the LED Stripes to the backplate and solder them based on following layout:
```
     ESXISTXFÜNF    1.......11
     ZEHNZWANZIG    22......12
     DREIVIERTEL    23......33
     XXNACHVORXX    44......34
     HALBXZWÖLFX    45......55
     ZWEIXSIEBEN    ....
     XDREIXXFÜNF    ....
     ELFNEUNVIER    ....
     XACHTZEHNRS    ....
     XSECHSXXUHR    109.......101
     ICONS          110.......120
     MINUTES        124.......121
```
![Image of final wordclock](https://github.com/leinich/ha-wordclock-esphome/blob/main/images/Back_leds.png?raw=true)

- Connect Ground and 5V and Data Pin of WS2812 to D5 Pin to the ESP8266
- Copy wordclock.yaml and wordclock.h into /config/esphome directory 
- Using esphome GUI you can flash wordclock.yaml to an esp8266

## Home Assistant integration:
Add the esphome to your Home Assistant instance.
You should now see the Dummy Light "Wordclock", which allows you to controll the brightness and color of the Wordclock.
Additionaly there is a service esphome.wordclock_setled, which allow you to set color to individual LEDS (e.g. the status icons)
This Service takes: number (PIN Number of LED, see above), red (0-255), green (0-255), blue (0-255)
```
service: esphome.wordclock_setled
data:
  number: 115
  green: 0
  red: 255
  blue: 0
```
