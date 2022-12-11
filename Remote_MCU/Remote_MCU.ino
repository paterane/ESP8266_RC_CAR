#include<SoftwareSerial.h>
int LED = 13;
int input[] = {A6, A4};
int value[2]; //let value[0] for Y and value[1] for X
int lastValue1;

SoftwareSerial COM(4,3);

void setup() {
  Serial.begin(9600);
  COM.begin(9600);
  for(int i=0; i<2; i++){
    pinMode(input[i],INPUT);
  }
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
}

void loop() {
  value[0] = analogRead(input[0]);
  value[1] = analogRead(input[1]);
    
  if(value[0] > 650){
    value[0] = 650;
  }
  else if(value[0] < 312){
    value[0] = 312;
  }
  if(value[1] > 750){
    value[1] = 750;
  }
  else if(value[1] < 350){
    value[1] = 350;
  }
  value[0] = map(value[0],312,650,-1023,1023);
  value[1] = 180 - map(value[1],350,750,0,180);
  if(value[0] <= 100 && value[0] >= -200){
    value[0] = 0;
  }
  value[1] -= value[1] % 10;
  if(value[0] != 0 || value[1] != lastValue1){
    lastValue1 = value[1];
    COM.print("Y");
    COM.print(value[0]);  
    COM.print("X");
    COM.println(value[1]);
    Serial.print(value[0]);
    Serial.print("  ");
    Serial.println(value[1]);
    delay(200);
  }
}
