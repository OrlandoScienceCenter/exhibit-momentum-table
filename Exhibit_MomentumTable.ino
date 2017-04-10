/* Momentum Table Monitoring and Control Program // 
   This does not provide direct motor control, but uses a GS1 Motor drive
   control from AutomationDirect. Control exists through contact 
   closure only at this time. 
*/
#define DESIREDRPM_TABLE    60      // desired RPM on output wheel
#define DESIREDRPM_MOTOR    600     // desired motor RPM (this may be superfulous) 
#define ALARMTIME           3000    // How long an alarm has to be present before alarm 
#define ROTATIONVARIANCE    50      // Variance in RPM allowable before alarm condition 
#define TRANSMISSIONRATIO   10      // Tranmission drive ratio n:1
#define NUMBEROFRESTARTS    10      // How many restarts to try after a stopped condition
#define STARTTIME           10000   // Expected ramp up time, in MS
#define STOPTIME            10000   // Expected ramp down time, in MS
 /////////
#define STARTRELAYPIN       D1      // pin locations
#define HALLSENSOR1PIN      5       // dual hall sensors to detect rotaion
#define HALLSENSOR2PIN      4

/**************************************/
/*            GLOBAL VARS              /
/**************************************/

uint16_t hallSensor1_count  = 0
uint16_t hallSensor2_count  = 0


void setup()
{
pinMode(STARTRELAYPIN, OUTPUT);
pinMode(HALLSENSOR1PIN, INPUT);
pinMode(HALLSENSOR2PIN, INPUT);


}

void loop()
{
}



void sensorPulseCount(){}

void motionControlStart(){}

void motiionControlStop(){}

