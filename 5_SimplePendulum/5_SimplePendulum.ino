#define BLYNK_PRINT Serial

#define echoPin 5 
#define trigPin 6

long duration;
int distance;

int change_length = 0;

int prev_value = 0;
int value = 0;
int IR_Pin = 7;
int IR = 7;
int L[3] = {10,20,30};

int IR_flag = 0;
int Prev_IR_flag = 1;
double g = 0;
double g_avg = 0;
int g_count = 0;
double curr_time = 0;
double prev_time = 0;

int active_length = L[0];
int prev_active_length = L[0];

bool Stepper_flag = false;
bool exp_start = false;
bool exp_flag = false;
bool prev_exp_flag = false;
bool Stepper_done = false;

int count = 0;

//defines pins
const int stepPin = 4;  //PUL -Pulse
const int dirPin = 3; //DIR -Direction
const int enPin = 2;  //ENA -Enable


#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <Servo.h> 

char auth[] = "1OqUpotls0HbtcZo_aY7uUgBbFyTGWSf";
// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = "Jewel's OnePlus";
//char pass[] = "d9baf5ce2ad9";

char ssid[] = "nhk";
char pass[] = "9704957641";


#define ESP8266_BAUD 115200

BlynkTimer timer;

// Declare the Servo pin 
int servoPin = 10; 
// Create a servo object 
Servo Servo1;

ESP8266 wifi(&Serial);
void setup()
{
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(enPin,OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  
  Serial.begin(115200);
  delay(10);
  Serial.begin(ESP8266_BAUD);
  delay(10);
  Blynk.begin(auth, wifi, ssid, pass);

  digitalWrite(enPin,LOW);
  Servo1.attach(servoPin);
  Servo1.write(0); 
  //timer.setInterval(100L, myTimerEvent);

  Stepper_flag = true;
  //BringUp();
}

void loop()
{ 
    Blynk.run();
    IR_Sensor();

}


//BLYNK_READ(V5) //Blynk app has something on V5
//{
  //int sensorData = distance; //reading the sensor on A0
  //Blynk.virtualWrite(V5, sensorData); //sending to Blynk
//}

void IR_Sensor()
{   
    if(exp_start == true)
    { 
      g = 0;
      count = 0;
      g_count = 0;
      g_avg = 0;

      exp_start = false;
    }
    
    // Select active IR sensor based on detected length

    Prev_IR_flag = IR_flag;
    IR_flag = digitalRead(7);
    if(Prev_IR_flag == 1 && IR_flag == 0)
    {
      count++;
      Prev_IR_flag = 0;

      prev_time = curr_time;
      curr_time = millis();
      double period = 2*(curr_time-prev_time);
      period /= 1000;
      double period_sq = period*period;
      g = ((4*3.141*3.141 *active_length)/period_sq)/100;

      //if(count % 10 < 3 && g <= 11.5 && g >= 8.5)
       //{  
              //Serial.print("g = ");
              //Serial.println(active_length);
       //}
    }
    if(g <= 15 && g >= 6 && count > 6)
    {   
        g_count++;
        g_avg += g;   
        double g_res = g_avg/g_count;  
        Blynk.virtualWrite(V7,g_res);
    }
      
}

void StepperUp(int IR_Pin)
{   
    Stepper_done = false;
    digitalWrite(dirPin,LOW);
  //Makes 200 Pulses for making one full cycle rotation
    while(digitalRead(IR_Pin)==HIGH){
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500); 
    }
    Stepper_done = true;
}

void StepperDown(int IR_Pin)
{   
    Stepper_done = false;
    digitalWrite(dirPin,HIGH);
  //Makes 200 Pulses for making one full cycle rotation
  while(digitalRead(IR_Pin) == HIGH){
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500); 
    Stepper_done = true;
  }
}


void changeLength()
{ 
  if(Stepper_flag == false)
  { 
    /*
    if(prev_active_length ==  L[0] && active_length == L[1])
      StepperDown(8);
    else if (prev_active_length ==  L[0] && active_length == L[2])
      StepperDown(9);
    else if (prev_active_length ==  L[1] && active_length == L[2]) 
      StepperDown(9);
    else if (prev_active_length == L[1] && active_length == L[0])
      StepperUp(7);
    else if (prev_active_length == L[2] && active_length == L[0])
      StepperUp(7);
    else if (prev_active_length ==  L[2] && active_length == L[1])
      StepperUp(8);
    */
    if(active_length == L[0])
      StepperDown(7);
    else if(active_length == L[1])
      StepperDown(8);
    else if(active_length == L[2])
      StepperDown(9);
    Stepper_flag = true;
      
  }

  else if(Stepper_flag == true)
    StepperUp(7);
}

/*
void IR_Sensor()
{
       Prev_IR_flag = IR_flag;
    IR_flag = digitalRead(IR);
    if(Prev_IR_flag == 1 && IR_flag == 0)
    {
      count++;
      Prev_IR_flag = 0;
   //   Serial.print("count = ");
 //     Serial.println(count);

      prev_time = curr_time;
      curr_time = millis();
      double period = 1.93*(curr_time-prev_time);
      period /= 1000;
      double sq = period*period;
      g = ((4*3.14*3.14 *0.28)/sq);

      
}
*/
void ultraSonicSensor()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance = duration * 0.034 / 2;
    //Serial.print("Distance: ");
    //Serial.print(distance);
    //Serial.println(" cm");
}

void ServoTest()
{ 
   Servo1.write(160);
   delay(500);
   Servo1.write(0);
   delay(500);
}


BLYNK_WRITE(V6)
{ 
  prev_value = value;  
  value = param.asInt(); // Get value as integer
  if(prev_value == false && value == true)
    ServoTest();
}


BLYNK_WRITE(V8)
{
  active_length = param.asInt();
  if(change_length == 1)
  {
  digitalWrite(dirPin, HIGH);
  //Makes 200 Pulses for making one full cycle rotation
  for(int i = 0; i < 1400 * (active_length-17); i++){
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
  }
    change_length = 0;
    Blynk.virtualWrite(V9,0);
  }
}

BLYNK_WRITE(V9)
{
  change_length = param.asInt();
 
}

BLYNK_WRITE(V10)
{
  int stepper_flag = param.asInt();
  if(stepper_flag == 1)
    {
      digitalWrite(dirPin, LOW);
    //Makes 200 Pulses for making one full cycle rotation
    while(digitalRead(8)==HIGH){
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
  }
        g = 0;
      count = 0;
      g_count = 0;
      g_avg = 0;

      Blynk.virtualWrite(V10,0);
    }
    
}

void Experiment()
{ 
  if(exp_flag == true && prev_exp_flag == false)
  { 
    prev_exp_flag = true;
    Stepper_done = false;
    changeLength();
    if(Stepper_done == true)
      ServoTest();
    exp_start = true;
  }
  else if(exp_flag == false && prev_exp_flag == true)
  { 
    prev_exp_flag = false;
    Stepper_done = false;
    changeLength();
    exp_start = false;
  }
  IR_Sensor();
}
