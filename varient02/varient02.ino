#include <Servo.h>
#include <ESP8266WiFi.h>

const char* STA_ssid = "yourssid";
const char* AP_ssid = "DIY_RC_Car";
const char* password = "yourssid_password";
bool networkFound = false;
long curMillis; 
int duration = 350;

Servo motor;
WiFiClient client;
WiFiServer server(80);

void setup(){
  pinMode(D3,OUTPUT);
  pinMode(D1,OUTPUT);
  analogWrite(D3,0);
  analogWrite(D1,0);
  motor.attach(D5,500,2400);
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int networksFound = WiFi.scanNetworks();
  for(int i=0; i<networksFound; i++){
    Serial.printf("%d: %s\n", i+1, WiFi.SSID(i));
    if(WiFi.SSID(i) == "HomeNet_PO"){
      networkFound = true;
    }
  }
  if(networkFound){
    WiFi.begin(STA_ssid,password);
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
  }
  else{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_ssid);
  }
  server.begin();
  server.setNoDelay(true);
}

void loop(){
  client = server.available();
  // wait for a client (web browser) to connect
  if (client){
    //Serial.println("\n[Client connected]");
    while (client.connected()){
      // read line by line what the client (web browser) is requesting
      if (client.available())
      { 
        String line = client.readStringUntil('\r');
        //Serial.print(line);
        // wait for end of client's request, that is marked with an empty line
        if (line.length() == 1 && line[0] == '\n')
        {
          client.print("HTTP/1.1 200 OK\r\n");
          //client.print("Content-Type: text/html\r\n");
          client.print("\r\n");
          client.print("Connection: close\r\n");
          break;
        }
        int startIndex = line.indexOf("?");
        int midIndex = line.indexOf("=");
        int comIndex = line.indexOf("&");
        int endIndex = line.indexOf(" ", midIndex);
        if(line.substring(startIndex + 1, midIndex) == "state"){
          String value1 = line.substring(midIndex + 1, comIndex);
          int Speed = value1.toInt();
          if(Speed > 0 && Speed <= 1023){
            //Serial.print("Forward speed: ");
            //Serial.println(Speed);
            analogWrite(D1,Speed);
            analogWrite(D3,0);
          }
          else if(Speed < 0 && Speed >= -1023){
            //Serial.print("Backward speed: ");
            Speed = abs(Speed);
            //Serial.println(Speed);
            analogWrite(D3,Speed);
            analogWrite(D1,0); 
          }
          else{
            analogWrite(D3,0);
            analogWrite(D1,0);
          }
          String value2 = line.substring(comIndex + 1, endIndex);
          int angle = value2.toInt();
          //Serial.println(angle);
          motor.write(angle);
        }
        if(line.substring(startIndex + 1, midIndex) == "stopTime"){
          String value = line.substring(midIndex + 1, endIndex);
          int number = value.toInt();
          duration = number; 
          //Serial.println(number);
        }
      } 
    }
    while (client.available()) {
      // but first, let client finish its request
      // that's diplomatic compliance to protocols
      // (and otherwise some clients may complain, like curl)
      // (that is an example, prefer using a proper webserver library)
      client.read();
    }
    curMillis = millis();
    // close the connection:
    client.stop();
    //Serial.println("[Client disconnected]");
  }
  else{
    if(millis() - curMillis > duration){
      //Serial.println("Stop");
      analogWrite(D1,0);
      analogWrite(D3,0);
      motor.write(90);
    }
  }
  while(Serial.available()){
    String Snum = Serial.readString();
    int num = Snum.toInt();
    Serial.println(num);
    duration = num;
  }
}

//Useful library's codes.
//WiFi.mode();
//WiFi.SSID();
//WiFi.scanNetworks();
//WiFi.scanDelete();
