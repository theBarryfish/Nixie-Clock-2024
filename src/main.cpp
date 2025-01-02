#include <Arduino.h>
#include <pinDefines.h>
#include "stdio.h"
#include <NTPClient.h>

#include "WiFi.h"
#include <WiFiUdp.h>

#include <Time.h>
//struct tm timeinfo;

/*
Code to drive the old Dirk & Barry Nixie board that has 3ea 32 bit shift register pin drivers
They are connected to xxYY MM DD HH MM SS nixies where only the used digits are connected and occupy register bits
as in 0 to 59 seconds the S10 digit only connects to 0,1,2,3,4 & 5 nixie digit selector bit


number of bits as utilized:   MSb is Month 10's
Y  Y   M  M   D  D   H  H   M  M   S  S
0  0   2 10   4 10   4 10   6 10   6 10
*/

// choose your time zone from this list
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define MY_TZ "EST5EDT,M3.2.0,M11.1.0"

 #define nixieDataPin 32
 #define nixieLatchPin 27
 #define nixieClockPin 33



int x=0;
String strX;

int oldSeconds;
int tempHours;
int tempHoursTens;
int tempHoursUnits;

long millisTensToDisplay;

#define LED 2 // led on node mcu board

// Replace with your network credentials
//const char* ssid     = "bwardiPhone";
//const char* password = "qpf9m0at983k6";

//const char* ssid     = "NETGEAR56";
//const char* password = "dailyearth432";

const char* ssid     = "BHNTC8715D82E5";
const char* password = "c1e41bec";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

#define MY_NTP_SERVER "0.north-america.pool.ntp.org"


/* Globals */
time_t now;                         // this are the seconds since Epoch (1970) - UTC
tm tm;                              // the structure tm holds time information in a more convenient way

int seconds = 0;
int minutes = 0;
int hours = 0;
int days = 1;
int months = 1;
int years = 25;

//-------------------------------
void clearShiftRegs()
//-------------------------------
{
for (int i=0; (i < 97); i++)            // loop to clear all of the output bits
    {
    digitalWrite(nixieClockPin,HIGH);
    delayMicroseconds(1);
    digitalWrite(nixieClockPin,LOW);   
    delayMicroseconds(1);
    }
}

//-------------------------------
void strobeTheData()
//-------------------------------
{
    digitalWrite(nixieLatchPin,HIGH);       // send data to the outputs
    digitalWrite(nixieLatchPin,LOW);        // 
    
}

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
void incTime()              
//-------------------------------
//Just for testing of rollover etc, does not really keep track of time
//Other time keeping/ updating functons NTP, GPS, RTC must be disabled so the YMDHMS car can be used as counters
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
void rollTheZeros()
//-------------------------------
//some bs code to flash the clock zeros during wifi acquistion
{
    clearShiftRegs();
    shiftOut(nixieDataPin, nixieClockPin, MSBFIRST, 1);     //rhs units zero
    strobeTheData();
    delay(100);
    clearShiftRegs();
    shiftOut(nixieDataPin, nixieClockPin, MSBFIRST, 4);     //rhs tens zero
    shiftOut(nixieDataPin, nixieClockPin, MSBFIRST, 0);     //push data to 2nd byte of SR
    strobeTheData();
    delay(100);    
    clearShiftRegs();
    shiftOut(nixieDataPin, nixieClockPin, MSBFIRST, 1);     //third zero   
    shiftOut(nixieDataPin, nixieClockPin, MSBFIRST, 0); 
    shiftOut(nixieDataPin, nixieClockPin, MSBFIRST, 0);     //push data to 3nd byte of SR
    strobeTheData();
    delay(100);    
    clearShiftRegs();
    shiftOut(nixieDataPin, nixieClockPin, MSBFIRST, 4);     //tens zero
    shiftOut(nixieDataPin, nixieClockPin, MSBFIRST, 0);     //push data to 4th byte of SR
    shiftOut(nixieDataPin, nixieClockPin, MSBFIRST, 0); 
    shiftOut(nixieDataPin, nixieClockPin, MSBFIRST, 0);     //push data to 4th byte of SR
    strobeTheData();
    delay(100);    

}



//-------------------------------
void updateTimeDisplay()
//-------------------------------
{
    digitalWrite(nixieLatchPin, LOW); //disable data from registers to outputs as long as you are transmitting
    sendDigitPair(years);       // must be in the range of 0-49    
    sendDigitPair(months);      // must be in the range of 0-39
    sendDigitPair(days);        // must be in the range of 0-39
    sendDigitPair(hours);       // must be in the range of 0-59
    sendDigitPair(minutes);     // must be in the range of 0-59
    sendDigitPair(seconds);     // must be in the range of 0-59
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
    Serial.println("starting nixie clock \n");    

    pinMode(LED_pin,OUTPUT);

    pinMode(nixieDataPin, OUTPUT);          // sets the digital pin 13 as output
    pinMode(nixieLatchPin, OUTPUT);
    pinMode(nixieClockPin, OUTPUT);

    digitalWrite(nixieDataPin,LOW);         // pins to shift registers
    digitalWrite(nixieLatchPin,LOW);  
    digitalWrite(nixieClockPin,LOW);

    clearShiftRegs();
    strobeTheData();

    delay(500);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
        {
        // indicate that the ntp server has not connected
    //    delay(400);
        digitalWrite(LED,HIGH);
        rollTheZeros();
        }


    digitalWrite(LED,LOW);

//With these 3 lines you define your timezone an the NTP server.
  configTime(0, 0, MY_NTP_SERVER);  // 0, 0 because we will use TZ in the next line
  setenv("TZ", MY_TZ, 1);            // Set environment variable with your time zone
  tzset();

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

    time(&now);                         // read the current time
    localtime_r(&now, &tm);             // update the structure tm with the current time

    years = tm.tm_year -100;            // eat 2000 years to leave the tens and units of the year  (tm is years since 1900)
    months = tm.tm_mon+1;
    days = tm.tm_mday;
    hours =  tm.tm_hour;       
        if (hours >= 13) hours -= 12;    // 12 hour mode
        if (hours == 0) hours = 12;
    minutes = tm.tm_min;
    seconds = tm.tm_sec;


    delay(200);
    //digitalWrite(LED_pin, !digitalRead(LED_pin));  //toggle LED
    updateTimeDisplay();   // send the data to the nixie display
}

/*

  Serial.print("year:");
  Serial.print(tm.tm_year + 1900);    // years since 1900
  Serial.print("\tmonth:");
  Serial.print(tm.tm_mon + 1);        // January = 0 (!)
  Serial.print("\tday:");
  Serial.print(tm.tm_mday);           // day of month
  Serial.print("\thour:");
  Serial.print(tm.tm_hour);           // hours since midnight 0-23
  Serial.print("\tmin:");
  Serial.print(tm.tm_min);            // minutes after the hour 0-59
  Serial.print("\tsec:");
  Serial.print(tm.tm_sec);            // seconds after the minute 0-61*
  Serial.print("\twday");
  Serial.print(tm.tm_wday);           // days since Sunday 0-6
  if (tm.tm_isdst == 1)               // Daylight Saving Time flag
    Serial.print("\tDST");  
  else
    Serial.print("\tstandard");




//  time_t local = now();
//    Serial.println("%5U31288%S%0J%2AGPS:lock,local %4A%255C  " + timeClient.getFormattedTime() + ".724%Z");

//      sprintf(tmpStringArray, "SAT %.2d:%.2d:%.2d.%.3d", hour(local), minute(local), second(local));
//      tempString = timeClient.getFormattedTime();









    */