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
#define DESIREDRPM_TABLE    60      //desired RPM on output wheel
#define DESIREDRPM_MOTOR    600     //desired motor RPM (this may be superfulous) 
#define ALARMTIME           3000    //How long alarm cond before alarm signal
#define ROTATIONVARIANCE    50      //Variance in RPM before alarm condition 
#define TRANSMISSIONRATIO   10      //Tranmission drive ratio n:1
#define NUMBEROFRESTARTS    10      //#of restarts attempted after stopped state
#define STARTTIME           10000   //Expected ramp up time, in MS
#define STOPTIME            10000   //Expected ramp down time, in MS
 /////////
#define STARTRELAYPIN       1      //pin locations
#define HALLSENSOR1PIN      5       //dual hall sensors to detect rotaion
#define HALLSENSOR2PIN      4


WiFiClient espClient;
PubSubClient client(espClient);

char msg[50];


/**************************************/
/*            GLOBAL VARS              /
/**************************************/

uint16_t hallSensor1_count  = 0 ;
uint16_t hallSensor2_count  = 0 ;



/**************************************/
/*               SETUP                 /
/**************************************/
void setup()
{
pinMode(STARTRELAYPIN, OUTPUT);
pinMode(HALLSENSOR1PIN, INPUT);
pinMode(HALLSENSOR2PIN, INPUT);

client.setServer(mqtt_server, 1883);
client.setCallback(callback);
}


/**************************************/
/*                LOOP                 /
/**************************************/
void loop()
{
//wait for mqtt commands
//send out mqtt status
sensorPulseCount(); // returns formatted RPM number
discSlipCheck();
//if commanded != actual caution torque slip
//if commanded != actual over time, report full error

//MQTT Connection Check
if (!client.connected()) {
  reconnect();
  }
  client.loop();
}


/**************************************/
/*             FUNCTIONS               /
/**************************************/
void sensorPulseCount(){}
//current rotation value xx
// report back RPM

void motionControlStart(){}
//enable start relay
//check for rotation within x seconds
//rotation state yes
//current rotation value xx
//if rotation doesn't start, report error

void motiionControlStop(){}
//disable start relay
//check for stopped rotation within x seconds
//rotation state no
//current rotation value 00
//if rotation doesn't stop, report error

void discSlipCheck()
{// check disc for slip condition
//if below certain rotatation for x seconds, motiionControlStop

}
