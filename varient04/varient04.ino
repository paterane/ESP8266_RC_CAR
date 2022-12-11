#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* STA_ssid = "RACER141";
unsigned int port = 2390;
unsigned int TX_port = 2391;
IPAddress TX_ip (192,168,4,1);

char packetBuffer[100];
char ipPacket[4];

long curMillis; 
int duration = 250;

Servo motor;
WiFiUDP udp;

void setup(){
  Serial.begin(9600);
  pinMode(D3,OUTPUT);
  pinMode(D1,OUTPUT);
  analogWrite(D3,0);
  analogWrite(D1,0);
  analogWriteRange(1023); //Default is 255
  analogWriteFreq(60); //Default is 1000Hz, higher the freq, higher the impedance and lower the torque and vice versa.
  motor.attach(D5,500,2400); // In ESP12 and ESP12E, servo range is re-calibrated to get full range of 0 - 180 degrees.
  WiFi.mode(WIFI_STA);
  WiFi.begin(STA_ssid);
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  IPAddress IP = WiFi.localIP();
  for(int i=0; i<5; i++){
    ipPacket[i] = IP[i];
  }

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  udp.begin(port);
  udp.beginPacket(TX_ip,TX_port);
  udp.write(ipPacket);
  udp.endPacket();
}

void loop(){
  ArduinoOTA.handle();
  int packetSize = udp.parsePacket(); // waiting for incoming packets of sizes
  if(packetSize){
    curMillis = millis();
    int len = udp.read(packetBuffer,100); // reading packet's contents
    if(len > 0){
      packetBuffer[len] = 0;
    }
    if(packetBuffer[0] == 'Y'){
      char* numPart = packetBuffer + 1;
      int Speed = atoi(numPart);
      if(Speed >= 0 && Speed <= 1023){
        analogWrite(D1,Speed);
        analogWrite(D3,0);
      }
      else if(Speed < 0 && Speed >= -1023){
        Speed = abs(Speed);
        analogWrite(D3,Speed);
        analogWrite(D1,0); 
      }
    }
    else if(packetBuffer[0] == 'X'){
      char* numPart = packetBuffer + 1;
      int angle = atoi(numPart);
      motor.write(angle);
    }
  }
  else{
    if(millis() - curMillis > duration){
      analogWrite(D1,0);
      analogWrite(D3,0);
    }
  }
}


//const char* STA_pass = "12@pnet#69";
//const char* AP_ssid = "RC_car";
//const char* AP_pass = "12101992peter";
//bool networkFound = false;
//WiFi.disconnect();
//delay(100);
//int networksFound = WiFi.scanNetworks();
/*for(int i=0; i<networksFound; i++){
  if(WiFi.SSID(i) == "HomeNet_PO"){
    networkFound = true;
  }
}
if(networkFound){
  WiFi.begin(STA_ssid,STA_pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
}
else{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_ssid,AP_pass);
}*/
