#include <SoftwareSerial.h>
#include <DFPlayer.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include "LedControl.h"
//-------------------------------------------------------------------- SRF-05
#define trig 5
#define echo 6
//-------------------------------------------------------------------- talk time
#define welcome_time 4000
#define info_time 22000
//-------------------------------------------------------------------- DF player
#define MP3_RX_PIN 2
#define MP3_TX_PIN 4
#define MP3_SERIAL_SPEED 9600
#define MP3_SERIAL_BUFFER_SIZE 32
#define MP3_SERIAL_TIMEOUT 100
SoftwareSerial mp3Serial(MP3_RX_PIN, MP3_TX_PIN);
DFPlayer mp3;
//-------------------------------------------------------------------- matrix
LedControl lc = LedControl(7, 9, 8, 4);
//-------------------------------------------------------------------- servo
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define MIN_PULSE_WIDTH 125
#define MAX_PULSE_WIDTH 575
#define DEFAULT_PULSE_WIDTH 1500
#define FREQUENCY 50
uint8_t servonum = 0 ;
#define RBM 100       // right blue minimum
#define LBM 550       // left blue minimum
#define RBLM 250      //right black minimum
#define LBLM 400      //left black minimuml
//-------------------------------------------------------------------- variable
unsigned long current = 0 , old = 0 , old2 = 0 ;
unsigned long duration = 0;
unsigned int distance = 0 ;
//-------------------------------------------------------------------- bytes
byte openeye[] = {
  B00000000,
  B01111110,
  B10000001,
  B10011001,
  B10011001,
  B10000001,
  B01111110,
  B00000000
};
byte closeeye[] = {
  B00000000,
  B00000000,
  B00011000,
  B00011000,
  B00011000,
  B00011000,
  B00000000,
  B00000000
};
byte mouth[] = {
  B01100000,
  B01100000,
  B00111000,
  B00011100,
  B00001110,
  B00000000,
  B00000000,
  B00000000
};
byte openmouth[] = {
  B01000010,
  B01000010,
  B01000010,
  B01000010,
  B00111100,
  B00000000,
  B00000000,
  B00000000
};
byte closemouth[] = {
  B00100100,
  B00100100,
  B00100100,
  B00100100,
  B00011100,
  B00000000,
  B00000000,
  B00000000
};
void setup() {
  //------------------------------------------------------------------ DF player
  mp3Serial.begin(MP3_SERIAL_SPEED);
  mp3.begin(mp3Serial, MP3_SERIAL_TIMEOUT, false);
  mp3.stop();
  mp3.reset();
  mp3.setSource(2);  //1=USB-Disk, 2=TF-Card, 3=Aux, 4=Sleep, 5=NOR Flash
  mp3.setVolume(30);
  mp3.sleep();
  //------------------------------------------------------------------ LED.martix
  int devices = lc.getDeviceCount();
  for (int address = 0; address < devices; address++) {
    lc.shutdown(address, false);
    lc.setIntensity(address, 4);
    lc.clearDisplay(address);
  }
  //------------------------------------------------------------------ servo
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);
  //------------------------------------------------------------------ srf
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(3, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(3, 1);
  digitalWrite(10, 0);
  //------------------------------------------------------------------ serial
  //Serial.begin(9600);
  start();
}
void loop() {
  if (srf() < 150) {
    mp3.wakeup(2);
    mp3.playTrack(1);
    for (int j = 0 ; j < 5 ; j++) {
      for (int i = 0 ; i < 8 ; i++) {
        switch (j) {
          case 0 :
            lc.setRow(j, i, mouth[i]);
            break ;
          case 1 :
          case 2 :
            lc.setRow(j, i, openeye[i]);
            break ;
          case 3 :
            lc.setRow(j, 7 - i, mouth[i]);
            break ;
        }
      }
    }
    for (int i = 0 ; i < 200 ; i += 10) {
      pwm.setPWM(3, 0, pulseWidth(RBM + i));
      pwm.setPWM(1, 0, pulseWidth(RBLM + i));
      pwm.setPWM(2, 0, pulseWidth(LBLM - i));
      if (i<=100){
        pwm.setPWM(0, 0, pulseWidth(LBM - i));
      }
    }
    delay(10000);
  }
  else {
    nojob();
  }
}
int pulseWidth(int angle) {
  int pulse_wide, analog_value;
  pulse_wide = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  analog_value = int(float(pulse_wide) / 1000000 * FREQUENCY * 4096);
  return analog_value;
}
void start() {
  for (int j = 0 ; j < 5 ; j++) {
    for (int i = 0 ; i < 8 ; i++) {
      switch (j) {
        case 0 :
          lc.setRow(j, i, mouth[i]);
          break ;
        case 1 :
        case 2 :
          lc.setRow(j, i, openeye[i]);
          break ;
        case 3 :
          lc.setRow(j, 7 - i, mouth[i]);
          break ;
      }
    }
  }
  pwm.setPWM(0, 0, pulseWidth(LBM));     // left blue
  pwm.setPWM(1, 0, pulseWidth(RBLM));    // right black
  pwm.setPWM(2, 0, pulseWidth(LBLM));    // left black
  pwm.setPWM(3, 0, pulseWidth(RBM));     // right blue
}
void welcome() {
  mp3.wakeup(2);
  mp3.playTrack(1);
  unsigned long old = 0 , old2 = 0 , break_time = 0;
  current = millis();
  old = current ;
  old2 = current ;
  break_time = current ;
  while (true) {
    current = millis();
    if (current - old >= 2500) {
      for (int j = 0 ; j < 5 ; j++) {
        for (int i = 0 ; i < 8 ; i++) {
          switch (j) {
            case 1 :
            case 3 :
              lc.setRow(j, i, closeeye[i]);
              break ;
          }
        }
      }
      old = current ;
    } else if (current - old >= 300) {
      for (int j = 0 ; j < 5 ; j++) {
        for (int i = 0 ; i < 8 ; i++) {
          switch (j) {
            case 1 :
            case 2 :
              lc.setRow(j, i, openeye[i]);
              break ;
          }
        }
      }

    }
    if (current - old2 >= 1000) {
      for (int j = 0 ; j < 5 ; j++) {
        for (int i = 0 ; i < 8 ; i++) {
          switch (j) {
            case 0 :
              lc.setRow(j, i, openmouth[i]);
              break ;
            case 3 :
              lc.setRow(j, 7 - i, openmouth[i]);
              break ;
          }
        }
      }
      pwm.setPWM(3, 0, pulseWidth(RBM));
      old2 = current ;
    } else  if (current - old2 >= 500) {
      for (int j = 0 ; j < 5 ; j++) {
        for (int i = 0 ; i < 8 ; i++) {
          switch (j) {
            case 0 :
              lc.setRow(j, i, closemouth[i]);
              break ;
            case 3 :
              lc.setRow(j, 7 - i, closemouth[i]);
              break ;
          }
        }
      }
      pwm.setPWM(3, 0, pulseWidth(RBM + 300));
    }
    if (current - break_time >= welcome_time) {
      break_time = current ;
      pwm.setPWM(3, 0, pulseWidth(RBM));
      break ;
    }
  }
}
void info() {
  int flag = 0 ;
  current = millis();
  old = current ;
  old2 = current ;
  while (true) {
    current = millis();
    if (flag == 0) {
      for (int j = 0 ; j < 5 ; j++) {
        for (int i = 0 ; i < 8 ; i++) {
          switch (j) {
            case 0 :
              lc.setRow(j, i, openmouth[i]);
              break ;
            case 1 :
              lc.setRow(j, i, openeye[i]);
              break ;
            case 3 :
              lc.setRow(j, 7 - i, openmouth[i]);
              break ;
            case 2 :
              lc.setRow(j, 7 - i, openeye[i]);
              break ;
          }
        }
      }
      pwm.setPWM(1, 0, pulseWidth(RBLM + 200));
      pwm.setPWM(2, 0, pulseWidth(LBLM - 200));
      pwm.setPWM(3, 0, pulseWidth(RBM));
      pwm.setPWM(0, 0, pulseWidth(LBM));
      delay(300);
      old = current ;
      flag = 1 ;
    }
    if (current - old >= 700 && flag == 1 ) {
      for (int j = 0 ; j < 5 ; j++) {
        for (int i = 0 ; i < 8 ; i++) {
          switch (j) {
            case 0 :
              lc.setRow(j, i, closemouth[i]);
              break ;
            case 3 :
              lc.setRow(j, 7 - i, closemouth[i]);
              break ;
          }
        }
      }
      pwm.setPWM(3, 0, pulseWidth(RBM + 200));
      pwm.setPWM(0, 0, pulseWidth(LBM - 100));
      flag = 2;
    }
    if (current - old >= 1000 && flag == 2 ) {
      for (int j = 0 ; j < 5 ; j++) {
        for (int i = 0 ; i < 8 ; i++) {
          switch (j) {
            case 0 :
              lc.setRow(j, i, openmouth[i]);
              break ;
            case 3 :
              lc.setRow(j, 7 - i, openmouth[i]);
              break ;
          }
        }
      }
      pwm.setPWM(3, 0, pulseWidth(RBM));
      pwm.setPWM(0, 0, pulseWidth(LBM));
      flag = 3 ;
    }
    if (current - old >= 1300 && flag == 3 ) {
      for (int j = 0 ; j < 5 ; j++) {
        for (int i = 0 ; i < 8 ; i++) {
          switch (j) {
            case 0 :
              lc.setRow(j, i, closemouth[i]);
              break ;
            case 3 :
              lc.setRow(j, 7 - i, closemouth[i]);
              break ;
          }
        }
      }
      pwm.setPWM(1, 0, pulseWidth(RBLM));
      pwm.setPWM(2, 0, pulseWidth(LBLM));
      pwm.setPWM(3, 0, pulseWidth(RBM + 200));
      pwm.setPWM(0, 0, pulseWidth(LBM - 100));
      flag = 4 ;
    }
    if (current - old >= 1600 && flag == 4 ) {
      for (int j = 0 ; j < 5 ; j++) {
        for (int i = 0 ; i < 8 ; i++) {
          switch (j) {
            case 0 :
              lc.setRow(j, i, openmouth[i]);
              break ;
            case 1 :
              lc.setRow(j, i, closeeye[i]);
              break ;
            case 2 :
              lc.setRow(j, 7 - i, closeeye[i]);
              break ;
            case 3 :
              lc.setRow(j, 7 - i, openmouth[i]);
              break ;

          }
        }
      }
      pwm.setPWM(3, 0, pulseWidth(RBM));
      pwm.setPWM(0, 0, pulseWidth(LBM));
      old = current ;
      flag = 5 ;
    }
    if (current - old >= 200 && flag == 5) {
      for (int j = 0 ; j < 5 ; j++) {
        for (int i = 0 ; i < 8 ; i++) {
          switch (j) {
            case 0 :
              lc.setRow(j, i, openmouth[i]);
              break ;
            case 1 :
              lc.setRow(j, i, openeye[i]);
              break ;
            case 2 :
              lc.setRow(j, 7 - i, openeye[i]);
              break ;
            case 3 :
              lc.setRow(j, 7 - i, openmouth[i]);
              break ;
          }
        }
      }
      pwm.setPWM(3, 0, pulseWidth(RBM + 200));
      pwm.setPWM(0, 0, pulseWidth(LBM - 100));
      delay(300);
      old = current ;
      flag = 0;
    }
    if (current - old2 >= info_time) {
      old2 = current ;
      old = current ;
      start();
      break ;
    }
  }
}
void nojob() {
  start();
  bool flag = true ;
  current = millis();
  old = current ;
  while (flag == true) {
    current = millis();
    if (srf() < 150 ) {
      flag = false ;
      break ;
    }
    if (flag == true && current - old < 3000) {
      for (int j = 0 ; j < 5 ; j++) {
        for (int i = 0 ; i < 8 ; i++) {
          switch (j) {
            case 0 :
              lc.setRow(j, i, mouth[i]);
              break ;
            case 1 :
            case 2 :
              lc.setRow(j, i, openeye[i]);
              break ;
            case 3 :
              lc.setRow(j, 7 - i, mouth[i]);
              break ;
          }
        }
      }
    } else if (flag == true && current - old > 3000) {
      for (int j = 0 ; j < 5 ; j++) {
        for (int i = 0 ; i < 8 ; i++) {
          switch (j) {
            case 0 :
              lc.setRow(j, i, mouth[i]);
              break ;
            case 1 :
            case 2 :
              lc.setRow(j, i, closeeye[i]);
              break ;
            case 3 :
              lc.setRow(j, 7 - i, mouth[i]);
              break ;
          }
        }
      }
    }
    if (flag == true && current - old > 3300) {
      old = current ;
    }
  }
}
int srf () {
  digitalWrite(trig, 0);
  delayMicroseconds(2);
  digitalWrite(trig, 1);
  delayMicroseconds(10);
  digitalWrite(trig, 0);
  duration = pulseIn(echo, 1) ;
  return  distance = duration / 58 ;
}
