#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the I2C address, columns (20), and rows (4)
LiquidCrystal_I2C lcd(0x27, 20, 4); // **CHANGE 0x27 IF YOUR ADDRESS IS DIFFERENT**

// Function to reverse a string
String reverseString(String str)
{
  int len = str.length();
  String reversed = "";
  for (int i = len - 1; i >= 0; i--)
  {
    reversed += str[i];
  }
  return reversed;
}

void printReverse(String message)
{
  int msgLen = message.length();
  int currentCol = 19; // Start at the last column
  int currentRow = 3;  // Start at the last row

  // Iterate backward through the message string
  for (int i = msgLen - 1; i >= 0; i--)
  {
    // 1. Set the cursor to the current position
    lcd.setCursor(currentCol, currentRow);

    // 2. Print the character
    lcd.print(message[i]);

    // 3. Move the cursor backward (decrement column)
    currentCol--;

    // 4. Check if the column is off the left edge (wraps around)
    if (currentCol < 0)
    {
      // Reset column to the right edge (19)
      currentCol = 19;

      // Move up one row
      currentRow--;

      // Stop if the row goes above the top edge (Row 0)
      if (currentRow < 0)
      {
        break;
      }
    }
  }
}

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

byte flip_bit_order(byte b)
{
  byte reversed_byte = 0;

  // Iterate 8 times (for 8 bits)
  for (int i = 0; i < 8; i++)
  {
    // 1. Left shift the reversed byte to make room for the new bit
    reversed_byte <<= 4;

    // 2. Check the Least Significant Bit (LSB) of the original byte:
    //    (b & 1) extracts the LSB (0 or 1).
    // 3. Use the bitwise OR operator to insert this bit into the LSB
    //    of the reversed_byte.
    reversed_byte |= (b & 1);

    // 4. Right shift the original byte to get the next bit in the next iteration
    b >>= 1;
  }
  return reversed_byte;
}

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200);
  pinMode(23, INPUT_PULLDOWN);
  pinMode(19, INPUT_PULLDOWN);
  pinMode(18, INPUT_PULLDOWN);
  pinMode(16, INPUT_PULLDOWN);
  pinMode(4, INPUT_PULLDOWN);

  // lcd.init();      // Initialize the LCD
  // lcd.backlight(); // Turn on the backlight

  // lcd.setCursor(0, 0); // Column 0, Row 0
  // lcd.print("ESP32 LCD Access");

  // lcd.setCursor(0, 1); // Column 0, Row 1
  // lcd.print("I2C Module Ready");

  // lcd.setCursor(0, 2); // Column 0, Row 2
  // lcd.print("20 Columns");

  // lcd.setCursor(0, 3); // Column 0, Row 3
  // lcd.print("4 Rows");

  // ------

  // lcd.init();
  // lcd.backlight();

  // String normalText = "Hello World!";
  // String flippedText = reverseString(normalText);

  // // Print normal text on the first row
  // lcd.setCursor(0, 0);
  // lcd.print(normalText);

  // // Print reversed (flipped) text on the second row
  // lcd.setCursor(0, 1);
  // lcd.print(flippedText); // Will display: !dlroW olleH

  // ------

  // lcd.init();
  // lcd.backlight();
  // lcd.clear(); // Clear the screen before starting

  // String targetMessage = "This message starts at (19,3) and ends at (0,0) across the 20x4 screen.";

  // // Call the function to print from (19,3) to (0,0)
  // printReverse(targetMessage);

  // ------

  lcd.init();
  lcd.backlight();
  // lcd.clear();

  lcd.createChar(0, flipped1);
  lcd.setCursor(0, 0);
  lcd.write(0);

  for (int i = 0; i < 8; i++)
  {
    byte temp = flipped1[i];
    flipped1[i] = flipped1[7 - i];
    flipped1[7 - i] = temp;
    i++;
    // delay(1000);
  }

  lcd.createChar(1, flipped1);
  lcd.setCursor(19, 3);
  lcd.write(1);

  for (int i = 0; i < 8; i++)
  {
    byte agent1 = B1;
    byte agent2 = B1;
    byte buffer = B0;

    for (int j = 0; j < 3; j++)
    {
      if (j < 2)
      {
        agent1 = (flipped1[i] & 1 << j) * pow(2, 4 - j * 2);
        agent2 = (flipped1[i] & 1 << 4 - j) * pow(2, 2 * j - 4);

        buffer |= (agent1 | agent2);
        Serial.printf("j = %d - LSB = %d * %d - MSB = %d * %d\nagent1 = %d\nagent2 = %d\nbuffer = %d\n--\n", j, (flipped1[i] & 1 << j), pow(2, 4 - j * 2), (flipped1[i] & 1 << 4 - j), pow(2, 2 * j - 4), agent1, agent2, buffer);
      }
      else if (j == 2)
      {
        buffer |= flipped1[i] & 1 << j;
        Serial.printf("buffer = %d\n---\n", buffer);
      }
      // Serial.printf(" j = %d - ", j);
      // Serial.println(agent2);
      // delay(100);
    }
    flipped1[i] = buffer;
  }

  // Load the custom character (flipped H) into CGRAM slot 0
  lcd.createChar(2, flipped1);

  // Set cursor to the last column (19) of the last row (3)
  lcd.setCursor(18, 3);

  // Print the custom character using the write function for slot 0
  lcd.write(2);

  // To truly flip a whole word diagonally, you would need to create
  // a custom character for every letter used and print them in reverse order.

  // ------

  // // Print the word "Temp: 25C" with a degree symbol (Code 223)
  // lcd.setCursor(0, 0);
  // lcd.print("Temp: 25");

  // // Print the degree sign using casting
  // lcd.print((char)223); // Sends code 223

  // lcd.print("C");
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
