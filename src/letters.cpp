#include <Arduino.h>
#include "CustomChars.h"

byte blankChar[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000};

byte flippedOne[8] = {
    B01100,
    B10100,
    B00100,
    B00100,
    B00100,
    B00100,
    B00100,
    B11111};

byte flippedHeitch[8] = {
    B10001,
    B10001,
    B10001,
    B11111,
    B10001,
    B10001,
    B10001,
    B10001};

byte flippedEe[8] = {
    B11111,
    B10000,
    B10000,
    B11111,
    B10000,
    B10000,
    B10000,
    B11111};

byte flippedeL[8] = {
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B11111};

byte flippedOw[8] = {
    B11111,
    B10011,
    B10101,
    B10101,
    B10101,
    B10101,
    B11001,
    B11111};

byte flippedExclamation[8] = {
    B01110,
    B01110,
    B01110,
    B01110,
    B00000,
    B00100,
    B01110,
    B00100};

// Custom Character 0
byte DECIMAL_0[8] = {
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B01110,
    B00000
};

// Custom Character 1
byte DECIMAL_1[8] = {
    B00100,
    B01100,
    B00100,
    B00100,
    B00100,
    B00100,
    B01110,
    B00000
};

// Custom Character 2
byte DECIMAL_2[8] = {
    B01110,
    B10001,
    B00001,
    B00010,
    B00100,
    B01000,
    B11111,
    B00000
};

// Custom Character 3
byte DECIMAL_3[8] = {
    B11111,
    B00001,
    B00001,
    B01110,
    B00001,
    B00001,
    B11111,
    B00000
};

// Custom Character 4
byte DECIMAL_4[8] = {
    B10001,
    B10001,
    B10001,
    B11111,
    B00001,
    B00001,
    B00001,
    B00000
};

// Custom Character 5
byte DECIMAL_5[8] = {
    B11111,
    B10000,
    B10000,
    B11110,
    B00001,
    B00001,
    B11110,
    B00000
};

// Custom Character 6
byte DECIMAL_6[8] = {
    B01110,
    B10001,
    B10000,
    B11110,
    B10001,
    B10001,
    B01110,
    B00000
};

// Custom Character 7
byte DECIMAL_7[8] = {
    B11111,
    B00001,
    B00001,
    B00010,
    B00100,
    B00100,
    B00100,
    B00000
};

// Custom Character 8
byte DECIMAL_8[8] = {
    B01110,
    B10001,
    B01110,
    B10001,
    B01110,
    B10001,
    B01110,
    B00000
};

// Custom Character 9
byte DECIMAL_9[8] = {
    B01110,
    B10001,
    B10001,
    B01111,
    B00001,
    B00001,
    B01110,
    B00000
};

void diagonalFlipChar(byte (&flip_arr)[8], bool type)
{

    for (int i = 0; i < 8; i++)
    {
        byte temp = flip_arr[i];
        flip_arr[i] = flip_arr[7 - i];
        flip_arr[7 - i] = temp;
        i++;
    }
    if (type == 1)
    {
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
                    // Serial.printf("j = %d - LSB = %d * %d - MSB = %d * %d\nagent1 = %d\nagent2 = %d\nbuffer = %d\n--\n", j, (flip_arr[i] & 1 << j), pow(2, 4 - j * 2), (flip_arr[i] & 1 << 4 - j), pow(2, 2 * j - 4), agent1, agent2, buffer);
                }
                else if (j == 2)
                {
                    buffer |= flip_arr[i] & 1 << j;
                    // Serial.printf("buffer = %d\n---\n", buffer);
                }
            }
            flip_arr[i] = buffer;
        }
    }
}