/* Momentum Table Monitoring and Control Program //
   This does not provide direct motor control, but uses a GS1 Motor drive
   control from AutomationDirect. Control exists through contact
   closure only at this time.
*/

//////
#define DESIREDRPM_TABLE    60             //desired RPM on output wheel
#define DISCSLIPCOUNT       150            //How long alarm cond before alarm signal
#define LOCKEDDISCRPM       20             //RPM to consider as "disc stopped"
#define ROTATIONVARIANCE    10             //Variance in RPM before alarm condition 
#define NUMBEROFRESTARTS    90             //#of restarts attempted after stopped state
#define STARTTIME           20 * SECONDS   //Expected ramp up time, in MS
#define STOPTIME            20 * SECONDS   //Expected ramp down time, in MS
/////////

#define STARTRELAYPIN       5              //pin locations
#define HALLSENSORPIN       4              //hall sensor to detect rotaion
#define SECONDS             1000           //ms in seconds 


/**************************************/
/*            GLOBAL VARS              /
/**************************************/

uint16_t hallSensor1_count  = 0;
uint16_t hallSensor2_count  = 0;
volatile uint8_t pulseCount = 0;
uint8_t rpm                 = 0;
unsigned long timeOld       = 0;
uint8_t allStopVar          = 1;
//
unsigned long now           = 0;
unsigned long prevMillis    = 0;
unsigned long OTAUntilMillis = 0;
bool OTARdyFlag = 0;
uint8_t discSlipPrecond     = 0;
uint8_t restartCount        = 0;
char msg[50];

/**************************************/
/*               SETUP                 /
  /**************************************/
void setup() {
  Serial.begin(115200);

  pinMode(STARTRELAYPIN, OUTPUT);
  pinMode(HALLSENSORPIN, INPUT_PULLUP);
  digitalWrite(STARTRELAYPIN, LOW);
  attachInterrupt(digitalPinToInterrupt(HALLSENSORPIN), sensorPulseCount, FALLING);
  
  //wifiSetup();
  //client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
  /////////////////////////////
  delay(5000);
  motionControlStart(); // Start the disc when plugged in
  ///////////////////////////// 
}


/**************************************/
/*                LOOP                 /
  /**************************************/
void loop() {

  Serial.println(pulseCount);

  if  (allStopVar == 0) {
    calculateRPM();
    discSlipCheck();
    lockedRotorCheck();
  }
  
}


/**************************************/
/*             FUNCTIONS               /
  /**************************************/
ICACHE_RAM_ATTR void sensorPulseCount() {
  pulseCount++;
  // expected two pulse counts per rotation, two sensors
}

void calculateRPM() {
  if (millis() - prevMillis >= 5000) {
    detachInterrupt(HALLSENSORPIN);              //disable interrupts while running calculate
    rpm = pulseCount * 6;          //converting frequency to RPM
    pulseCount = 0;                 //reset pulse counter
    prevMillis = millis();          //update prevMillis
    Serial.print("RPM: ");
    Serial.println(rpm);
    attachInterrupt(digitalPinToInterrupt(HALLSENSORPIN), sensorPulseCount, FALLING);
  }
}

void motionControlStart() {
  if (restartCount < NUMBEROFRESTARTS) {
    Serial.println("Motor Control Start");
    digitalWrite(STARTRELAYPIN, HIGH); // enable start relay if not above fail counter
    delay(4000); // wait for 4 second for a rotation

    if (pulseCount == 0) {              // if no rotation detected in interrupt
      digitalWrite(STARTRELAYPIN, LOW); // turn off motor becuase of no rotation
      Serial.println("No rotation detected -- attempting restart");
      restartCount++;
      delay (STOPTIME); // delay 10 seconds before attempting restart
      pulseCount = 0; // reset pulse count
      motionControlStart();
    }
    if (pulseCount >= 1) {
      Serial.println("motion started, system running");
      delay(STARTTIME); 	// wait 10(STARTTIME) seconds for disc to come up to fast enough speed
      // to not trip locked rotor functiion
      restartCount = 0; // set restart counter to 0, since we're started
      allStopVar = 0; // Set allstop condition to off
      loop();
    }
  }
  if (restartCount >= NUMBEROFRESTARTS) {
    digitalWrite(STARTRELAYPIN, LOW);
    Serial.println("disc not started");
    
    //MQTT "Disc Start / Restart Failed ---- SYSTEM HALTED"
    allStop();
  } 
} 

void motionControlStop() {
  digitalWrite(STARTRELAYPIN, LOW);//disable start relay
  delay (15000); // wait 10 seconds for spindown
  calculateRPM();
  Serial.println("MotionControlStop");
  if (rpm > 0) {
    allStop();
  }
}

void discSlipCheck() {
  if (rpm < ((DESIREDRPM_TABLE - ROTATIONVARIANCE) && (allStopVar == 0))) {
    discSlipPrecond++;
  }
  else if (discSlipPrecond > 0 && allStopVar == 0) {
    discSlipPrecond--;
  }
  if (discSlipPrecond > DISCSLIPCOUNT) {
    Serial.println("Continious disc slip detected");
    discSlipPrecond = 0;
  }
}


void lockedRotorCheck() {
  if (rpm <= LOCKEDDISCRPM) {
    Serial.println("Locked rotor. attempting restart");
    digitalWrite(STARTRELAYPIN, LOW); // immediately shut down if RPM lowByte
    delay(STOPTIME);
    motionControlStart();
  }
}

void allStop() {
  digitalWrite(STARTRELAYPIN, LOW);
  Serial.println("System in All-Stop");
  //delay(10000);
  allStopVar = 1;
}

void exhibitStatusMsg() {
  char gs_msg [50];
  snprintf (gs_msg, 38, "Disc RPM %i", now / rpm);
  Serial.println(gs_msg);
}
