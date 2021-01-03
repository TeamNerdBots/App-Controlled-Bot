#include <Wire.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>


// Set these to run example. 
#define FIREBASE_HOST "autobot-a743a.firebaseio.com" 
#define FIREBASE_AUTH "jdypaqAErfWDY8LHYNMkYPte833uQIBepNV21djw" 
#define WIFI_SSID "POCO" 
#define WIFI_PASSWORD "amitrawat"


// MPU6050 Slave Device Address
const uint8_t MPU6050SlaveAddress = 0x68;

// Select SDA and SCL pins for I2C communication 
const uint8_t scl = D6;
const uint8_t sda = D7;

// sensitivity scale factor respective to full scale setting provided in datasheet 
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;

// MPU6050 few configuration register addresses
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;

int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;
double errorAX, errorAY, errorAZ ,errorGX, errorGY, errorGZ;
unsigned int currMillis, prevMillis = 0;
int num_reads = 0;
double alpha = 0.1;
double Ax = 0, Ay = 0, Az = 0, Gx = 0, Gy = 0, Gz = 0, T = 0;

void setup() {
  Serial.begin(9600);
  
   // connect to wifi. 
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
   //Serial.print("connecting"); 
   
   while (WiFi.status() != WL_CONNECTED) { 
    // Serial.print("."); 
     delay(500); 
   } 
//   
//   Serial.println(); 
//   Serial.print("connected: "); 
//   Serial.println(WiFi.localIP()); 
   Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); 
   Wire.begin(sda, scl);
   MPU6050_Init();

   delay(200);

   //measure zero error for calibration
   countError();
}

void loop() {
  
  double Ax1 = 0, Ay1 = 0, Az1 = 0, Gx1 = 0, Gy1 = 0, Gz1 = 0, T1 = 0;
  currMillis = millis();
  prevMillis = currMillis;
  
  while(currMillis - prevMillis < 15){
    Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
    num_reads++;
    
    //divide each with their sensitivity scale factor
    Ax1 += ((double)AccelX/AccelScaleFactor - errorAX);
    Ay1 += ((double)AccelY/AccelScaleFactor - errorAY);
    Az1 += ((double)AccelZ/AccelScaleFactor - errorAZ);
    T1 += (double)Temperature/340+36.53; //temperature formula
    Gx1 += ((double)GyroX/GyroScaleFactor - errorGX);
    Gy1 += ((double)GyroY/GyroScaleFactor - errorGY);
    Gz1 += ((double)GyroZ/GyroScaleFactor - errorGZ);
    currMillis = millis();
    delay(2);  }

  //applying a simple LP filter
  Ax = (1 - alpha)*Ax + alpha*Ax1;  
  Ax = Ax/num_reads;
  num_reads = 0;

  //send data to firebase realtime DB
  Firebase.setFloat("acceleration", Ax);
  Serial.println(Ax);
}

void countError(){
  double AX = 0, AY = 0, AZ = 0, GX = 0, GY = 0, GZ = 0;
  
  for(int i = 1; i <= 20; i++){
      Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
      AX += (double)AccelX/AccelScaleFactor;
      AY += (double)AccelY/AccelScaleFactor;
      AZ += (double)AccelZ/AccelScaleFactor;
      GX += (double)GyroX/GyroScaleFactor;
      GY += (double)GyroY/GyroScaleFactor;
      GZ += (double)GyroZ/GyroScaleFactor;
    } 
    
    errorAX = AX/20;
    errorAY = AY/20;
    errorAZ = AZ/20;
    errorGX = GX/20;
    errorGY = GY/20;
    errorGZ = GZ/20;
 }

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

// read all 14 register
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}

//configure MPU6050
void MPU6050_Init(){
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);//set +/-250 degree/second full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);// set +/- 2g full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}
