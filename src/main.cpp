#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "customChars.h"

// Set the I2C address, columns (20), and rows (4)
LiquidCrystal_I2C lcd(0x27, 20, 4); // **CHANGE 0x27 IF YOUR ADDRESS IS DIFFERENT**

int bit_ctr = 0;
bool old_state = false;
int guessArray[4] = {0, 0, 0, 0};
bool enterBit = false;

// --- Debounce Configuration ---
// Time (in ms) to ignore further signals after the initial interrupt.
const unsigned long debounceDelay = 100; 

// Variable to track the last time the interrupt was successfully handled
// Must be volatile as it's modified within the ISR
volatile unsigned long lastDebounceTime = 0; 

/**
 * @brief Toggles the LED state whenever the interrupt pin state changes.
 * * IMPORTANT: ISRs must be fast and contain minimal code. Avoid 'delay()'
 * and excessive computation. Use the 'volatile' keyword for shared variables.
 */
void IRAM_ATTR handleInterrupt()
{
    // Read the current time inside the ISR
  unsigned long currentMillis = millis();
  
  // Check if enough time has passed since the last valid interrupt
  if ((currentMillis - lastDebounceTime) > debounceDelay) {
    
    enterBit ^= 1;
  }
      // Reset the debounce timer
    lastDebounceTime = currentMillis;
}

void setup()
{

  Serial.begin(115200);
  pinMode(23, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);

  pinMode(32, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(32), handleInterrupt, FALLING);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(7, 1);
  lcd.printf("HELLO!");

  diagonalFlipChar(flippedHeitch, 1);
  lcd.createChar(0, flippedHeitch);
  lcd.setCursor(12, 2);
  lcd.write(0);

  diagonalFlipChar(flippedEe, 1);
  lcd.createChar(1, flippedEe);
  lcd.setCursor(11, 2);
  lcd.write(1);

  diagonalFlipChar(flippedeL, 1);
  lcd.createChar(2, flippedeL);
  lcd.setCursor(10, 2);
  lcd.write(2);

  lcd.setCursor(9, 2);
  lcd.write(2);

  diagonalFlipChar(flippedOw, 1);
  lcd.createChar(4, flippedOw);
  lcd.setCursor(8, 2);
  lcd.write(4);

  diagonalFlipChar(flippedExclamation, 1);
  lcd.createChar(5, flippedExclamation);
  lcd.setCursor(7, 2);
  lcd.write(5);

  diagonalFlipChar(flippedOne, 1);
  lcd.createChar(6, flippedOne);

  delay(1000);
}

void loop()
{
  // put your main code here, to run repeatedly:

  bool inBit[3] = {!digitalRead(18), !digitalRead(19), !digitalRead(23)};

  lcd.setCursor(0, 0);
  lcd.printf("%d%d%d-%d", inBit[0], inBit[1], inBit[2], enterBit);

  for (int i = 0; i < 5; i++)
  {
    if (i < 3)
    {
      if (inBit[i] > 0)
      {
        lcd.setCursor(19-i, 3);
        lcd.write(6);
      }
      else if (inBit[i] == 0)
      {
        lcd.setCursor(19-i, 3);
        lcd.print('0');
      }
    }
    else if (i == 3)
    {
      lcd.setCursor(19-i, 3);
      lcd.print('-');
    }
    else if (i == 4)
    {
      if (enterBit > 0)
      {
        lcd.setCursor(19-i, 3);
        lcd.write(6);
      }
      else if (enterBit == 0)
      {
        lcd.setCursor(19-i, 3);
        lcd.print('0');
      }
    }
  }

  if (enterBit != old_state)
  {
    if (bit_ctr < 3)
    {
      for (int j = 0; j < 3; j++)
      {
        guessArray[bit_ctr] += inBit[j] * pow(2, 2 - j);
        Serial.printf("iter = %d - inBit = %d - number = %d\n", bit_ctr, inBit[j], guessArray[bit_ctr]);
      }

      
      lcd.setCursor(0,1);
      lcd.print(">");
      lcd.setCursor(3-bit_ctr,1);
      lcd.printf("%d",guessArray[bit_ctr]);

      bit_ctr++;
    }
    else if(bit_ctr == 3){
      bit_ctr = 0;
      guessArray[0] = 0;
      guessArray[1] = 0;
      guessArray[2] = 0;

      lcd.setCursor(0,1);
      lcd.print(">");
      lcd.setCursor(3,1);
      lcd.printf("%d",guessArray[2]);
      lcd.setCursor(2,1);
      lcd.printf("%d",guessArray[1]);
      lcd.setCursor(1,1);
      lcd.printf("%d",guessArray[0]);
    }

    old_state = enterBit;
    Serial.printf("ctr = %d - xor_state = %d\n", bit_ctr, enterBit);
    Serial.printf(">%d%d%d\n", guessArray[2],guessArray[1],guessArray[0]);
  }
  
  delay(100);
}
