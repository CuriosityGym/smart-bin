#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <WiFiManager.h> 
#include <EEPROM.h>

// WiFi parameters
//#define WLAN_SSID       "DT_LAB"
//#define WLAN_PASS       "fthu@050318"

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "xxxxxxxxxx"
#define AIO_KEY         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"  // Obtained from account info on io.adafruit.com
byte count = 0;
//int led =16;
int pwmValue =1023;
int trigPin1 = 15;
int echoPin1 = 13;
int trigPin2 = 14;
int echoPin2 = 12;
long duration;
int distance;
int lmotorA=4;
int lmotorB=0;
int enA = 5;
int dustbinHeightLimit=10;
int dustbinHeight = 0;
int dustbinStatus = 0;
bool dustbinEmpty = true;
bool messageSent = false;
WiFiClient client;
 
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
 
/****************************** Feeds ***************************************/

Adafruit_MQTT_Publish dustbin = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/dustbin");
 
void setup() {
  // pinMode(5, OUTPUT);
  // pinMode(4, OUTPUT);
  // pinMode(0, OUTPUT);
  // pinMode(2, OUTPUT);
   pinMode(lmotorA, OUTPUT); 
   pinMode(lmotorB, OUTPUT);
   pinMode(enA, OUTPUT);
  // pinMode(led,OUTPUT);
   pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
   pinMode(echoPin1, INPUT); // Sets the echoPin as an Input
   pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
   pinMode(echoPin2, INPUT); // Sets the echoPin as an Input
   
   Serial.begin(115200);
   // Connect to WiFi access point.
  Serial.println(); Serial.println();
  delay(10);
  //Serial.print(F("Connecting to "));
  //Serial.println(WLAN_SSID);
  int x = 0;
  /*WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED && x < 40) {
    delay(500);
    Serial.print(F("."));
    x=x+1;
  }*/
  WiFiManager wifiManager;
    // wifiManager.resetSettings();
    wifiManager.autoConnect("Smart Bin");
    Serial.println("connected to");
    Serial.println( WiFi.SSID().c_str());
    Serial.println(WiFi.psk().c_str());
    wifiManager.setConfigPortalTimeout(180);

  Serial.println();

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
  // connect to adafruit io 
  mqtt.connect();

}

void loop() {
  if(count == 10){
    Serial.println("connecting");
    // ping adafruit io a few times to make sure we remain connected
    if(! mqtt.ping(3)) {
      // reconnect to adafruit io
      if(! mqtt.connected())
        mqtt.connect();
    }
   count = 0;
  }
  for(int i=0; i<2; i++){
     distance+=measureDistance(trigPin1,echoPin1);
     delay(2);
  }
  distance = distance/2;
  Serial.print("Distance: ");
  Serial.println(distance);
  if(distance < 30 && dustbinEmpty == false){
     //digitalWrite(led, LOW);
     openBin();
     delay(12000);
     //stopMotor();
    // delay(10000);
     closeBin();
     //digitalWrite(led, HIGH);
     delay(200);
     stopMotor(); 
     delay(5000);
  }
  if(distance < 30 && dustbinEmpty == true){
     //digitalWrite(led, LOW);
     openBin();
     delay(7000);
     //stopMotor();
     //delay(5000);
     closeBin();
     //digitalWrite(led, HIGH);
     delay(200);
     stopMotor(); 
     delay(5000);
  }  
  for(int i=0; i<3; i++){
     dustbinHeight+=measureDistance(trigPin2,echoPin2);
     delay(2);
     if(i==2) count= count+1;
   } 
  dustbinHeight = dustbinHeight/3;
  Serial.print("Dustbin height: ");
  Serial.println(dustbinHeight);
  if((dustbinHeight < dustbinHeightLimit) && messageSent == false){
     dustbinStatus = 1;
     if (! dustbin.publish(dustbinStatus))
        Serial.println(F("Failed to publish"));
     else
        Serial.println(F("published!"));

     messageSent = true;
     dustbinEmpty = false;
     Serial.print("Dustbin full");
     delay(500);
  }
  if(messageSent == true && dustbinEmpty == false && dustbinHeight >17){
     dustbinEmpty = true;
     messageSent = false;
     dustbinStatus = 0;
     if (! dustbin.publish(dustbinStatus))
        Serial.println(F("Failed to publish"));
     else
        Serial.println(F("published!"));
     Serial.print("Dustbin empty");
  }
}

int measureDistance(int trigger_pin, int echo_pin){
  int d=0;
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  // put your main code here, to run repeatedly:
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);
  duration = pulseIn(echo_pin, HIGH);
  // Calculating the distance
  d= duration*0.0346/2;
  // Prints the distance on the Serial Monitor
  //Serial.print("Distance: ");
  //Serial.println(d);
  return d;
}
void stopMotor(void)
{
   analogWrite(enA,0);
    digitalWrite(lmotorA, LOW);  
  digitalWrite(lmotorB, LOW);
    //analogWrite(5, 0);
   // analogWrite(4, 0);
}

 
void openBin(void)
{
  digitalWrite(lmotorA, LOW);  
  digitalWrite(lmotorB, HIGH);  
 /* for(int i=0; i<=512; i+8){
   analogWrite(enA,i);
   delay(1);
  }*/
  analogWrite(enA,1023);
   // analogWrite(5, pwmValue);
   // analogWrite(4, pwmValue);
   // digitalWrite(0, HIGH);
   // digitalWrite(2, HIGH);
}


void closeBin(void)
{
    analogWrite(enA,0);
    digitalWrite(lmotorA, HIGH);  
    digitalWrite(lmotorB, LOW);
    //analogWrite(5, pwmValue);
   // analogWrite(4, pwmValue);
    //digitalWrite(0, LOW);
   // digitalWrite(2, HIGH);
}
