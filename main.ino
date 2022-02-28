#include <avr/io.h>
#include <avr/interrupt.h>
#include <MIDIUSB_Defs.h>
#include <USB-MIDI.h>
#include <FastLED.h>
#include <Arduino.h>

//setup debugging flag to activate debugging output text to serial
#define DB_FLAG 1
//use template if(DB_FLAG) Serial.println();

//define PIN numbers
#define LED_PIN 8 //for LED data output
#define POT_PIN A0 //for decay time input
#define BUTTON_PIN 10 //for color changing input

//define LED object
#define NUM_LEDS 50 //based on number of chipsets in LED strip
#define LEDSTART 0 //define starting LED on the strip
#define LEDEND 49 //define ending LED on the strip
CRGB leds[NUM_LEDS];

//array of intensities; initializes all intensity values to be 0
int intensity[50]={0};

//values used for HUE in HSV color mixing; set by button through a cycle method
int colorPalette[] = {
    0, //red
    0, //amber
    0, //yellow
    0, //green
    0, //cyan
    0, //blue
    0, //magenta
    0, //pink
    0 //lavender
};

//color selector initializes to white
//range of 0 to 9 (9 is white)
int cs = 0;

//mapping function
int map(int key){
    //recieved a value between 21 and 108 based on MIDI note numbers
    //scale this to be 0 to 87
    key -= 21;
    //scale to be 0 to 50 to match LEDs
    key = key/NUM_LEDS;
    return key;
}

//decay step function (reads from POT and changes decay step based on POT input)

//interrupt for MIDI input
ISR(USART1_RX_vect){
    //ISR should filter out NOTE ONs with velocity > 0
    //MIDI.read();
    if(1){
        int velocity = 100; //midi byte 3 in
        int note = 42; //midi byte 2 in
        intensity[map(note)] = 2 * velocity;
    }
}

//second ISR changes the color (on pin change)
ISR(PCINT0_vect){
    if(cs>=9) //ensures button will loop through color array
        cs = 0;
    else //increments to next color in color array
        cs++;
}

//set up a timer to change values of intensity??

//things to send to another file for lights
void blackout() {
  for (int i = 0; i < 50; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

int main(void){
    //ALL SETUP CODE HERE

    //init interrupt which should trigger on pin change for button push (color changer)

    //init interrupt which should trigger on UART input via MIDI

    //init pin modes to setup input from one analog and one digital pin
    pinMode(POT_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT);

    //enable global interrupts
    sei();

    //setup LED strip for use
    FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS);

    //blackout the strip to erase any previous data
    blackout();

    //setup MIDI input
    //MIDI.begin(1); //listening to channel 1

    while(1){ //looping code
        //constantly write values to each LED chipset
        for(int i=0; i<50; i++){
            if (cs==9) //takes care of white case; colorPalette does not have a white setting
                leds[i]=CHSV(0,0,intensity[i]);
            else
                leds[i]=CHSV(colorPalette[cs],255,intensity[i]);
            FastLED.show();
        }
    }

    return 0;
}