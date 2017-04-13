/* Momentum Table Monitoring and Control Program // 
   This does not provide direct motor control, but uses a GS1 Motor drive
   control from AutomationDirect. Control exists through contact 
   closure only at this time. 
*/
 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include "Secrets.h"

//////
//#define DESIREDRPM_TABLE    60      //desired RPM on output wheel
#define DESIREDRPM_MOTOR    600     //desired motor RPM (this may be superfulous) 
#define DISCSLIPCOUNT       250     //How long alarm cond before alarm signal
#define LOCKEDDISCRPM       10      //RPM to consider as "disc stopped"
#define ROTATIONVARIANCE    50      //Variance in RPM before alarm condition 
//#define TRANSMISSIONRATIO   10      //Tranmission drive ratio n:1
#define NUMBEROFRESTARTS    5      //#of restarts attempted after stopped state
#define STARTTIME           10000   //Expected ramp up time, in MS
#define STOPTIME            10000   //Expected ramp down time, in MS
 /////////
#define STARTRELAYPIN       1      //pin locations
#define HALLSENSORPIN       5       //hall sensor to detect rotaion
#define SECONDS             1000    //ms in seconds 

WiFiClient espClient;
PubSubClient client(espClient);



/**************************************/
/*            GLOBAL VARS              /
/**************************************/

uint16_t hallSensor1_count  = 0;
uint16_t hallSensor2_count  = 0;
volatile uint8_t pulseCount = 0;
uint8_t rpm                 = 0;
unsigned long timeOld       = 0;
//
unsigned long now           = 0;
unsigned long prevMillis    = 0;
unsigned long OTAUntilMillis = 0;
uint8_t discSlipPrecond    = 0;
uint8_t restartCount        = 0;
char msg[50];                   

/**************************************/
/*               SETUP                 /
/**************************************/
void setup(){
pinMode(STARTRELAYPIN, OUTPUT);
pinMode(HALLSENSORPIN, INPUT);
attachInterrupt(digitalPinToInterrupt(HALLSENSORPIN), sensorPulseCount, RISING);
client.setServer(mqtt_server, 1883);
client.setCallback(callback);
}


/**************************************/
/*                LOOP                 /
/**************************************/
void loop(){
//wait for mqtt commands
//send out mqtt status
calculateRPM();
discSlipCheck();
lockedRotorCheck();
//if commanded != actual caution torque slip
//if commanded != actual over time, report full error
now = millis();
//MQTT Connection Check
if (!client.connected()) {
  reconnect();
  }
  client.loop();
}


/**************************************/
/*             FUNCTIONS               /
/**************************************/
void sensorPulseCount(){
pulseCount++;
// expected two pulse counts per rotation, two sensors 
}

void calculateRPM(){
if (millis() - prevMillis == 1000){
    detachInterrupt();              //disable interrupts while running calculate
    rpm = pulseCount * 60;          //converting frequency to RPM
    pulseCount = 0;                 //reset pulse counter
    prevMillis = millis();          //update prevMillis
    attachInterrupt(digitalPinToInterrupt(HALLSENSORPIN), sensorPulseCount, RISING);
    //reattach interrupt
    }
  }

void motionControlStart(){
    if (restartCount < NUMBEROFRESTARTS){
        digitalWrite(STARTRELAYPIN, HIGH); // enable start relay if not above fail counter
        delay(2000); // wait for 2 second for a rotation 
        if (pulseCount > 1){               // if no rotation detected in interrupt
            digitalWrite(STARTRELAYPIN, LOW); // turn off motor becuase of no rotation
            restartCount++;
            delay (2000); // delay 2 seconds before attempting restart
            pulseCount = 0; // reset pulse count 
            motionControlStart();
            }
        else {
        restartCount = 0; // set restart counter to 0, since we're started
        }
    }    
    if (restartCount >= NUMBEROFRESTARTS) {
        //MQTT "Disc Start / Restart Failed ---- SYSTEM HALTED"
        allStop();   
    }
}

void motiionControlStop(){}
//disable start relay
//check for stopped rotation within x seconds
//rotation state no
//current rotation value 00
//if rotation doesn't stop, report error

void discSlipCheck(){
    if (rpm < (DESIREDRPM_TABLE - ROTATIONVARIANCE)){
        discSlipPrecond++;
        }
    else
        if (discSlipPrecond > 0){
        discSlipPrecond--;
        }
    if (discSlipPrecond > DISCSLIPCOUNT){
    //SEND MQTT MESSAGE HERE
    // "Caution! - Disc Slip Detected"
    //
    }
}

void lockedRotorCheck(){
    if (rpm <= LOCKEDDISCRPM){
        digitalWrite(STARTRELAYPIN, LOW); // immediately shut down if RPM lowByte
        }                                 // disc is stuck or stopped 
        //SEND MQTT MESSAGE HERE "WARNING! Disc has stopped rotation"
              
}

void allStop(){
    digitalWrite(STARTRELAYPIN, LOW);
    // wait here until MQTT command to restart or system power cycle
    //motionControlStart();
    