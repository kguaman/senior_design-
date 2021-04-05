// Run Arduino As Standalone for Fall Detection


#include <Wire.h>
#include <SoftwareSerial.h>
#include "model.h"
#define NUM_SAMPLES 35  // num samples per duration process


long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;

long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ, rotX_rad, rotY_rad, rotZ_rad;

float sensorVals[35][6];
float axSum, aySum, azSum, accelMag, accelMagSum, axMean, ayMean, azMean, accelMagMean ; // for accel mean calculation
float gxSum, gySum, gzSum, gyroMag, gyroMagSum, gxMean, gyMean, gzMean, gyroMagMean; // for gyro mean calculation

float axSquaredSum, aySquaredSum, azSquaredSum, axRMS, ayRMS, azRMS; // for accel RMS calculation
float gxSquaredSum, gySquaredSum, gzSquaredSum, gxRMS, gyRMS, gzRMS; // for gyro RMS calculation

float features[14];
int label;

int consecNonfall=7;
int switchState = 0;
int solenoidPin1 = 9;                    //This is the output pin on the Arduino
int solenoidPin2 = 10;
double seconds=0;
unsigned long time;
Eloquent::ML::Port::RandomForest clf;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(13,INPUT);
  pinMode(solenoidPin1, OUTPUT);          //Sets that pin as an output
  pinMode(solenoidPin2, OUTPUT);
  setupMPU();
}


void loop() {


//  time = 0;
  resetFeatures();
  getDurationData();

  getDurationFeatures();
  
  Serial.println("Raw Sample");
  printDurationData();
  Serial.println("Sample Features");
  printFeatures();

  label = getLabel();
  Serial.println(label);

  // Lock or Check to Unlock
  if(label == 1){
    digitalWrite(solenoidPin1, LOW);
    digitalWrite(solenoidPin2, LOW);
    consecNonfall=0;
  }
  else{
    if(consecNonfall!=7){
      consecNonfall+=1;
    }
    else{
      digitalWrite(solenoidPin1, HIGH);
      digitalWrite(solenoidPin2, HIGH);
    }
  }
  
  // measure timing
//  time = millis();
  delay(607);   // Wait 0.7 for data to be collected. Note: timed using millis()

}


void setupMPU(){
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
  Wire.endTransmission(); 
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission(); 
}

void recordAccelRegisters() {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processAccelData();
}

void processAccelData(){
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
  accelMag = sqrt(pow(gForceX,2)+pow(gForceY,2)+pow(gForceZ,2));
}

void recordGyroRegisters() {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x43); //Starting register for Gyro Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Gyro Registers (43 - 48)
  while(Wire.available() < 6);
  gyroX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  gyroY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  gyroZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processGyroData();
}

void processGyroData() {

  // deg/s
  rotX = gyroX / 131.0;
  rotY = gyroY / 131.0; 
  rotZ = gyroZ / 131.0;

  // rad/s
  rotX_rad = (rotX/180)*PI;
  rotY_rad = (rotY/180)*PI;
  rotZ_rad = (rotZ/180)*PI;
  
  gyroMag = sqrt(pow(rotX_rad,2)+pow(rotY_rad,2)+pow(rotZ_rad,2));
}

void printData() {
  
  // Get triaxial acceleration
  Serial.print(gForceX);
  Serial.print(",");
  Serial.print(gForceY);
  Serial.print(",");
  Serial.print(gForceZ);
  Serial.print(",");

  // Get triaxial gyroscope
  Serial.print(rotX_rad);
  Serial.print(",");
  Serial.print(rotY_rad);
  Serial.print(",");
  Serial.println(rotZ_rad);
}

void resetFeatures(){
  axSum=0, aySum=0, azSum=0;
  gxSum=0, gySum=0, gzSum=0;
  accelMagSum=0;
  gyroMagSum=0;
  
  axSquaredSum=0, aySquaredSum=0, azSquaredSum=0;
  gxSquaredSum=0, gySquaredSum=0, gzSquaredSum=0;

  for(int i =0; i<14;i++){
    features[i]=0;
  }
  
}
void getDurationData() {
  
  for(int i=0;i<NUM_SAMPLES;i++){
    recordAccelRegisters();
    recordGyroRegisters();
    for(int j=0;j<6;j++){
    
      if(j == 0){
        sensorVals[i][j]=gForceX;
        axSum+=gForceX;
        axSquaredSum+=pow(gForceX,2);
      }
      else if(j ==1){
        sensorVals[i][j]=gForceY;
        aySum+=gForceY;
        aySquaredSum+=pow(gForceY,2);
      }
      else if(j ==2){
        sensorVals[i][j]=gForceZ;
        azSum+=gForceZ;
        azSquaredSum+=pow(gForceZ,2);
      }
      else if(j ==3){
        sensorVals[i][j]=rotX_rad;
        gxSum+=rotX_rad;
        gxSquaredSum+=pow(rotX_rad,2);
      }
      else if(j ==4){
        sensorVals[i][j]=rotY_rad;
        gySum+=rotY_rad;
        gySquaredSum+=pow(rotY_rad,2);
      }
      else{
        sensorVals[i][j]=rotZ_rad;
        gzSum+=rotZ_rad;
        gzSquaredSum+=pow(rotZ_rad,2);
      }
    }
    accelMagSum+=accelMag;
    gyroMagSum+=gyroMag;
  }
}

void printDurationData() {
  for(int i=0;i<NUM_SAMPLES;i++){
    for(int j=0;j<6;j++){
      Serial.print(sensorVals[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void getDurationFeatures() {
  
  // getMeanAccel
  features[0]= axSum/NUM_SAMPLES;
  features[1] = aySum/NUM_SAMPLES;
  features[2] = azSum/NUM_SAMPLES; 

  // getMeanGyro
  features[3] = gxSum/NUM_SAMPLES;
  features[4] = gySum/NUM_SAMPLES;
  features[5] = gzSum/NUM_SAMPLES;

  // get accel and gyro mean
  features[6] = accelMagSum/NUM_SAMPLES;
  features[7] = gyroMagSum/NUM_SAMPLES;

  // get accel and gyro RMS
  features[8] = sqrt(axSquaredSum/NUM_SAMPLES);
  features[9] = sqrt(aySquaredSum/NUM_SAMPLES);
  features[10] = sqrt(azSquaredSum/NUM_SAMPLES);
  features[11] = sqrt(gxSquaredSum/NUM_SAMPLES);
  features[12] = sqrt(gySquaredSum/NUM_SAMPLES);
  features[13] = sqrt(gzSquaredSum/NUM_SAMPLES);
}

void printFeatures() {
  for(int i =0;i<14;i++){
    Serial.print(features[i]);
    Serial.print(" ");
  }
  Serial.println();
}

int getLabel() {
    //Serial.print("Detected: ");
    //Serial.println(clf.predictLabel(features));
    return clf.predictLabel(features);
  
}
