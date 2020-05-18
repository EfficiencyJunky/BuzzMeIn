/**************************************************************
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 *
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 *
 **************************************************************
 * This example shows how to use Arduino WiFi shield
 * to connect your project to Blynk.
 *
 * Feel free to apply it to any other example. It's simple!
 *
 **************************************************************/

// CODE UPDATED 5/17/2020 by Turner Kirk


#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <SPI.h>
#include <WiFi101.h>
//#include <BlynkSimpleWifi.h> // removed this line after Blynk library was updated and replaced with the one below
#include <BlynkSimpleWiFiShield101.h>


//#define __DEBUG_PRINTS__
#define __USE_EXTERNAL_AUTH_AND_WIFI_CREDENTIALS__

#if defined(__USE_EXTERNAL_AUTH_AND_WIFI_CREDENTIALS__)
  #include "MyBlynkAuthAndWiFiCreds.h"
#else
// ******************************************************************************
//       ******** AUTH TOKEN AND SSID/PASSWORD VARIABLES *********
// ******************************************************************************
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "";

// Your WiFi credentials
char ssid[] = "";
char pass[] = "";        // Set to "" for open networks
#endif

// ******************************************************************************
//       ******** VIRTUAL PIN ASSIGNMENTS/DECLARATIOINS *********
// ******************************************************************************
// Software Variables
#define V_PIN_MOMENTARY_BUTTON V1
#define V_PIN_LATCH_BUTTON V5
#define V_PIN_PANIC_BUTTON V7

#define V_PIN_NUM_ACTIVATIONS_DISP V2
#define V_PIN_NUM_DEACTIVATIONS_DISP V3
#define V_PIN_UPTIME_DISP V4
#define V_PIN_COUNTDOWN_DISP V6

#define V_PIN_COUNTDOWN_GAUGE V8

#define V_PIN_LATCH_TIME_SLIDER V9


// ******************************************************************************
//       ******** HARDWARE VARIABLES *********
// ******************************************************************************
int secs_to_latch = 5;      // The length of time in seconds that the buzzer is activated for when the latch button is pressed

const uint8_t relayPin = 4;     // The pin assignment for the relay that activates the buzzer
int numActivations = 0;    // This variable gets updated everytime the buzzer is activated 
int numDeactivations = 0;  // This variable gets updated everytime the buzzer is de-activated 




// *********************************************************************************************************************************************************
// SETUP FUNCTION    ---     SETUP FUNCTION      ---     SETUP FUNCTION      ---     SETUP FUNCTION      ---     SETUP FUNCTION      ---     SETUP FUNCTION
// SETUP FUNCTION    ---     SETUP FUNCTION      ---     SETUP FUNCTION      ---     SETUP FUNCTION      ---     SETUP FUNCTION      ---     SETUP FUNCTION
// *********************************************************************************************************************************************************
void setup()
{
  // initialize the LED pin as an output:
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

#if defined(__DEBUG_PRINTS__)  
  Serial.begin(9600);
#endif

  Blynk.begin(auth, ssid, pass);
  // Or specify server using one of those commands:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, ssid, pass, "server.org", 8442);
  //Blynk.begin(auth, ssid, pass, server_ip, port);

  initInterface();
}

// *********************************************************************************************************************************************************
// SETUP FUNCTION    ---     SETUP FUNCTION      ---     SETUP FUNCTION      ---     SETUP FUNCTION      ---     SETUP FUNCTION      ---     SETUP FUNCTION
// SETUP FUNCTION    ---     SETUP FUNCTION      ---     SETUP FUNCTION      ---     SETUP FUNCTION      ---     SETUP FUNCTION      ---     SETUP FUNCTION
// *********************************************************************************************************************************************************



// ******************************************************************************************************************************************************
// ************************** BLYNK FUNCTIONS CALLED BY APP INTERACTION ********************************************************************************* 
// ******************************************************************************************************************************************************

/*
// This function will run every time Blynk connection is established
BLYNK_CONNECTED() 
{
//  if (isFirstConnect) {
   Blynk.syncAll();
//    isFirstConnect = false;
//  }

  // You can also update some virtual pin
  // I'll push uptime, just for this example
//  int value = millis() / 1000;
  Blynk.virtualWrite(V_PIN_NUM_ACTIVATIONS_DISP, "Arduino Connected");
}
*/

// when the Momentary pushbutton is pressed call this function
BLYNK_WRITE(V_PIN_MOMENTARY_BUTTON)
{

#if defined(__DEBUG_PRINTS__)
  BLYNK_LOG("Got a value: %s", param.asStr());
#endif

  // You can also use:
  // int i = param.asInt() or
  // double d = param.asDouble()
  
  int buttonState = param.asInt();

  // check if the pushbutton is pressed.
  // if it is pressed, the buttonState is HIGH and we want to turn the buzzer on
  // if it is released, the buttonState is LOW and we want to turn the buzzer off
  if (buttonState == HIGH) {
    turnBuzzerOn();
  }
  else {
    turnBuzzerOff();
  }
}


// when the Panic pushbutton is pressed call this function
BLYNK_WRITE(V_PIN_PANIC_BUTTON)
{  
  int buttonState = param.asInt();

  // check if the PANIC pushbutton is pressed.
  // if it is pressed, the buttonState is HIGH and we want to turn the buzzer off
  // if it is released, we don't care
  if (buttonState == HIGH) {
    turnBuzzerOff();
  }
}




// read the latch button pin and countdown the latch
BLYNK_WRITE(V_PIN_LATCH_BUTTON)
{

#if defined(__DEBUG_PRINTS__)
  BLYNK_LOG("Got a value: %s", param.asStr());
#endif

  int buttonState = param.asInt();

  // check if the LATCH pushbutton is pressed.
  // if it is pressed, the buttonState is HIGH and we want to turn the buzzer on for 5 seconds
  if (buttonState == HIGH) {

    //turn the buzzer on
    turnBuzzerOn();

    // logic for counting 5 seconds and updating the UI on the app
    // secs_to_latch is a global variable that can be adjusted with a slider in the app
    // the default should be 5
    for(int i=0; i < secs_to_latch; i++){

      // update the gauge each time through the loop to show the current count starting with the value stored in "secs_to_latch" variable
      Blynk.virtualWrite(V_PIN_COUNTDOWN_GAUGE, secs_to_latch - i);    

      // wait 1000 milliseconds because we want to countdown in increments of 1 second
      delay(1000);
    }

    // update the gauge one last time to show a value of 0
    Blynk.virtualWrite(V_PIN_COUNTDOWN_GAUGE, 0);
    
    delay(20);

    // Set the state of the Latch Button back to 0
    Blynk.virtualWrite(V_PIN_LATCH_BUTTON, 0);

    delay(20);

    // turn the buzzer off
    turnBuzzerOff();

  }

}

// when the Panic pushbutton is pressed call this function
BLYNK_WRITE(V_PIN_LATCH_TIME_SLIDER)
{  
  int seconds_from_slider = param.asInt();

  secs_to_latch = seconds_from_slider;

  // update the gauge to have a new max value for countdowns
  Blynk.setProperty(V_PIN_COUNTDOWN_GAUGE, "max", secs_to_latch);

  delay(20);
}



// This function is called by the app every N seconds where N is set on the widget in the app
// The goal of this function is to display the uptime of the arduino (the amount of time since it was last turned on)
// The display is formatted as "_ day _ hr _ min _ sec" where the '_' are filled in with the actual time passed for each unit of time
BLYNK_READ(V_PIN_UPTIME_DISP)
{

  // the "ConvertSectoDay" function calculates the number of days/hours/minuts/seconds that have passed since the Arduino was turned on
  // it returns a String that is formatted as specified above
  String time_formatted = ConvertSectoDay( (int)(millis() / 1000) );

//  Serial.println("Time String is: " + time_formatted);

  // once we've got the string, we want to update the corresponding display widget in the app
  Blynk.virtualWrite(V_PIN_UPTIME_DISP, time_formatted);

#if defined(__DEBUG_PRINTS__)
//  BLYNK_LOG("Time String is: %s", time_formatted);
#endif  
}

// ******************************************************************************************************************************************************
// MAIN LOOP    ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP
// MAIN LOOP    ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP
// ******************************************************************************************************************************************************
void loop()
{
  Blynk.run();
}

// ******************************************************************************************************************************************************
// MAIN LOOP    ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP
// MAIN LOOP    ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP      ---     MAIN LOOP
// ******************************************************************************************************************************************************




// ******************************************************************************************************************************************************
// ************************** HELPER FUNCTIONS ********************************************************************************************************** 
// ******************************************************************************************************************************************************
void turnBuzzerOn(){
    // turn relay on:
    digitalWrite(relayPin, LOW);

    // increment number of Activations
    numActivations++;    

    // update the app's display widget with the number of Activations
    Blynk.virtualWrite(V_PIN_NUM_ACTIVATIONS_DISP, numActivations);
  
}



void turnBuzzerOff(){
    // turn relay off:
    digitalWrite(relayPin, HIGH);
    
    // increment number of Deactivations
    numDeactivations++;

    // update the app's display widget with the number of Deactivations
    Blynk.virtualWrite(V_PIN_NUM_DEACTIVATIONS_DISP, numDeactivations);
  
}



// Found this function here
// https://www.geeksforgeeks.org/converting-seconds-into-days-hours-minutes-and-seconds/
String ConvertSectoDay(int n) 
{ 
    const int days = n / (24 * 3600); 
  
    n = n % (24 * 3600); 
    const int hours = n / 3600; 
  
    n %= 3600; 
    const int minutes = n / 60 ; 
  
    n %= 60; 
    const int seconds = n; 

    // How to make strings
    // https://www.arduino.cc/en/Tutorial/StringConstructors
    String days_s =  String(days);         // create a string using a const integer
    String hours_s =  String(hours);
    String minutes_s =  String(minutes);
    String seconds_s =  String(seconds);

    return String(days_s + " day " + hours_s + " hr " + minutes_s + " min " + seconds_s + " sec"); // concatenating multiple strings
} 


// Function that initializes the interface whenever the power is cycled to the Arduino
void initInterface(){

  // Initialize Button States
  Blynk.virtualWrite(V_PIN_MOMENTARY_BUTTON, 0);
  delay(20);
  Blynk.virtualWrite(V_PIN_LATCH_BUTTON, 0);
  delay(20);
  Blynk.virtualWrite(V_PIN_PANIC_BUTTON, 0);

  delay(20);

  // Initialize Display Values
  Blynk.virtualWrite(V_PIN_NUM_ACTIVATIONS_DISP, 0);
  delay(20);
  Blynk.virtualWrite(V_PIN_NUM_DEACTIVATIONS_DISP, 0);
  delay(20);
  String time_formatted = ConvertSectoDay( (int)(millis() / 1000) );
  Blynk.virtualWrite(V_PIN_UPTIME_DISP, time_formatted);
  
  delay(20);

  // Initialize values of the Slider and Gauge widgets
  // Slider Value should be "secs_to_latch"
  // Gauge value should be 0
  Blynk.virtualWrite(V_PIN_LATCH_TIME_SLIDER, secs_to_latch);
  delay(20);
  Blynk.virtualWrite(V_PIN_COUNTDOWN_GAUGE, 0);

  delay(20);
  // set the max value of the Gauge widget to the "secs_to_latch" variable
  Blynk.setProperty(V_PIN_COUNTDOWN_GAUGE, "max", secs_to_latch);

  delay(20);
  
}
