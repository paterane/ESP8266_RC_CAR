#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


const char ssid = "RACER141";
unsigned int localPort = 2391;
unsigned int RCRecPort = 2390;
char sendPacket[10];
char recPacket[12];
IPAddress RCIPAddress(192,168,4,2);

WiFiUDP udp;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  udp.begin(localPort);
}

void loop() {
  int packetSize = udp.parsePacket();
  if(packetSize){
    int len = udp.read(recPacket,12);
    if(len>0){
      recPacket[len]=0;
    }
    for(int i=0; i<5; i++){
      RCIPAddress[i] = recPacket[i];
    }
  }
  if(Serial.available()){
    String line = Serial.readStringUntil('\n');
    String Speed = line.substring(line.indexOf('Y'),line.indexOf('X'));
    String angle = line.substring(line.indexOf('X'),line.indexOf('\r'));
    Speed.toCharArray(sendPacket,10);
    udp.beginPacket(RCIPAddress,RCRecPort);
    udp.write(sendPacket);
    udp.endPacket();
    angle.toCharArray(sendPacket,10);
    udp.beginPacket(RCIPAddress,RCRecPort);
    udp.write(sendPacket);
    udp.endPacket();
  }
}


/*
   if (line.length() == 1 && line[0] == '\n')
  {
    
    break;
  }
*/
//incoming.toCharArray(sendpacket,22);
