/*
   d. bodnar - 9-06-2016
   Lightning & thunder for CHP castle
*/

#include <DFPlayer_Mini_Mp3.h>
#include <Adafruit_NeoPixel.h>


const int buttonPin = 3;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status
int buusyPin = 10;// buusyPin = 10; // sound player busy
int NUM_LEDS = 18;
int LED_PIN = 5;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRBW);

int sensorPin = A0;    // Audio level samples
int sensorValue = 0;  // variable to store the value coming from the sensor
int buttn = 0;
const int HIGH_STRIKE_LIKELIHOOD = 5;
const int LOW_STRIKE_LIKELIHOOD = 10;
int currentDataPoint = 0;
int chance = LOW_STRIKE_LIKELIHOOD;

// Simple moving average plot
int NUM_Y_VALUES = 35;

float yValues[] = {
  0,
  1,
  4.6,
  13,
  8,
  8.9,
  14,
  14,
  5,
  3,
  6.8,
  12,
  9,
  3.6,
  5.8,
  2,
  11,
  10.5,
  4.1,
  7,
  10,
  9,
  7.1,
  7.5,
  7.4,
  12,
  15,
  10,
  0,
  3,
  3.5,
  4,
  1,
  7,
  1
};

float simple_moving_average_previous = 0;
float random_moving_average_previous = 0;

float (*functionPtrs[10])(); //the array of function pointers
int NUM_FUNCTIONS = 2;


//***************************************SETUP*************************************
void setup () {
  pinMode(buttonPin, INPUT);
  pinMode(buusyPin, INPUT);
  pinMode(buttonPin, INPUT);
  Serial.begin (9600);
  mp3_set_serial (Serial);  //set Serial for DFPlayer-mini mp3 module
  mp3_set_volume (30);          // must remove mp3_reset(); to get this to work
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // initializes the array of function pointers.
  functionPtrs[0] = simple_moving_average;
  functionPtrs[1] = random_moving_average;
}

//.......................................LOOP................................................
void loop () {
  Serial.println("");
  Serial.println("Waiting for Button Push");
  do {
    buttn = digitalRead(buttonPin); // pins closest to power pins
  } while (buttn == 0);

  Serial.println("Button Hit");
  mp3_play(1);
  delay(100);
  if (random(chance) == 5) {
    int led = random(NUM_LEDS);
    for (int i = 0; i < 10; i++) {
      // Use this line to keep the lightning focused in one LED.
      // lightningStrike(led):
      // Use this line if you want the lightning to spread out among multiple LEDs.
      lightningStrike(random(NUM_LEDS));
    }
    // Once there's been one strike, I make it more likely that there will be a second.
    chance = HIGH_STRIKE_LIKELIHOOD;
  } else {
    chance = LOW_STRIKE_LIKELIHOOD;
  }
  turnAllPixelsOff();
  delay(250);
}

void turnAllPixelsOff() {
  for (int n = 0; n < NUM_LEDS; n++) {
    strip.setPixelColor(n, 0);
  }
  strip.show();
}

void lightningStrike(int pixel) {
  float brightness = callFunction(random(NUM_FUNCTIONS));
  float ScaledWhite = abs(brightness*500);
  
 
  
  
  strip.setPixelColor(pixel, strip.Color(ScaledWhite, ScaledWhite, ScaledWhite, ScaledWhite));
  strip.show();
  delay(random(1, 200));
  currentDataPoint++;
  currentDataPoint = currentDataPoint%NUM_Y_VALUES;
}

float callFunction(int index) {
  return (*functionPtrs[index])(); //calls the function at the index of `index` in the array
}

// https://en.wikipedia.org/wiki/Moving_average#Simple_moving_average
float simple_moving_average() {
  uint32_t startingValue = currentDataPoint;
  uint32_t endingValue = (currentDataPoint+1)%NUM_Y_VALUES;
  float simple_moving_average_current = simple_moving_average_previous + 
                                  (yValues[startingValue])/NUM_Y_VALUES - 
                                  (yValues[endingValue])/NUM_Y_VALUES;

  simple_moving_average_previous = simple_moving_average_current;
  return simple_moving_average_current;
}


// Same as simple moving average, but with randomly-generated data points.
float random_moving_average() {
  float firstValue = random(1, 10);
  float secondValue = random(1, 10);
  float random_moving_average_current = random_moving_average_previous +
                                  firstValue/NUM_Y_VALUES -
                                  secondValue/NUM_Y_VALUES;
  random_moving_average_previous = random_moving_average_current;

  return random_moving_average_current;
    
}

//...................................... END LOOP ........................................
