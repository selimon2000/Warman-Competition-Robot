//Winch Motor
int INI4 = 7; //Brown
int INI3 = 6; //Red
int ENB = 5; //PWM, Orange
//Gondola Motor
// do not require enable as using full power
int ENA = 4; //PWM, Yellow
int INI1 = 2; //White
int INI2 = 3; //Black
//Motor Speed Constants
int SB = 255; // winch
int SA = 255;

// SETUP ULTRASONIC SENSOR, which will face downwards
const int trigPin = 13; //Orange
const int echoPin = 12; //Brown
long int duration;
float distance;
float finalDistance;
float UltrasonicRecordedDistance;

//IR SENSOR : hardcoded in function below to Pin A0
float IRRecordedDistance;

// robot distances
float spooled=6; //in cm, the distance between the lower sensor fo the gondola and the upper surface of the car. Shouldn't be too high; as long as it doesn't touch
float unspooled=39; //in cm, the distance between the lower sensor fo the gondola and the upper surface of the car. Shouldn't be too high; as long as it doesn't touch

int pylonDistance=24; //in cm, the distance between the sensor of the gondola facing towards the pylon

// robot delays
long int retrieveTime=10*1000; //approximate time taken for robot to place retrieve package in seconds
long int dropTime = 10*1000; //approximate time taken for robot to place retrieve package in seconds

long int startTime, endTime;

void setup() 
{ 
  //Gondola and Winch motor pin set up:
  pinMode(ENB, OUTPUT);
  pinMode(INI1, OUTPUT);
  pinMode(INI2, OUTPUT);
  pinMode(INI3, OUTPUT);
  pinMode(INI4, OUTPUT);
  //Ultrasonic:
  pinMode(trigPin, OUTPUT); // orange wire, Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // yellow wire, Sets the echoPin as an Input
  pinMode(8, OUTPUT);     //green wire, to power sensor
  digitalWrite(8, HIGH);  // same as above, but these 2 lines need to be enabled for 5volts output

  Serial.begin(9600); // Starts the serial communication
}

void loop() 
{ 
while(1)
{
Serial.print("USDistance: ");Serial.println(UltrasonicDistance());  
}
 
//Step 1A: Start with winch fully unspooled, until robot does grips payload and starts moving other direction
delay(4000);
  
//Step 1B: Bring car closer to winch
WinchUp();
//how long it takes for the robot to come back closer to winch
delay(3600);

//car has returned back by now, and is NOT about to do donut, delaying for momentum
WinchStop();
delay(1000);

WinchUp();
//Check if robot has been sufficiently winched up
UltrasonicRecordedDistance=UltrasonicDistance();

while(UltrasonicRecordedDistance>=spooled)
{UltrasonicRecordedDistance=UltrasonicDistance();}

WinchStop();
delay(1000); //just to stop momentum/precaution

//Step 2: Traverse to other side
//Bring system forwards by powering gondola motor
GondolaToDeposit();
startTime=millis();

//until the sensor recognises the pylon to be the exact distance away, do NOTHING different, keep moving forward
IRRecordedDistance=IRDistance();
while(IRRecordedDistance>=pylonDistance)
{IRRecordedDistance=IRDistance();
Serial.print("IRDistance: ");Serial.println(IRRecordedDistance);}
endTime=millis();
GondolaStop();Serial.print("STOPPING");
delay(1000); // purpose to stop momentum and precaution


// Step 3: Bring car down and then back up:
WinchDown();
//rather than relying in sensors, rely on delay to place car onto the ground
/*UltrasonicRecordedDistance=UltrasonicDistance();
while(UltrasonicRecordedDistance<=unspooled)
{UltrasonicRecordedDistance=UltrasonicDistance();
Serial.print("UR: ");Serial.println(IRDistance();}*/
delay(6000);
WinchStop();

delay(11000); //delaying for momentum

//robot is on the ground now, and ready to move:
WinchDown();
delay(3000); //approximate time taken for robot to do the donut

//robot moves forward:
delay(3000);

//robot is at deposit zone, and drops package:
WinchStop();
delay(2000);

//robot has dropped package, and ready to return underneath gondola
WinchUp();
delay(3000);

//robot is ready to be grappled up, but wants to stop first to reduce momentum
WinchStop();
delay(1000);

//by now it has done the donut, and it needs to steer, and lock rotating disk
WinchUp();

//Check if robot has been sufficiently winched up
UltrasonicRecordedDistance=UltrasonicDistance();
while(UltrasonicRecordedDistance>=spooled)
{UltrasonicRecordedDistance=UltrasonicDistance();}
WinchStop();
delay(1000); //just to stop momentum/precaution


// Step 4: Bring back to start zone
GondolaToStart();
delay(endTime-startTime); // Rather than having a sensor on the car, im gonna replicate the time taken to get to pylong drop off zone
delay(3000); //Further delay as precaution'

//End program by ending powering motors at 20% and entering while (1) loop so it appears to stop, rather than actually stopping motors as it starts to slide as holding torque when stopped is pretty weak
int SA = 50;
analogWrite(ENA, SA);
while(1){}
}

void GondolaToStart()//DOES NOTHING, BROKEN
{digitalWrite(INI1, LOW);
  digitalWrite(INI2, HIGH);
  analogWrite(ENA, SA);}
void GondolaToDeposit()
{digitalWrite(INI1, HIGH);
  digitalWrite(INI2, LOW);
  analogWrite(ENA, SA);}
void GondolaStop()
{digitalWrite(INI1, LOW);
 digitalWrite(INI2, LOW);
 analogWrite(ENA, SA);}
 
void WinchUp()
{digitalWrite(INI3, HIGH);
  digitalWrite(INI4, LOW);
  analogWrite(ENB, SB);}
  void WinchDown()
{digitalWrite(INI3, LOW);
 digitalWrite(INI4, HIGH);
 analogWrite(ENB, SB);}
void WinchStop()
{digitalWrite(INI3, LOW);
  digitalWrite(INI4, LOW);
  analogWrite(ENB, SB);}

float IRDistance()
{int value1 = analogRead(A0);
delay(20);
int value2 = analogRead(A0);
delay(20);
int value3 = analogRead(A0);
delay(20);

float x = (value1 + value2 + value3) / 3;
float distance = 40969 * pow(x, -1.22);
return distance;}

float UltrasonicDistance()
{ 
  finalDistance=0;
  
  for (int i=1;i<=3;i++)
  {
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2;
    finalDistance+=distance;
  }
  finalDistance=finalDistance/3;
  //return:
  return finalDistance;}
