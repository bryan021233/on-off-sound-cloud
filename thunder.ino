
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Adafruit_NeoPixel.h>

int NUM_LEDS = 18;
int LED_PIN = 5;
int rxPin = 10;    // DFplayer RX to Arduino pin 10
int txPin = 11;    // DFplayer TX toArduinopin 11
int busyPin = 12;  // DFplayer BUSY connected to pin 12
SoftwareSerial mySoftwareSerial(rxPin, txPin);
DFRobotDFPlayerMini myDFPlayer;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRBW);

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


void setup() 
{
 pinMode(busyPin, INPUT);

 { // Neopixel setup
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // initializes the array of function pointers.
  functionPtrs[0] = simple_moving_average;
  functionPtrs[1] = random_moving_average;
 }
 
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  Serial.println(F("Initializing DFPlayer..."));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin. Check connection and SD card, or reset the Arduino."));
    while (true);
  }

  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.setTimeOut(500);                   // Set serial communictaion time out 500ms
  myDFPlayer.volume(22);                        // Set volume value (0~30).
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);              // Set EQ to BASS (normal/pop/rock/jazz/classic/bass)
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);  // Set device we use SD as default
  myDFPlayer.enableDAC();                       // Enable On-chip DAC

}

void loop() {
  int thunderDelay = random (50, 500);  // Min. and max. delay between flashing and playing sound
  int thunderFile = random (1, 17);       // There are 17 soundfiles: 0001.mp3 ... 0017.mp3
  int loopDelay = random (500, 15000);   // Min. and max. delay between each loop
  
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
 Serial.print(F("Pausing before playing thunder sound, milliseconds: "));
  Serial.println(thunderDelay);
  delay(thunderDelay);

  Serial.print(F("Playing thunder sound, file number: "));
  Serial.println(thunderFile);
  myDFPlayer.playMp3Folder(thunderFile);
  delay(1000); // Give the DFPlayer some time

  while (digitalRead(busyPin) == LOW) { // Wait for the DFPlayer to finish playing the MP3 file
  }

  Serial.print(F("Pausing before next loop, milliseconds: "));
  Serial.println(loopDelay);
  delay(loopDelay);
}
