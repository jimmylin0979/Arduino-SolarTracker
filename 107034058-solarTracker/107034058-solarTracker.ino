#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Photoresistance
int ldrTL = A2;   //top left LDR green
int ldrTR = A1;   //top right LDR yellow
int ldrBL = A3;   // bottom left LDR blue
int ldrBR = A0;   // bottom right LDR orange

//defining Servos
Servo servoHoriz;
int servoH = 0;
int servoHLimitHigh = 180;
int servoHLimitLow = 0;

Servo servoVerti;
int servoV = 0;
int servoVLimitHigh = 160;
int servoVLimitLow = 0;

int tolerance = 10;

void setup() {
    // Photoresistance
    pinMode(ldrTL, INPUT);
    pinMode(ldrTR, INPUT);
    pinMode(ldrBL, INPUT);
    pinMode(ldrBR, INPUT);

    // servo
    servoHoriz.attach(10);
    servoVerti.attach(9);
    servoHoriz.write(80);
    servoVerti.write(80);

    // serial
    Serial.begin(9600);

    // lcd setup
    lcd.init();
    lcd.backlight();
    lcd.print(" Solar  Tracker ");

    delay(1000);
}

int axisMove(int avgValue_Incr, int avgValue_Decr, int servoAngle) {
    int diff = avgValue_Incr - avgValue_Decr;
    int absDiff = diff < 0? -diff : diff;
    // if diff is smaller than tolerance, do not change the current angle
    if(absDiff < tolerance) {
      return servoAngle;
    }
    
    if (diff > 0) {
        if (servoAngle >= servoVLimitHigh) {
            servoAngle = servoVLimitHigh;
        } else {
            servoAngle = servoAngle + 1;
        }
        return servoAngle;
    } else if (diff < 0) {
        if (servoAngle <= servoVLimitLow) {
            servoAngle = servoVLimitLow;
        } else {
            servoAngle = servoAngle - 1;
        }
        return servoAngle;
    } else {
      return servoAngle;
    }
}

int recordTimes = 0;
int ROUNDS = 5;

int cumulativeTop = 0, cumulativeBot = 0;
int cumulativeLeft = 0, cumulativeRight = 0;

void loop() {
    // get the current angle of Servos
    servoH = servoHoriz.read();
    servoV = servoVerti.read();

    // get the value of LDRs
    int topl = analogRead(ldrTL);
    int topr = analogRead(ldrTR);
    int botl = analogRead(ldrBL);
    int botr = analogRead(ldrBR);

  
    Serial.print("topl = ");
    Serial.print(topl);
    Serial.print(", topr = ");
    Serial.print(topr);
    Serial.print(", botl = ");
    Serial.print(botl);
    Serial.print(", botr = ");
    Serial.print(botr);
    Serial.println();
    

    // calculating average
    int avgTop = (topl + topr) / 2;     //average of top LDRs
    int avgBot = (botl + botr) / 2;     //average of bottom LDRs
    int avgLeft = (topl + botl) / 2;    //average of left LDRs
    int avgRight = (topr + botr) / 2;   //average of right LDRs

    // display one lcd screen
    lcd.setCursor(0, 0);
    lcd.print("Top=");
    lcd.print(avgTop);
    lcd.print(";Bot=");
    lcd.print(avgBot);
    lcd.setCursor(0, 1);
    lcd.print("Lef=");
    lcd.print(avgLeft);
    lcd.print(";Rig=");
    lcd.print(avgRight);
    
    /*
    Serial.print("avgTop = ");
    Serial.print(avgTop);
    Serial.print(", avgBot = ");
    Serial.print(avgBot);
    Serial.print(", avgLeft = ");
    Serial.print(avgLeft);
    Serial.print(", avgRight = ");
    Serial.print(avgRight);

    Serial.print(";  servoH = ");
    Serial.print(servoH);
    Serial.print(", servoV = ");
    Serial.println(servoV);
    */

    // control servo to the appropiate angle
    int resV = axisMove(avgTop, avgBot, servoV);
    servoVerti.write(resV);
    
    int resH = axisMove(avgRight, avgLeft, servoH);
    servoHoriz.write(resH);

    delay(5);
}
