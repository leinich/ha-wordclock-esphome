#include "esphome.h"
#include <FastLED.h>

#define NUM_LEDS 125
#define DATA_PIN D5

// esphome dependencies:
// needs: esphome time --> id: current_time
// needs: esphome fastled --> id: fastledlight

// Layout of Clock (X not used chars) - PIN Layout
// ESXISTXFÜNF    1.......11
// ZEHNZWANZIG    22......12
// DREIVIERTEL    23......33
// XXNACHVORXX    44......34
// HALBXZWÖLFX    45......55
// ZWEIXSIEBEN    ....
// XDREIXXFÜNF    ....
// ELFNEUNVIER    ....
// XACHTZEHNRS    ....
// XSECHSXXUHR    109.......101
// ICONS          110.......120
// MINUTES        124.......121

int leds_time_it_is[] = {0, 1, 3, 4, 5}; // ES IST
int leds_minutes[] = {124, 123, 122, 121}; // Minutes LEDS
int leds_skiped[] = {110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120}; // Check Line 101
int leds_time_minutes[][15] = {
    {101, 100,  99,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // UHR
    {  7,   8,   9,  10,  41,  40,  39,  38,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // FÜNF, NACH
    { 21,  20,  19,  18,  41,  40,  39,  38,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // ZEHN, NACH
    { 26,  27,  28,  29,  30,  31,  32,  41,  40,  39,  38,  -1,  -1,  -1,  -1}, // VIERTEL, NACH
    { 17,  16,  15,  15,  14,  13,  12,  11,  41,  40,  39,  38,  -1,  -1,  -1}, // ZWANZIG, NACH
    {  7,   8,   9,  10,  37,  36,  35,  44,  45,  46,  47,  -1,  -1,  -1,  -1}, // FÜNF, VOR, HALB
    { 44,  45,  46,  47,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // HALB
    {  7,   8,   9,  10,  41,  40,  39,  38,  44,  45,  46,  47,  -1,  -1,  -1}, // FÜNF, NACH, HALB
    { 17,  16,  15,  15,  14,  13,  12,  11,  37,  36,  35,  -1,  -1,  -1,  -1}, // ZWANZIG, VOR
    { 22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  -1,  -1,  -1,  -1}, // DREIVIERTEL
    { 21,  20,  19,  18,  37,  36,  35,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // ZEHN, VOR
    {  7,   8,   9,  10,  37,  36,  35,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}  // FÜNF, VOR
};

int leds_time_hours[][6] = {
    { 49,  50,  51,  52,  53,  -1}, // ZWÖLF
    { 63,  62,  61,  60,  -1,  -1}, // EINS
    { 65,  64,  63,  62,  -1,  -1}, // ZWEI
    { 67,  68,  69,  70,  -1,  -1}, // DREI
    { 80,  79,  78,  77,  -1,  -1}, // VIER
    { 73,  74,  75,  76,  -1,  -1}, // FÜNF
    {108, 107, 106, 105, 104,  -1}, // SECHS
    { 60,  59,  58,  57,  56,  55}, // SIEBEN
    { 89,  90,  91,  92,  -1,  -1}, // ACHT
    { 84,  83,  82,  81,  -1,  -1}, // NEUN
    { 93,  94,  95,  96,  -1,  -1}, // ZEHN
    { 87,  86,  85,  -1,  -1,  -1}  // ELF
};

CRGB leds[NUM_LEDS];
int hour = -1;
int minute = -1;
int red = 124;
int green = 124;
int blue = 124;
int brightness = 50;

class Wordclock : public Component, public CustomAPIDevice {
    public:
        void setup() override {
            FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
            FastLED.setBrightness(brightness);
            // Start all LED with on and default color and brightness to check if everything is working...
            for(int i = 0; i < NUM_LEDS; i++) { leds[i].setRGB(red, 0, 0); FastLED.show(); delay(10); }
            for(int i = 0; i < NUM_LEDS; i++) { leds[i].setRGB(0, green, 0); FastLED.show(); delay(10); }
            for(int i = 0; i < NUM_LEDS; i++) { leds[i].setRGB(0, 0, blue); FastLED.show(); delay(10); }
            for(int i = 0; i < NUM_LEDS; i++) { leds[i].setRGB(0, 0, 0); }
            FastLED.show();
            register_service(&Wordclock::on_setled, "setled", {"number","red", "blue", "green"});
        }
        void on_setled(int number, int red, int blue, int green) {
            if (number < NUM_LEDS || number > 0) {
                ESP_LOGD("setled", "Setting led number %d to color %i %i %i", number, red, green, blue );
                leds[number].setRGB(red, green, blue);
                FastLED.show();
                }
            else { ESP_LOGE("setled", "Not a valid LED Number - out of range"); }
        }
        void loop() override {
            auto time = id(current_time).now();
            int h = time.hour;
            int m = time.minute;
            // https://www.esphome.io/api/classesphome_1_1light_1_1_light_color_values.html LightColorValues Class
            auto fastledlight2 = id(fastledlight).current_values;
            //convert float 0.0 till 1.0 into int 0 till 255
            red = (int)(fastledlight2.get_red()*255);
            green = (int)(fastledlight2.get_green()*255);
            blue = (int)(fastledlight2.get_blue()*255);
            brightness = 0;
            //check if light is on and set brightness
            if (fastledlight2.get_state() > 0 ) { brightness = (int)(fastledlight2.get_brightness()*255); }
            else { ESP_LOGD("loop", "fastledlight state off - b: %i rgb %i %i %i", brightness, red, green, blue); delay(500);}
            FastLED.setBrightness(brightness);
            //check if valid time. Blink red,green,blue until valid time is present
            if (time.is_valid() == false) {
                ESP_LOGE("loop", "Got invalid time from current_time Time: %i:%i", h, m );
                leds[0].setRGB(255, 0, 0); FastLED.show(); delay(250);
                leds[0].setRGB(0, 255, 0); FastLED.show(); delay(250);
                leds[0].setRGB(0, 0, 255); FastLED.show(); delay(250);
                leds[0].setRGB(0, 0, 0);   FastLED.show();
            }
            else {
                // only update once in a Minute
                if(h != hour || m != minute) {
                   //ESP_LOGD("loop", "Using b: %i rgb %i %i %i", brightness, red, green, blue);
                    hour = h;
                    minute = m;
                    if (hour >= 0 && time.is_valid() == true){
                        int tmp_hour = hour;
                        int tmp_minute = (minute - (minute % 5));
                        if(tmp_minute >= 25) { tmp_hour += 1; }
                        tmp_minute = tmp_minute / 5;
                        tmp_hour = tmp_hour % 12;
                        int minutessum = minute % 5;
                        // Reset all LED, but skip LED 110 till 120
                        for(int i = 0; i < NUM_LEDS; i++) {     if(i < 110 || i > 120) leds[i].setRGB(0, 0, 0); }
                        for(int i = 0; i < 5; i++) {            leds[leds_time_it_is[i]].setRGB(red, green, blue); }
                        for(int i = 0; i < 15; i++) {           if(leds_time_minutes[tmp_minute][i] >= 0) { leds[leds_time_minutes[tmp_minute][i]].setRGB(red, green, blue); } }
                        for(int i = 0; i < 6; i++) {            if(leds_time_hours[tmp_hour][i] >= 0) { leds[leds_time_hours[tmp_hour][i]].setRGB(red, green, blue); } }
                        for(int i = 0; i < minutessum; i++) {   leds[leds_minutes[i]].setRGB(red, green, blue);}
                        FastLED.show();
                        ESP_LOGD("loop", "Update Time: %i:%i  Brightness: %i RGB: %i-%i-%i", hour, minute, brightness, red, green, blue);
                        ESP_LOGD("loop", "Using tmp_hour: %i tmp_minute: %i minutessum: %i", tmp_hour, tmp_minute, minutessum);
                    }
                }
            }
        }
};
