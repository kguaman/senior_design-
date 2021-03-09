#include <Adafruit_ICM20948.h>
#include <Adafruit_ICM20X.h>
#include <Adafruit_Sensor.h>
#include <math.h>
#include <Wire.h>
#include <SoftwareSerial.h>

const float RAD2DEG = 180.0f / PI;
const float G = 9.81; 


int k = 0;
float accx,accy,accz;

float gyrox,gyroy,gyroz;


float mag_acc;
float mag_acc_2;
float mag_gyro;
float mag_gyro_2;
/*
float theta_mea,phi_mea;
float thetanew,phinew; 
float thetaold = 0;
float phiold = 0;
*/
float current_mag_acc = 0;
float current_mag_acc_2 =0;   
float  current_mag_gyro =0;
float current_mag_gyro_2 =0;

float total_mag_acc ,total_mag_acc_2 ,total_mag_gyro,total_mag_gyro_2; 
unsigned long startMillis;
unsigned long currentMillis;
unsigned long elapsedMillis;


Adafruit_ICM20948 icm;
Adafruit_ICM20948 icm2;
//Adafruit_Sensor *icm_accel, *icm_gyro, *icm_mag;
//Adafruit_Sensor *icm_accel_2, *icm_gyro_2, *icm_mag_2;

//int button = 2;
int consecNonfall=5;   // Number of consecutive nonfall readings until solenoid retracts
int switchState = 0;    // 0-not pressed  1-pressed
int solenoidPin = 9;   // output pin of solenoid

void setup(void) {
  Serial.begin(115200);
  Wire.begin();
  pinMode(2,INPUT);
  pinMode(solenoidPin, OUTPUT);
  icm.begin_I2C(0x69);
  
  // range can be change 2,4,8,16,2g sound resobale for human movments 
  icm.setAccelRange(ICM20948_ACCEL_RANGE_2_G);
 
  // lower the DPS gyro can capture slow rotation but fast saturation would be sturated
  icm.setGyroRange(ICM20948_GYRO_RANGE_2000_DPS);// range 250,500,1000,2000
  
  icm.setGyroRateDivisor(255);
}

void loop() {
  
  //  /* Get a sensor event */
  //  /* Get a new normalized sensor event */
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t mag;
  sensors_event_t temp;
  icm.getEvent(&accel, &gyro, &temp, &mag);


  accz = accel.acceleration.z/G; 
  accy = accel.acceleration.y/G;
  accx = accel.acceleration.x/G;
  
  gyrox =gyro.gyro.x;
  gyroy =gyro.gyro.y;
  gyroz =gyro.gyro.z;
  
  char response = Serial.read();
  if(response == 'f' or response == 'n'){
    Serial.println(response);
    if(response == 'f'){
      digitalWrite(solenoidPin, LOW);      //Switch Solenoid EXTEND
      consecNonfall=0;
    }
    else{
      if(consecNonfall!=5){
         consecNonfall+=1;
         
      }
      else{
        digitalWrite(solenoidPin, HIGH);       //Switch Solenoid CONTRACT    
      }
    }
    //delay(10);
  }
  else{ 
    data();
  }
  switchState=digitalRead(2);
  if(switchState==1){
    Serial.println("PRESSED");
  }

/*
  mag_acc = sqrt(square(accz) + (square(accy))+ (square(accx)));
  mag_acc_2 = sqrt(square(accx_2) + (square(accy_2)) + (square(accz_2))); 
  mag_gyro= sqrt(square(gyrox) + (square(gyroy)) + (square(gyroz)));
  mag_gyro_2 = sqrt(square(gyrox_2) + (square(gyroy_2)) + (square(gyroz_2)));

*/
}   

void data(){  
  Serial.print(accx);
  Serial.print(",");
  Serial.print(accy);
  Serial.print(",");
  Serial.print(accz);   
  Serial.print(",");
  
  
  Serial.print(gyrox);
  Serial.print(",");
  Serial.print(gyroy);
  Serial.print(",");
  Serial.println(gyroz);   
}
