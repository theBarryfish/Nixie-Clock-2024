#include <Arduino.h>
#include <pinDefines.h>

 #define nixieDataPin 32
 #define nixieLatchPin 27
 #define nixieClockPin 33


/*
Code to drive the old Dirk & Barry Nixie board that has 3ea 32 bit shift register pin drivers
They are connected to xxYY MM DD HH MM SS nixies where only the used digits are connected and occupy register bits
as in 0 to 59 seconds the S10 digit only connects to 0,1,2,3,4 & 5 nixie digit selector bit


number of bits as utilized:   MSb is Month 10's
Y  Y   M  M   D  D   H  H   M  M   S  S
0  0   2 10   4 10   4 10   6 10   6 10
*/

int seconds = 0;
int minutes = 0;
int hours = 0;
int days = 1;
int months = 1;
int years = 25;



//-------------------------------
void sendDigitPair(int digitData)
//-------------------------------
{
    int tempData = 0;
    tempData = 1<<((digitData / 10)+10);                                // convert the tens data to the digit selector bit and shift left to the upper 6 bits of the word (move 2 bits for the upper byte plus 8 for the lower byte)
    tempData = tempData + (1<<(digitData % 10));                        // or-in the lower 10 bits
    shiftOut(nixieDataPin, nixieClockPin, MSBFIRST, tempData>>8);       // move the upper byte down and shift it out
    shiftOut(nixieDataPin, nixieClockPin, MSBFIRST, tempData & 255);    // shift out the low byte
}

//-------------------------------
void incTime()              // TERRIBLE !!!  FIX ME leap year, DST, Time zone, yada
//-------------------------------
{
seconds ++;
 if (seconds >= 59) 
 { 
    seconds = 0;
    minutes ++;
    if (minutes >=59)
    {
        minutes = 0;
        hours ++;
        if (hours >= 24) 
        {
            hours = 0;
            months ++;
            if (months >= 13)
            {
                months = 1;
            }
        }
    }
 }
}

//-------------------------------
void updateTimeDisplay()
//-------------------------------
{
    digitalWrite(nixieLatchPin, LOW); //disable data from registers to outputs as long as you are transmitting
    sendDigitPair(years);   // must be in the range of 0-59    
    sendDigitPair(months);   // must be in the range of 0-59
    sendDigitPair(days);   // must be in the range of 0-59
    sendDigitPair(hours);   // must be in the range of 0-59
    sendDigitPair(minutes);   // must be in the range of 0-59
    sendDigitPair(seconds);   // must be in the range of 0-59
    digitalWrite(nixieLatchPin, HIGH); //strobe the data stored in the latches to the outputs
}




//                         /$$                        
//                        | $$                        
//    /$$$$$$$  /$$$$$$  /$$$$$$   /$$   /$$  /$$$$$$ 
//   /$$_____/ /$$__  $$|_  $$_/  | $$  | $$ /$$__  $$
//  |  $$$$$$ | $$$$$$$$  | $$    | $$  | $$| $$  \ $$
//   \____  $$| $$_____/  | $$ /$$| $$  | $$| $$  | $$
//   /$$$$$$$/|  $$$$$$$  |  $$$$/|  $$$$$$/| $$$$$$$/
//  |_______/  \_______/   \___/   \______/ | $$____/ 
//                                          | $$      
//                                          | $$      
//                                          |__/      
                    

void setup() {
    Serial.begin(115200);
Serial.println("starting \n");    

    pinMode(LED_pin,OUTPUT);

    pinMode(nixieDataPin, OUTPUT);          // sets the digital pin 13 as output
    pinMode(nixieLatchPin, OUTPUT);
    pinMode(nixieClockPin, OUTPUT);

    digitalWrite(nixieDataPin,LOW);         // pins to shift registers
    digitalWrite(nixieLatchPin,LOW);  
    digitalWrite(nixieClockPin,LOW);

    digitalWrite(nixieLatchPin,HIGH);       // Latch high so the data goes straight to the outputs

    for (int i=0; (i < 97); i++)            // loop to clear all of the output bits
    {
     digitalWrite(nixieClockPin,HIGH);
     delayMicroseconds(1);
     digitalWrite(nixieClockPin,LOW);   
     delayMicroseconds(1);
    }




}

//   /$$                              
//  | $$                              
//  | $$  /$$$$$$   /$$$$$$   /$$$$$$ 
//  | $$ /$$__  $$ /$$__  $$ /$$__  $$
//  | $$| $$  \ $$| $$  \ $$| $$  \ $$
//  | $$| $$  | $$| $$  | $$| $$  | $$
//  | $$|  $$$$$$/|  $$$$$$/| $$$$$$$/
//  |__/ \______/  \______/ | $$____/ 
//                          | $$      
//                          | $$      
//                          |__/      
//                                    

void loop() {

digitalWrite(LED_pin, !digitalRead(LED_pin));  //toggle LED
delay(20);
incTime();
updateTimeDisplay();

}

