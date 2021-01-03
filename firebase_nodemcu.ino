#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>


// Set these to run example. 
#define FIREBASE_HOST "autobot-a743a.firebaseio.com" 
#define FIREBASE_AUTH "jdypaqAErfWDY8LHYNMkYPte833uQIBepNV21djw" 
#define WIFI_SSID "POCO" 
#define WIFI_PASSWORD "amitrawat"
#define EN1 D5
#define EN2 D6


void setup() { 
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(3, OUTPUT);
//  Serial.begin(9600); 
   
   // connect to wifi. 
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
//Serial.print("connecting"); 
   
   while (WiFi.status() != WL_CONNECTED) { 
//Serial.print("."); 
     delay(500); 
   } 
   
//   Serial.println(); 
//   Serial.print("connected: "); 
//   Serial.println(WiFi.localIP()); 
   Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); 
} 


void loop() { 
   int curr_speed = Firebase.getInt("speed");
   String command = Firebase.getString("command");

   if(command == "L"){
      moveLeft();
    }
   else if(command == "R"){
      moveRight();
    }
   else{
     if(curr_speed < 0){
        moveBackward();
     }
     else{
      moveForward();
      }
   }
   analogWrite(EN1, abs(curr_speed));
   analogWrite(EN2, abs(curr_speed));
   delay(30);
}

void moveForward(){
  digitalWrite(D1, HIGH);
  digitalWrite(D0, LOW);
  digitalWrite(D3, HIGH);
  digitalWrite(D2, LOW);
  digitalWrite(3, HIGH);
  digitalWrite(1, LOW);
  digitalWrite(D8, HIGH);
  digitalWrite(D7, LOW);
  }
  
void moveBackward(){
  digitalWrite(D0, HIGH);
  digitalWrite(D1, LOW);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, LOW);
  digitalWrite(1, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(D7, HIGH);
  digitalWrite(D8, LOW);
  }

void moveRight(){
  digitalWrite(D0, LOW);
  digitalWrite(D1, LOW);
  digitalWrite(1, LOW);
  digitalWrite(3, LOW);
  }

void moveLeft(){
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D7, LOW);
  digitalWrite(D8, LOW);
  }
