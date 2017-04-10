/

#define DESIREDRPM          60      // desired RPM on output wheel
#define ALARMTIME           3000    // How long an alarm has to be present before alarm 
#define ROTATIONVARIANCE    50      // Variance in RPM allowable before alarm condition 
#define TRANSMISSIONRATIO   10      // Tranmission drive ratio n:1
#define NUMBEROFRESTARTS    10      // How many restarts to try after a stopped condition
#define STARTTIME           10000   // Expected ramp up time, in MS
#define STOPTIME            10000   // Expected ramp down time, in MS
            
#define STARTRELAYPIN       D1      // pin locations
#define HALLSENSOR1PIN      5
#define HALLSENSOR2PIN      4
void setup()
{
}

void loop()
{
}