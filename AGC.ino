#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"


// --- Pins ---
const int MIC_PIN = A0;          
const int SENSITIVITY_PIN = A1;  
const int BTN_ANNOUNCE = 4;      
const int BTN_FIRE = 3;          
const int SWITCH_AUTO = 2;    




unsigned long lastTriggerTime = 0;
const int cooldown = 10000;      
int noiseCounter = 0;


SoftwareSerial mySoftwareSerial(11, 10); // RX, TX
DFRobotDFPlayerMini myDFPlayer;


void setup() {
  Serial.begin(9600);
  mySoftwareSerial.begin(9600);


  pinMode(BTN_ANNOUNCE, INPUT_PULLUP);
  pinMode(BTN_FIRE, INPUT_PULLUP);
  pinMode(SWITCH_AUTO, INPUT_PULLUP);


  Serial.println(F("Initializing DFPlayer..."));
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Error: Check SD card or wiring."));
    while(true);
  }
 
  myDFPlayer.volume(23);
  Serial.println(F("System Ready."));
}


void loop() {
  int micValue = analogRead(MIC_PIN);
  int currentAmplitude = abs(micValue - 300);


  int adaptiveVol;
  if (currentAmplitude >= 200) {
    adaptiveVol = 30;
  } else {
    adaptiveVol = 23;
  }


  if (digitalRead(BTN_ANNOUNCE) == LOW) {
    Serial.print("ANNOUNCE Trig - Amp: "); Serial.print(currentAmplitude);
    Serial.print(" | Vol: "); Serial.println(adaptiveVol);
   
    myDFPlayer.volume(adaptiveVol);
    delay(100);
    myDFPlayer.play(2);
    delay(500);
  }


  if (digitalRead(BTN_FIRE) == LOW) {
    Serial.print("FIRE ALARM Trig - Amp: "); Serial.print(currentAmplitude);
    Serial.print(" | Vol: "); Serial.println(adaptiveVol);
   
    myDFPlayer.volume(adaptiveVol);
    delay(100);
    myDFPlayer.play(1);
    delay(500);
  }


  // 2. Check Auto-Silence 
  if (digitalRead(SWITCH_AUTO) == LOW) {
    monitorNoise();
  }
}


void monitorNoise() {
  int micValue = analogRead(MIC_PIN);
  int amplitude = abs(micValue - 300);


  // --- AUTO SILENCE TRIGGER LOGIC ---
  if (amplitude >= 200) {
    noiseCounter += 5;
  } else {
      if (noiseCounter > 0) noiseCounter -= 2;
  }


   if (noiseCounter > 50) {
    if (millis() - lastTriggerTime > cooldown) {
      Serial.print("AUTO SILENCE TRIGGERED! Amp: "); Serial.print(amplitude);
      Serial.println(" | Setting Vol to 30");
     
      myDFPlayer.volume(30);       delay(100);
      myDFPlayer.play(3);     
      lastTriggerTime = millis();
      noiseCounter = 0;
    }
  }
  delay(10);
}
