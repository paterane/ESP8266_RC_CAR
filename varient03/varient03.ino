#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>

const char* ssid_AP = "RC_car";
const char* pass_AP = "password";
unsigned int port = 2390;

char packetBuffer[100];
//char replyBuffer[] = "OK";
String revPacket;

long curMillis; 
int duration = 250;

Servo motor;
WiFiUDP udp;

void setup(){
  pinMode(D3,OUTPUT);
  pinMode(D1,OUTPUT);
  analogWrite(D3,0);
  analogWrite(D1,0);
  motor.attach(D5,500,2400);
  Serial.begin(9600);
  while(!Serial);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid_AP,pass_AP);
  udp.begin(port);
}

void loop(){
  int packetSize = udp.parsePacket(); // waiting for incoming packets of sizes
  if(packetSize){
    curMillis = millis();
    int len = udp.read(packetBuffer,packetSize); // reading packet's contents
    if(len > 0){
      packetBuffer[len] = 0;
    }
    revPacket = String(packetBuffer);
    /*udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write(replyBuffer);
    udp.endPacket();*/
    int Index1 = revPacket.indexOf('=');
    int Index2 = revPacket.indexOf('&');
    if(revPacket.substring(revPacket.indexOf('s') , Index1) == "state"){
      String value1 = revPacket.substring(Index1 + 1 , Index2);
      int Speed = value1.toInt();
      Serial.println(Speed);
      if(Speed >= 0 && Speed <= 1023){
        analogWrite(D1,Speed);
        analogWrite(D3,0);
      }
      else if(Speed < 0 && Speed >= -1023){
        Speed = abs(Speed);
        analogWrite(D3,Speed);
        analogWrite(D1,0); 
      }
      String value2 = revPacket.substring(Index2 + 1);
      int angle = value2.toInt();
      Serial.println(angle);
      motor.write(angle);
    }
    else if(revPacket.substring(revPacket.indexOf('c') , Index1) == "config"){
      String ssid_STA = revPacket.substring(Index1 + 1 , Index2);
      String pass_STA = revPacket.substring(Index2 + 1);
      WiFi.mode(WIFI_STA);
      WiFi.begin(pass_STA,pass_STA);
      while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(500);
      }
      Serial.println();
      revPacket = "";
      Serial.print("Connected, IP address: ");
      Serial.println(WiFi.localIP());
      udp.begin(port);
    }
  }
  else{
    if(millis() - curMillis > duration){
      analogWrite(D1,0);
      analogWrite(D3,0);
    }
  }
}
