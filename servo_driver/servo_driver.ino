#include <SoftwareSerial.h>
#include <DFPlayer.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include "LedControl.h"
//-------------------------------------------------------------------- SRF-05
#define trig 5
#define echo 6
#define limit 150       //Detection distance
//-------------------------------------------------------------------- talk time
#define welcome_time 4000  //time dast tekon dadn (nesbat be file soti avazesh konid)
#define info_time 22000   //time harf zadan (nesbat be file soti avazesh konid)
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
#define SERVOMIN  150
#define SERVOMAX  600
#define hand_angle 70   // maximum angle of hands
#define arm_angle 70    // maximum angle of arms
#define slow 5         //delay betwen servo motion
#define RH 3            // Right Hand
#define LH 0            // Left Hand
#define RA 2            // Right Arm
#define LA 1            // Leftt Arm
#define SPACE 270
#define SPACE_2 300
#define SPACE_3 300
//-------------------------------------------------------------------- variable
unsigned long current = 0 , old = 0 , old2 = 0 , duration = 0 , distance = 0 ;
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
//--------------------------------------------------------------------
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
  pwm.setPWMFreq(50);  // Analog servos run at ~60 Hz updates
  yield();
  //------------------------------------------------------------------ srf
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(3, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(3, 1);
  digitalWrite(10, 0);
  //start();
}
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;

  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  Serial.print(pulselength); Serial.println(" us per period");
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit");
  pulse *= 1000;
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}
void loop() {
  if (srf() < limit && srf() != 0) {
    welcome();
    info();
    delay(3000);
  }
  else {
    nojob();
  }
}
/*void start() {
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
  pwm.setPWM(LH, 0, SERVOMAX);     // left blue
  pwm.setPWM(RA, 0, SERVOMAX);    // right black
  pwm.setPWM(LA, 0, SERVOMIN);    // left black
  pwm.setPWM(RH, 0, SERVOMAX);     // right blue
}*/
void welcome() {
  mp3.wakeup(2);
  mp3.playTrack(1);
  unsigned long old = 0 , old2 = 0 , break_time = 0;
  current = millis();
  old = current ;
  old2 = current ;
  break_time = current ;
  bool flag = false ;
  int current_state = 0 ;
  arm("right","up");
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
    if (current - old2 >= 1000 && flag==false) {
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
      hand("right", "up");
      old2=current ;
      flag=true ;
    } else  if (current - old2 >= 2500 && flag==true ) {
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
      hand("right", "down");
      flag=false ;
      old2 = current ;
    }
    if (current - break_time >= welcome_time) {
      break_time = current ;
      hand("right", "down");
      break ;
    }
  }
  arm("right","down");
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
      arm("all", "up");
      hand("all", "up");
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
      hand("all", "down");
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
      hand("all", "up");
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
      arm("all", "down");
      hand("all", "down");
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
      hand("all", "up");
      old = current ;
      flag = 5 ;
    }
    if (current - old >= 500 && flag == 5) {
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
      hand("all", "down");
      delay(300);
      old = current ;
      flag = 0;
    }
    if (current - old2 >= info_time) {
      old2 = current ;
      old = current ;
      hand("all", "down");
      arm("all", "down");
      break ;
    }
  }
}
void nojob() {
  bool flag = true ;
  current = millis();
  old = current ;
  while (flag == true) {
    current = millis();
    if (srf() < limit && srf() != 0) {
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
    if (flag == true && current - old > 4000) {
      old = current ;
    }
  }
}
void arm(String side , String mode) {
  if (side == "left") {
    if (mode == "up") {
      for (int i = SERVOMAX ; i >= SERVOMIN + SPACE_3 ; i--) {
        pwm.setPWM(LA, 0, i);
        delay(slow);
      }
    } else {
      for (int i = SERVOMIN + SPACE_3 ; i <= SERVOMAX; i++) {
        pwm.setPWM(LA, 0, i);
        delay(slow);
      }
    }
  } else if (side == "right") {
    if (mode == "up") {
      for (int i = SERVOMIN ; i <= SERVOMAX - SPACE_3 ; i++) {
        pwm.setPWM(RA, 0, i);
        delay(slow);
      }
    } else {
      for (int i = SERVOMAX - SPACE_3 ; i >= SERVOMIN; i--) {
        pwm.setPWM(RA, 0, i);
        delay(slow);
      }
    }
  } else if (side == "all") {
    if (mode == "up") {
      for (int i = SERVOMAX , j = SERVOMIN+SPACE_3; i >= SERVOMIN + SPACE_3 ; i-- , j++) {
        pwm.setPWM(LA, 0, i);
        pwm.setPWM(RA, 0, j);
        delay(slow);
      }
    } else {
      for (int i = SERVOMIN + SPACE_3 , j = SERVOMAX; i <= SERVOMAX; i++ , j--) {
        pwm.setPWM(LA, 0, i);
        pwm.setPWM(RA, 0, j);
        delay(slow);
      }
    }
  }
}
void hand(String side , String mode) {
  if (side == "left") {
    if (mode == "up") {
      for (int i = SERVOMAX-SPACE ; i >=  SERVOMIN ; i--) {
        pwm.setPWM(LH, 0, i);
        delay(slow+5);
      }
    } else {
      for (int i = SERVOMIN ; i <= SERVOMAX-SPACE ; i++) {
        pwm.setPWM(LH, 0, i);
        delay(slow+5);
      }
    }
  } else if (side == "right") {
    if (mode == "up") {
      for (int i = SERVOMIN ; i <=  SERVOMAX-SPACE ; i++) {
        pwm.setPWM(RH, 0, i);
        delay(slow+5);
      }
    } else {
      for (int i = SERVOMAX-SPACE ; i >= SERVOMIN ; i--) {
        pwm.setPWM(RH, 0, i);
        delay(slow+5);
      }
    }
  } else if (side == "all") {
    if (mode == "up") {
      for (int i = SERVOMIN , j=SERVOMAX-SPACE; i <=  SERVOMAX-SPACE ; i++ , j--) {
        pwm.setPWM(LH, 0, j);
        pwm.setPWM(RH, 0, i);
        delay(slow);
      }
    } else {
      for (int i = SERVOMAX-SPACE , j=SERVOMIN ; i >= SERVOMIN ; i-- , j++) {
        pwm.setPWM(LH, 0, j);
        pwm.setPWM(RH, 0, i);
        delay(slow);
        
      }
    }
  }
}
int srf () {
  digitalWrite(trig, 0);
  delayMicroseconds(2);
  digitalWrite(trig, 1);
  delayMicroseconds(10);
  digitalWrite(trig, 0);
  duration = pulseIn(echo, 1 , 10000) ;
  return  distance = duration / 58 ;
}
