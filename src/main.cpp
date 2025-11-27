/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <time.h> // Include for proper use of time(0) and srand

// REPLACE WITH THE MAC Address of your receiver
// Receiver MAC: 08:3A:F2:52:80:B0 (from user's file)
uint8_t broadcastAddress[] = {0x08, 0x3A, 0xF2, 0x52, 0x80, 0xB0};

// Structure example to send data
// Must match the receiver structure
// We repurpose digit0-digit3 to send back the 4 feedback codes (0, 1, or 2)
typedef struct struct_message
{
  int digit0;   // Feedback code for guess position 3
  int digit1;   // Feedback code for guess position 2
  int digit2;   // Feedback code for guess position 1
  int digit3;   // Feedback code for guess position 0
  int guess_status; // Total correct position count (Greens)
} struct_message;

// Variable to store if sending data was successful
String success;

// Define variables to store incoming readings
int incomingDigit[4] = {0, 0, 0, 0};
int randomNumber[4] = {0, 0, 0, 0};

int incoming_len = 0;
int check_ctr = 0;

// Create a struct_message to sendData
struct_message sendData;

// Array to hold the feedback codes (0, 1, or 2)
// 2: Green (Correct Digit, Correct Position)
// 1: Yellow (Correct Digit, Wrong Position)
// 0: Gray (Incorrect Digit)
int feedbackCode[4] = {0, 0, 0, 0};

// Create a struct_message to hold incoming data
struct_message incomingCode;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  // Do nothing or log status
  (void)mac_addr;
  (void)status;
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  incoming_len = len;

  memcpy(&incomingCode, incomingData, sizeof(incomingCode));

  // Note: Maintaining user's original backward mapping for array indexing
  incomingDigit[3] = incomingCode.digit0;
  incomingDigit[2] = incomingCode.digit1;
  incomingDigit[1] = incomingCode.digit2;
  incomingDigit[0] = incomingCode.digit3;
  
  // Print Sender MAC
  Serial.print("Sender MAC Address: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", mac[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  Serial.printf("Guess >> %d%d%d%d\n---\n", incomingDigit[0], incomingDigit[1], incomingDigit[2], incomingDigit[3]);
}

/**
 * @brief Implements the Wordle-like two-pass matching logic.
 * * @param guess The incoming guess array (4 digits)
 * @param code The secret random number array (4 digits)
 * @param feedback The output array to store the result codes (0, 1, or 2)
 * @return int The total number of correct position matches (Greens)
 */
int checkGuess(const int guess[4], const int code[4], int feedback[4]) {
    // Local temporary arrays to track which digits have been 'used'
    // This prevents a digit in the code from matching multiple digits in the guess.
    int tempCode[4];
    int tempGuess[4];
    int greenCount = 0;

    // Initialize temporary arrays and feedback array (default to 0/Gray)
    for (int i = 0; i < 4; i++) {
        tempCode[i] = code[i];
        tempGuess[i] = guess[i];
        feedback[i] = 0; // Default to Gray
    }

    // --- PASS 1: Find Exact Matches (GREEN = 2) ---
    for (int i = 0; i < 4; i++) {
        if (tempGuess[i] == tempCode[i]) {
            feedback[i] = 2; // Green
            greenCount++;
            
            // Mark both as consumed (e.g., set to -1, a value outside 0-9)
            tempGuess[i] = -1;
            tempCode[i] = -1;
        }
    }

    // --- PASS 2: Find Partial Matches (YELLOW = 1) ---
    for (int i = 0; i < 4; i++) {
        // Only check digits that weren't an exact match (i.e., not consumed in Pass 1)
        if (feedback[i] == 0) { // Still Gray
            for (int j = 0; j < 4; j++) {
                // Check if the guess digit matches any unconsumed code digit
                if (tempGuess[i] == tempCode[j] && tempGuess[i] != -1) {
                    feedback[i] = 1; // Yellow (Correct Digit, Wrong Position)
                    
                    // Mark the matched code digit as consumed to enforce frequency limits
                    tempCode[j] = -1; 
                    
                    // Break the inner loop since the match is found
                    break;
                }
            }
        }
    }

    return greenCount;
}


void setup()
{
  // Init Serial Monitor
  Serial.begin(115200);

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
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  // Initialize random number generator and create the secret 4-digit code (0-9)
  srandom(time(0));
  for (int i = 0; i < 4; i++) {
      randomNumber[i] = random(8); // Generates a number from 0 to 9
  }

  Serial.printf("\n\nTo start things off = %d%d%d%d\n\n", randomNumber[0], randomNumber[1], randomNumber[2], randomNumber[3]);

  delay(2000);
}

void loop()
{
  if (incoming_len > 0)
  {
    // --- 1. Process the guess using the Wordle logic ---
    // The check_ctr will now hold the total count of 'Green' matches
    check_ctr = checkGuess(incomingDigit, randomNumber, feedbackCode);

    // --- 2. Prepare the feedback packet ---
    // We send the 4 feedback codes (0, 1, or 2) back to the receiver.
    // Maintaining the user's original backward mapping for the send packet:
    sendData.digit0 = feedbackCode[3]; // Feedback for position 3
    sendData.digit1 = feedbackCode[2]; // Feedback for position 2
    sendData.digit2 = feedbackCode[1]; // Feedback for position 1
    sendData.digit3 = feedbackCode[0]; // Feedback for position 0

    sendData.guess_status = check_ctr; // Total 'Green' matches

    Serial.printf("Feedback Codes (0=Gray, 1=Yellow, 2=Green) -> %d%d%d%d | Correct Positions: %d/4\n", 
                  sendData.digit3, sendData.digit2, sendData.digit1, sendData.digit0, sendData.guess_status);

    // --- 3. Send message via ESP-NOW ---
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&sendData, sizeof(sendData));
    
    if (result != ESP_OK) {
        Serial.println("Error sending feedback.");
    }

    // --- 4. Check for Win Condition ---
    if (check_ctr == 4)
    {
      Serial.println("Congrats! Generating new code...");
      delay(1000);
      
      // Generate new code (now using all 10 digits 0-9)
      srandom(time(0));
      for (int i = 0; i < 4; i++) {
          randomNumber[i] = random(8);
      }
      
      Serial.printf("\n\nNew CODE = %d%d%d%d\n\n", randomNumber[0], randomNumber[1], randomNumber[2], randomNumber[3]);
    }
    
    // --- 5. Reset flags for next guess ---
    incoming_len = 0;
    // check_ctr is already reset by the checkGuess() return value
  }
  delay(100);
}