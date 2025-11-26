#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the I2C address, columns (20), and rows (4)
LiquidCrystal_I2C lcd(0x27, 20, 4); // **CHANGE 0x27 IF YOUR ADDRESS IS DIFFERENT**

// Byte array for a custom character (Flipped 'H' across diagonal)
byte flippedH[8] = {
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    B10001,
    B10001};

byte blankChar[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000};

byte flipped1[8] = {
    B01100,
    B10100,
    B00100,
    B00100,
    B00100,
    B00100,
    B00100,
    B11111};

void diagonalFlipChar(byte (&flip_arr)[8]){

  for (int i = 0; i < 8; i++)
  {
    byte temp = flip_arr[i];
    flip_arr[i] = flip_arr[7 - i];
    flip_arr[7 - i] = temp;
    i++;
  }

  for (int i = 0; i < 8; i++)
  {
    byte agent1 = B1;
    byte agent2 = B1;
    byte buffer = B0;

    for (int j = 0; j < 3; j++)
    {
      if (j < 2)
      {
        agent1 = (flip_arr[i] & 1 << j) * pow(2, 4 - j * 2);
        agent2 = (flip_arr[i] & 1 << 4 - j) * pow(2, 2 * j - 4);

        buffer |= (agent1 | agent2);
        Serial.printf("j = %d - LSB = %d * %d - MSB = %d * %d\nagent1 = %d\nagent2 = %d\nbuffer = %d\n--\n", j, (flip_arr[i] & 1 << j), pow(2, 4 - j * 2), (flip_arr[i] & 1 << 4 - j), pow(2, 2 * j - 4), agent1, agent2, buffer);
      }
      else if (j == 2)
      {
        buffer |= flip_arr[i] & 1 << j;
        Serial.printf("buffer = %d\n---\n", buffer);
      }
    }
    flip_arr[i] = buffer;
  }
}

void setup()
{

  Serial.begin(115200);
  pinMode(23, INPUT_PULLDOWN);
  pinMode(19, INPUT_PULLDOWN);
  pinMode(18, INPUT_PULLDOWN);
  pinMode(16, INPUT_PULLDOWN);
  pinMode(4, INPUT_PULLDOWN);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.createChar(0, flipped1);
  lcd.setCursor(0, 0);
  lcd.write(0);

  diagonalFlipChar(flipped1);

  lcd.createChar(1, flipped1);
  lcd.setCursor(19, 3);
  lcd.write(1);

}

void loop()
{
  // put your main code here, to run repeatedly:
  // Serial.printf("%d%d%d%d->%d\n",digitalRead(23),digitalRead(19),digitalRead(18),digitalRead(16),digitalRead(4));
  // byte a = B1;
  // a <<= 1;
  // Serial.println(a);
  // delay(1000);
}
