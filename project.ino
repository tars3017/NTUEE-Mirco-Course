#include "LedControl.h"
#include <Keypad.h>
// keypad define =========================
const byte ROWS = 4; // 共有四行
const byte COLS = 4; // 共有四列
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
// keypad define ==========================


// sound define
int melody[] = {262, 294, 330, 349, 392, 440, 494,
                523, 588, 660, 698, 784, 880, 988, 0};

int endGameTime[] = {8, 8, 8, 8, 4, 4, 4, 8};
int endGameTone[] = {494, 698, 0, 698, 698, 660, 588, 523};  
// sound define


// led define ============================
LedControl lc = LedControl(12, 10, 11, 1);
// led define ============================

// pattern define =======================
const int patternNum = 4;
byte pattern[4]={B00010101,B00011001,B00100011,B01000011};
//byte a[5]=    {B01111110,B10001000,B10001000,B10001000,B01111110};
// pattern define =======================
void setup() {
  Serial.begin(9600);
  lc.shutdown(0, false);
  lc.setIntensity(0, 1);
  lc.clearDisplay(0);
  randomSeed(analogRead(0)); // set random seed
//  lc.setRow(0, 2, B01111110);  //B01234567 <--position
//  lc.setRow(0, 0, B01111110);
}
byte randomChar(){
  return pattern[random(0, patternNum)]; 
}
char IntToChar(int num){
  return ((char)(48 + num));
}
void loop() {
  lc.setRow(0, 6, B01011011); //s
  lc.setRow(0, 5, B00001111); //t
  lc.setRow(0, 4, B01110111); //a
  lc.setRow(0, 3, B00000101); //r
  lc.setRow(0, 2, B00001111); //t
  lc.setRow(0, 1, B10110000); //!
  char key = kpd.getKey();
  //wait for start
  if(key){
    // start !!
    // TODO play music

    for(int i = 0; i < 8; i++)
      lc.setRow(0, i, B00000000); // Init
    // target pattern
    int curPos = -1; // move from right to left
    byte curPat = randomChar();  // get a random pattern
    byte lastPat = curPat; // to see repeat or not

    // player set
    int playPatInd = 0; // player's pattern index
    int point = 0, plus = 10; // plus per time
    float bonus = 1.08;
    int port1 = 5, port2 = 6, port3 = 7;

    // set screen
    lc.setRow(0, 4, B00110110);
    lc.setRow(0, 6, pattern[playPatInd]);

    int sepTime = 800; // set time
    int quick = 20;
    unsigned long lastTime = millis();
    while(1){
      char cmd = kpd.getKey();
      if(cmd == '0'){
        // turn the play's pattern
        lc.setRow(0, port2, B00000000);
        playPatInd = (playPatInd + 1) % patternNum; 
        tone(13, 600, 50);
        lc.setRow(0, port2, pattern[playPatInd]);
      }
//      Serial.println(Time);
      if(millis() - lastTime >= sepTime){
        if(curPos != 3)
          lc.setRow(0, 3, B00000000);
        lastTime = millis();
        lc.setRow(0, curPos, B00000000);
        tone(13, 800, 20);
        curPos += 1;
        lc.setRow(0, curPos, curPat);
      }
      if(curPos == 3){
        sepTime -= quick;
        if(pattern[playPatInd] != curPat){
          noTone(13);
          tone(13, 623, 470);
          delay(200);
          tone(13, 110, 260);
          delay(800);
          break;
        } 
        else{
          point += plus;
          plus *= bonus;
          tone(13, 750, 20);
          curPos = -1;
          while(curPat == lastPat){
            curPat = randomChar();
          }
          lastPat = curPat;
        }
        for(int i = 0; i < 3; i++)
          lc.setRow(0, i, B00000000);
        
      }
    }
    // play ending music
    for(int note = 0; note < 8; note++){
      int noteDur = 1000 / endGameTime[note];
      tone(13, endGameTone[note], noteDur);
      delay(noteDur * 1.30);
      noTone(13);
    }
    for(int i = 0; i < 8; i++){
      lc.setRow(0, i, B00000001);
      delay(150);
    }
    // show point
    lc.setRow(0, 7, B01100111);
    lc.setRow(0, 6, B00001111);
    lc.setRow(0, 5, B10000101);
    int show[5] = {0};
    int lastNotZero = 0;
    for(int i = 0; i < 5; i++){
      show[i] = point % 10;
      if(show[i] != 0) lastNotZero = i;
      point /= 10;
    }
    for(int i = 0; i < 3; i++){
      for(int j = 0; j < 5-lastNotZero; j++){
        lc.setChar(0, j, IntToChar(show[j]), false);
      }
      for(int j = 0; j < lastNotZero; j++){
        lc.setChar(0, 4-j, '0', false);
      }
      delay(500);
    }
    while(!kpd.getKey());
    lc.clearDisplay(0);
  }
}
