// code based on the colorwheel_32x32 demo from Adafruit RGBmatrixPanel library.
// Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
// for Adafruit Industries.
// BSD license, all text above must be included in any redistribution.

// this code powers "I can move on", an interactive poem by Yole Quintero,
// made as physical game by Yole Quintero and Ciro Duran.
// For the Global Game Jam 2019, at Hacksmiths, London.
// 25, 26, 27 January 2019

#include <RGBmatrixPanel.h>

// Most of the signal pins are configurable, but the CLK pin has some
// special constraints.  On 8-bit AVR boards it must be on PORTB...
// Pin 8 works on the Arduino Uno & compatibles (e.g. Adafruit Metro),
// Pin 11 works on the Arduino Mega.  On 32-bit SAMD boards it must be
// on the same PORT as the RGB data pins (D2-D7)...
// Pin 8 works on the Adafruit Metro M0 or Arduino Zero,
// Pin A4 works on the Adafruit Metro M4 (if using the Adafruit RGB
// Matrix Shield, cut trace between CLK pads and run a wire to A4).

//#define CLK  8   // USE THIS ON ARDUINO UNO, ADAFRUIT METRO M0, etc.
//#define CLK A4 // USE THIS ON METRO M4 (not M0)
#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

#define REGIONS 5
#define REGION_OFF 0
#define REGION_CORRECT 1
#define REGION_INCORRECT 2
#define REGION_END 3

// Regions in the RGB matrix that get turned on
const int regions[REGIONS][4] { { 0, 0, 10, 10 }, { 20, 0, 10, 10 }, { 0, 20, 10, 10 }, { 20, 20, 10, 10 }, { 10, 10, 10, 10 } };

const int part1Pin = 2;
const int part2Pin = 3;
const int part3Pin = 4;
const int part4Pin = 5;
const int part5Pin = 6;
//const int resetPin = 7;

int partStates[REGIONS] { REGION_OFF, REGION_OFF, REGION_OFF, REGION_OFF, REGION_OFF };

int currentPart = 0;

void setup() {

  Serial.begin(9600);
  
  pinMode(part1Pin, INPUT);
  pinMode(part2Pin, INPUT);
  pinMode(part3Pin, INPUT);
  pinMode(part4Pin, INPUT);
  pinMode(part5Pin, INPUT);

  matrix.begin();
}

void loop() {
  int i;
  uint16_t c;

  for (i=0; i < REGIONS; i++) {
    if (partStates[i] == REGION_OFF) {
      c = matrix.Color333(0, 0, 0);
    } else if (partStates[i] == REGION_CORRECT) {
      c = matrix.Color333(6, 7, 6);
    } else if (partStates[i] == REGION_INCORRECT) {
      c = matrix.Color333(3, 0, 0);
    } else if (partStates[i] == REGION_END) {
      c = matrix.Color333(3, 3, 7);
    }

    matrix.fillRect(regions[i][0], regions[i][1], regions[i][2], regions[i][3], c);
  }

  delay(100);

  readBoard();
}

void readBoard() {
  int pinReads [REGIONS] { 0, 0, 0, 0, 0 };
  int i;
  pinReads[0] = digitalRead(part1Pin);
  pinReads[1] = digitalRead(part2Pin);
  pinReads[2] = digitalRead(part3Pin);
  pinReads[3] = digitalRead(part4Pin);
  pinReads[4] = digitalRead(part5Pin);

  Serial.print("Parts: ");
  Serial.print(pinReads[0]);
  Serial.print(", ");
  Serial.print(pinReads[1]);
  Serial.print(", ");
  Serial.print(pinReads[2]);
  Serial.print(", ");
  Serial.print(pinReads[3]);
  Serial.print(", ");
  Serial.print(pinReads[4]);
  Serial.println(".");
  
  if (currentPart == 0) {
    if (!pinReads[1] && !pinReads[2] && !pinReads[3] && !pinReads[4]) {
      if (pinReads[0]) {
        partStates[0] = REGION_CORRECT;
        currentPart = 1;
      } else {
        partStates[0] = partStates[1] = partStates[2] = partStates[3] = partStates[4] = REGION_OFF;
      }
    } else if (!pinReads[1] || pinReads[2] || pinReads[3] || pinReads[4]) {
      partStates[0] = partStates[1] = partStates[2] = partStates[3] = partStates[4] = REGION_INCORRECT;
    }
  } else if (currentPart == 1) {
    if (pinReads[0] && !pinReads[2] && !pinReads[3] && !pinReads[4]) {
      if (pinReads[1]) {
        partStates[1] = REGION_CORRECT;
        currentPart = 2;
      } else {
        partStates[0] = REGION_CORRECT;
        partStates[1] = partStates[2] = partStates[3] = partStates[4] = REGION_OFF;
      }
    } else if (!pinReads[0] || pinReads[2] || pinReads[3] || pinReads[4]) {
      partStates[0] = partStates[1] = partStates[2] = partStates[3] = partStates[4] = REGION_INCORRECT;
    }
  } else if (currentPart == 2) {
    if (pinReads[0] && pinReads[1] && !pinReads[3] && !pinReads[4]) {
      if (pinReads[2]) {
        partStates[2] = REGION_CORRECT;
        currentPart = 3;
      } else {
        partStates[0] = partStates[1] = REGION_CORRECT;
        partStates[2] = partStates[3] = partStates[4] = REGION_OFF;
      }
    } else if (!pinReads[0] || !pinReads[1] || pinReads[3] || pinReads[4]) {
      partStates[0] = partStates[1] = partStates[2] = partStates[3] = partStates[4] = REGION_INCORRECT;
    }
  } else if (currentPart == 3) {
    if (pinReads[0] && pinReads[1] && pinReads[2] && !pinReads[4]) {
      if (pinReads[3]) {
        partStates[3] = REGION_CORRECT;
        currentPart = 4;
      } else {
        partStates[0] = partStates[1] = partStates[2] = REGION_CORRECT;
        partStates[3] = partStates[4] = REGION_OFF;
      }
    } else if (!pinReads[0] || !pinReads[1] || !pinReads[2] || pinReads[4]) {
      partStates[0] = partStates[1] = partStates[2] = partStates[3] = partStates[4] = REGION_INCORRECT;
    }
  } else if (currentPart == 4) {
    if (pinReads[0] && pinReads[1] && pinReads[2] && pinReads[3]) {
      if (pinReads[4]) {
        partStates[4] = REGION_CORRECT;
        currentPart = 5;
      } else {
        partStates[0] = partStates[1] = partStates[2] = partStates[3] = REGION_CORRECT;
        partStates[4] = REGION_OFF;
      }
    } else if (!pinReads[0] || !pinReads[1] || !pinReads[2] || !pinReads[3]) {
      partStates[0] = partStates[1] = partStates[2] = partStates[3] = partStates[4] = REGION_INCORRECT;
    }
  } else if (currentPart == 5) {
    partStates[0] = partStates[1] = partStates[2] = partStates[3] = partStates[4] = REGION_END;
  }
}
