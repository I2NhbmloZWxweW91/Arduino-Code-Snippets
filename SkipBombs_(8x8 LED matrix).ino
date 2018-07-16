/*
 * Created By : HD Dananjaya (@_hddananjaya)
 * Github     : https://github.com/I2NhbmloZWxweW91
 * Date       : 16 July 2018
 * License    : DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE (https://en.wikipedia.org/wiki/WTFPL)
 */
 
#include "LedControl.h"
#include <binary.h>


/* 8x8 Matrix controlling */
LedControl lc = LedControl(12, 10, 11, 1); // Pins: DIN,CLK,CS, # of Display connected
unsigned long delayTime = 400; // Delay between Frames


/* Setup multitasking stuff */
#define buttonsCycle 50U
#define bombCycle 100U

unsigned long buttonsLastMillis = 0;
unsigned long bombLastMillis = 0;

boolean buttonsState = false;
boolean bombState = false;

/* Define Buttons */
const int pinButt1 = 2;
const int pinButt2 = 4;
int butt1Status = 0;
int butt2Status = 0;

/* Only one time */
bool clearMeBitch = true;

/* 1,2,3,4 vals for each track */
int carPosition;

/* Bomb indexing */
int colBombIndex = -1;

/* Make bomb positions */
int ledsInRow;
int bombsPositions[4];

/* Delay to restart the game when over */
bool waitABit = false;

/* to avoid press and hold */
int butt1LastValue = LOW;
int butt2LastValue = LOW;


/*
*   Multitasking 
*/
boolean cycleCheck(unsigned long *lastMillis, unsigned int cycle)
{
  unsigned long currentMillis = millis();
  if (currentMillis - *lastMillis >= cycle)
  {
    *lastMillis = currentMillis;
    return true;
  }
  else
    return false;
}


/* 
*   Draw car LEDs according to current position of it 
*/
void setCarLeds(int currentPosition) {

  Serial.println(currentPosition);

  if (clearMeBitch) {

    for (int clearIndex = 0; clearIndex < 8; clearIndex++) {

      lc.setLed(0, 6, clearIndex, false);
      lc.setLed(0, 7, clearIndex, false);

    }
    clearMeBitch = false;
  }

  switch (currentPosition) {
    case 1:
      lc.setLed(0, 6, 0, true);
      lc.setLed(0, 6, 1, true);
      lc.setLed(0, 7, 0, true);
      lc.setLed(0, 7, 1, true);
      break;
    case 2:
      lc.setLed(0, 6, 2, true);
      lc.setLed(0, 6, 3, true);
      lc.setLed(0, 7, 2, true);
      lc.setLed(0, 7, 3, true);
      break;
    case 3:
      lc.setLed(0, 6, 4, true);
      lc.setLed(0, 6, 5, true);
      lc.setLed(0, 7, 4, true);
      lc.setLed(0, 7, 5, true);
      break;
    case 4:
      lc.setLed(0, 6, 6, true);
      lc.setLed(0, 6, 7, true);
      lc.setLed(0, 7, 6, true);
      lc.setLed(0, 7, 7, true);
      break;
  }
}



/*
*   Start Animation
*/
void startGameBoy() {
  for (int count = 0; count < 30; count++) {
    lc.setRow(0, random(0, 8), random(0, 255));
    delay(random(25, 100));
  }
  delay(2000);
}



/* Main Setup */
void setup() {

  Serial.begin(9600);

  /* set buttons */
  pinMode(pinButt1, OUTPUT);
  pinMode(pinButt2, OUTPUT);

  /* speaker */
  pinMode(8, OUTPUT);

  lc.shutdown(0, false); // Wake up displays
  lc.setIntensity(0, 5); // Set intensity levels
  lc.clearDisplay(0);  // Clear Displays

  /* start animation */
  startGameBoy();
  delay(1000);
  lc.clearDisplay(0);
  
  /* when init */
  carPosition = 2;
  setCarLeds(carPosition);
  delay(2500);

}




/* Main loop */
void loop() {

  /*
  *   Event one to trigger
  */
  if (cycleCheck(&buttonsLastMillis, buttonsCycle)) {

    /* Read buttons input */
    butt1Status = digitalRead(pinButt1);
    butt2Status = digitalRead(pinButt2);

    /* If button 1 pressed (ignore press and hold)*/
    if (butt1Status == HIGH && butt1LastValue != butt1Status) {
      /* tone xD */
      tone(8, 2000, 50);
      
      carPosition -= 1;
      if (carPosition < 1) carPosition = 1; // to avoid being exceed the range
      clearMeBitch = true;
      setCarLeds(carPosition);
      butt1LastValue = HIGH;


    } else if (butt1Status == LOW ) butt1LastValue = LOW;

    /* If button 2 pressed (ignore press and hold)*/
    if (butt2Status == HIGH && butt2LastValue != butt2Status) {
      /* tone xD */
      tone(8, 2000, 50);

      carPosition += 1;
      if (carPosition > 4) carPosition = 4; // to avoid being exceed the range
      clearMeBitch = true;
      setCarLeds(carPosition);
      butt2LastValue = HIGH;

    } else if (butt2Status == LOW) butt2LastValue = LOW;


    /* how game can be over */
    if (colBombIndex == 6)
      for (int index=0; index<4; index++)
        if (bombsPositions[index] == 1 && index == carPosition-1) {
          startGameBoy();
          lc.clearDisplay(0);
          waitABit =true;
        }
        
    buttonsState = !buttonsState;
  }


  /*
  *   Event two to trigger
  */
  if (cycleCheck(&bombLastMillis, bombCycle)) {
    //colBombIndex = 33; //for debuging
    
    if (colBombIndex == -1) { // generate new bomb line

      if (waitABit){ // if game restarted
        delay(1000);
        waitABit = false;
      }

      bool correctLineGenerated = false;

      while (!correctLineGenerated) { // need to get a correct bomb line!

        /* assign bombs randomly */
        for (int index=0; index<4; index++)
          bombsPositions[index] = random(0, 2);
        
   
        ledsInRow = bombsPositions[0] * 192 + bombsPositions[1] * 48 + bombsPositions[2] * 12 + bombsPositions[3] * 3;
        int p = bombsPositions[0] * 192;
        int q = bombsPositions[1] * 48;
        int r = bombsPositions[2] * 12;
        int s = bombsPositions[0] * 3;

        /* Remove useless posibilities */
        if (p + q + r + s != 0 && p + q + r + s != 255) correctLineGenerated = true;

      }

    }

    /* Jump bomb line to next row */
    colBombIndex++;

    /* Turn on leds according to ledsInRow */
    lc.setRow(0, colBombIndex, ledsInRow);
    lc.setRow(0, colBombIndex + 1, ledsInRow);

    /* Turn off previous ones */
    lc.setRow(0, colBombIndex - 1, B00000000);
    lc.setRow(0, colBombIndex, B00000000);

    /* When bomb line arrived to end need to start a new bomb line ;))000 */
    if (colBombIndex == 7) colBombIndex = -1;

    setCarLeds(carPosition);

  }

  bombState = !bombState;
}

