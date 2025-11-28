#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "customChars.h"
#include <OneButton.h>

// Set the I2C address, columns (20), and rows (4)
LiquidCrystal_I2C lcd(0x27, 20, 4); // **CHANGE 0x27 IF YOUR ADDRESS IS DIFFERENT**
OneButton butt(32, false, false); 

// --- Function Prototypes ---
void handleClick();

int bit_ctr = 0;
bool old_state = false;
int guessArray[4] = {0, 0, 0, 0};
bool enterBit = false;
int try_attempt = 0;

// --- Debounce Configuration ---
// Time (in ms) to ignore further signals after the initial interrupt.
const unsigned long debounceDelay = 500;

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
  if ((currentMillis - lastDebounceTime) > debounceDelay)
  {

    enterBit ^= 1;
  }
  // Reset the debounce timer
  lastDebounceTime = currentMillis;
}

// REPLACE WITH THE MAC Address of your receiver
// Receiver MAC: 3C:71:BF:FD:44:6C
// Receiver MAC: 78:E3:6D:11:26:30
// uint8_t broadcastAddress[] = {0x3C, 0x71, 0xBF, 0xFD, 0x44, 0x6C};
// uint8_t broadcastAddress[] = {0x78, 0xE3, 0x6D, 0x11, 0x26, 0x30};

// uint8_t broadcastAddress[] = {0x60, 0x55, 0xF9, 0x7B, 0xAC, 0x60}; // (1)
// uint8_t broadcastAddress[] = {0x60, 0x55, 0xF9, 0x7B, 0xA8, 0x74}; // (2)
// uint8_t broadcastAddress[] = {0x60, 0x55, 0xF9, 0x7B, 0x92, 0xD0}; // (3)
// uint8_t broadcastAddress[] = {0x60, 0x55, 0xF9, 0x7B, 0x91, 0xA0}; // (4)
// uint8_t broadcastAddress[] = {0x60, 0x55, 0xF9, 0x7B, 0x93, 0xF8}; // (5)
// uint8_t broadcastAddress[] = {0x60, 0x55, 0xF9, 0x7B, 0x9B, 0x40}; // (6)
// uint8_t broadcastAddress[] = {0x60, 0x55, 0xF9, 0x7B, 0xAC, 0xAC}; // (7)
uint8_t broadcastAddress[] = {0x60, 0x55, 0xF9, 0x7B, 0xAD, 0xD0}; // (9)

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
  int digit0;
  int digit1;
  int digit2;
  int digit3;
  int guess_status;

} struct_message;

// Variable to store if sending data was successful
String success;

// Define variables to store incoming readings
int incomingDigit[4] = {0, 0, 0, 0};
int incomingGuess_status;

// Create a struct_message to sendData
struct_message sendData;

// Create a struct_message to hold incoming data
struct_message incomingCode;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0)
  {
    success = "Delivery Success :)";
    lcd.setCursor(19, 0);
    lcd.print((char)B01011110); // '^'
  }
  else
  {
    success = "Delivery Fail :(";
    lcd.setCursor(19, 0);
    lcd.print("?");
  }
  try_attempt++;
  lcd.setCursor(9, 0);
  lcd.print("Try");

  lcd.setCursor(9, 1);
  lcd.print(try_attempt);
}

// Callback when data is received
int incoming_len = 0;
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&incomingCode, incomingData, sizeof(incomingCode));
  Serial.print("Bytes received: ");
  Serial.println(len);

  incoming_len = len;
  incomingDigit[0]= incomingCode.digit3;
  incomingDigit[1]= incomingCode.digit2;
  incomingDigit[2]= incomingCode.digit1;
  incomingDigit[3]= incomingCode.digit0;

  for (int i = 0; i < 4; i++) {
    if(incomingDigit[3-i] >= 0 && incomingDigit[3-i] <= 9){
      lcd.setCursor(4-i,1);
      lcd.print(incomingDigit[3-i]);
      Serial.print(incomingDigit[3-i]);
    }
    else{
      lcd.setCursor(4-i,1);
      lcd.print((char)(incomingDigit[3-i]));
      Serial.print(incomingDigit[3-i]);
    }
  }
  Serial.println();

  incomingGuess_status = incomingCode.guess_status;
}

void inputMatrixTranslte(bool enterPin, bool *arr){
  for (int i = 0; i < 5; i++)
  {
    if (i < 3)
    {
      if (arr[i] > 0)
      {
        lcd.setCursor(19 - i, 1);
        lcd.write(6);
      }
      else if (arr[i] == 0)
      {
        lcd.setCursor(19 - i, 1);
        lcd.print('0');
      }
    }
    else if (i == 3)
    {
      lcd.setCursor(19 - i, 1);
      lcd.print('-');
    }
    else if (i == 4)
    {
      if (enterPin > 0)
      {
        lcd.setCursor(19 - i, 1);
        lcd.write(6);
      }
      else if (enterPin == 0)
      {
        lcd.setCursor(19 - i, 1);
        lcd.print('0');
      }
    }
  }
}

void handleClick() {
  enterBit ^= 1;
}

void setup()
{

  Serial.begin(115200);
  delay(1000);
  
  Serial.println("BINUS CompEng's Wordle!");
  pinMode(23, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);

  pinMode(32, INPUT_PULLDOWN);
  butt.attachClick(handleClick);

  // attachInterrupt(digitalPinToInterrupt(32), handleInterrupt, FALLING);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    lcd.setCursor(9, 0);
    lcd.print("x");
    return;
  }

  lcd.setCursor(7, 0);
  lcd.printf("HELLO!");

  diagonalFlipChar(flippedHeitch, 1);
  lcd.createChar(0, flippedHeitch);
  lcd.setCursor(12, 1);
  lcd.write(0);

  diagonalFlipChar(flippedEe, 1);
  lcd.createChar(1, flippedEe);
  lcd.setCursor(11, 1);
  lcd.write(1);

  diagonalFlipChar(flippedeL, 1);
  lcd.createChar(2, flippedeL);
  lcd.setCursor(10, 1);
  lcd.write(2);

  lcd.setCursor(9, 1);
  lcd.write(2);

  diagonalFlipChar(flippedOw, 1);
  lcd.createChar(4, flippedOw);
  lcd.setCursor(8, 1);
  lcd.write(4);

  diagonalFlipChar(flippedExclamation, 1);
  lcd.createChar(5, flippedExclamation);
  lcd.setCursor(7, 1);
  lcd.write(5);

  diagonalFlipChar(flippedOne, 1);
  lcd.createChar(6, flippedOne);

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  delay(3000);
  lcd.clear();
}

void loop()
{
  // put your main code here, to run repeatedly:
  butt.tick();
  bool inBit[3] = {!digitalRead(18), !digitalRead(19), !digitalRead(23)};

  lcd.setCursor(0, 0);
  lcd.print(inBit[0]);

  lcd.setCursor(1, 0);
  lcd.print(inBit[1]);
  
  lcd.setCursor(2, 0);
  lcd.print(inBit[2]);

  lcd.setCursor(3, 0);
  lcd.print((char)B01111110); // '-'

  lcd.setCursor(4, 0);
  lcd.print(enterBit);
  
  // lcd.printf("%d%d%d-%d", inBit[0], inBit[1], inBit[2], enterBit);

  // print diagonally matrix-translate binary input digit
  // inputMatrixTranslte(enterBit, inBit);

// Convert Binary to Decimal->Show it to LCD->Send guess decimal guess array
  int code_digit = 4;

  // Blink cursor at number decimal entry
  lcd.setCursor(code_digit - bit_ctr, 1);
  lcd.cursor_on();
  lcd.blink_on();

  // If the input is entered or not
  if (enterBit != old_state)
  {
    if (bit_ctr < code_digit)
    {
      for (int j = 0; j < 3; j++)
      {
        guessArray[bit_ctr] += inBit[j] * pow(2, 2 - j);
        Serial.printf("iter = %d - inBit = %d - number = %d\n", bit_ctr, inBit[j], guessArray[bit_ctr]);
      }

      lcd.setCursor(0, 1);
      lcd.print(">");
      lcd.setCursor(code_digit - bit_ctr, 1);
      lcd.print(guessArray[bit_ctr]);

      bit_ctr++;
    }
    else if (bit_ctr == code_digit)
    {
      sendData.digit3 = guessArray[3];
      sendData.digit2 = guessArray[2];
      sendData.digit1 = guessArray[1];
      sendData.digit0 = guessArray[0];

      bit_ctr = 0;
      guessArray[0] = 0;
      guessArray[1] = 0;
      guessArray[2] = 0;
      guessArray[3] = 0;

      lcd.setCursor(0, 1);
      lcd.print(">");
      lcd.setCursor(4, 1);
      lcd.print((char)guessArray[3]);
      lcd.setCursor(3, 1);
      lcd.print((char)guessArray[2]);
      lcd.setCursor(2, 1);
      lcd.print((char)guessArray[1]);
      lcd.setCursor(1, 1);
      lcd.print((char)guessArray[0]);

      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&sendData, sizeof(sendData));

      if (result == ESP_OK)
      {
        Serial.println("Sent with success");
      }
      else
      {
        Serial.println("Error sending the data");
      }
    }

    old_state = enterBit;
    Serial.printf("ctr = %d - xor_state = %d\n", bit_ctr, enterBit);
    Serial.printf(">%d%d%d%d\n", guessArray[3], guessArray[2], guessArray[1], guessArray[0]);
  }

  if(incoming_len > 0){
    lcd.setCursor(19,1);
    lcd.print(incomingCode.guess_status);
    if (incomingCode.guess_status == 4){
      try_attempt = 0;
    }
    incoming_len = 0;
  }

  delay(100);
}
