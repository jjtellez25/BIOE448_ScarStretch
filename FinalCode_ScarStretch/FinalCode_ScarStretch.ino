//This example implements a simple sliding On/Off button. The example
// demonstrates drawing and touch operations.
//
//Thanks to Adafruit forums member Asteroid for the original sketch!
//
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include <ezBuzzer.h>

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ts = Adafruit_FT6206();

#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

boolean RecordOn = false;

const int yellowPin = 24; // pin for pause button
const int greenPin = 28; // pin for start button
const int redPin = 32; // pin for main menu button 

int greenState = 0;
int yellowState = 0;
int redState = 0;
int buttonCounter = 0; //timer
int sec = 0; //timer
int min = 0; //timer

unsigned long startMillis; //timer
unsigned long pauseMillis; //timer
unsigned long elapsedMillis; //timer
unsigned long calculateMillis;
unsigned long previousMillis;
unsigned long previousEndMillis;
unsigned long currentEndMillis;
unsigned long motorMillis;
int endcount = 0;
const long interval = 1000; //for time change and end flashing

// buzzer
const int BUZZER_PIN = 41;

ezBuzzer buzzer(BUZZER_PIN);

int melodyCompletion[] = 
{
  // notes in the melody
  NOTE_C6,
  NOTE_C6, NOTE_D6, NOTE_E6, NOTE_F6, NOTE_G6, NOTE_A6, NOTE_B6,
  NOTE_C7
};
int noteDurationCompletion[] = 
{
  // note durations: 4 = quarter note, 8 = eighth note, etc
  4,
  32, 32, 32, 32, 32, 32, 32,
  2,
};
int melodyError[] = 
{
  // notes in the melody
  NOTE_E6,
};
int noteDurationError[] = 
{
  // note durations: 4 = quarter note, 8 = eighth note, etc
  2,
};

int buzzercounter = 0;
int endInit = 0;
int errInit = 0;
unsigned long buzzerPauseMillis;

#define MOTOR_IN1  42
#define MOTOR_IN2  36
#define LIMIT_SWITCH_PIN1 46
#define LIMIT_SWITCH_PIN2 50
int motorcounter = 0;

#define START_X 75
#define START_Y 190
#define START_W 170
#define START_H 50

#define GARR_X 135
#define GARR_Y 170
#define GARR_W 50
#define GARR_H 60 

#define CNT_Y 70
#define CNT1_X 20
#define CNT2_X 85 
#define CNT3_X 157
#define CNT4_X 230

#define TIME_X CNT1_X
#define TIME_Y CNT_Y
#define TIME_W 280
#define TIME_H 100

#define PAUSE_X 0
#define PAUSE_Y 190
#define PAUSE_W 106
#define PAUSE_H 50

#define STOP_X 214
#define STOP_Y PAUSE_Y
#define STOP_W PAUSE_W
#define STOP_H PAUSE_H

#define RESUME_X 95
#define RESUME_Y PAUSE_Y
#define RESUME_W 120
#define RESUME_H PAUSE_H

#define END_X 20
#define END_Y 20
#define END_W 280
#define END_H 200

#define R1_X 0
#define R1_Y 0 
#define R1_W 320
#define R1_H 20

#define R2_X 0
#define R2_Y 20
#define R2_W 20
#define R2_H 200

#define R3_X 0
#define R3_Y 220
#define R3_W 184
#define R3_H 20

#define R4_X 300
#define R4_Y 20
#define R4_W 30
#define R4_H 170

#define ERR1_X 0 
#define ERR1_Y 0 
#define ERR1_W 320
#define ERR1_H 40

#define ERR2_X 0 
#define ERR2_Y 40
#define ERR2_W 40
#define ERR2_H 110

#define ERR3_X 0 
#define ERR3_Y 150
#define ERR3_W 320
#define ERR3_H 40

#define ERR4_X 280 
#define ERR4_Y 40
#define ERR4_W 40
#define ERR4_H 110

#define ERR5_X 0
#define ERR5_Y RESUME_Y
#define ERR5_W 64
#define ERR5_H RESUME_H

#define ERR_tx_X 60 
#define ERR_tx_Y 70

void motor() {
  // Serial.println(LIMIT_SWITCH_PIN1);
  // Serial.print(LIMIT_SWITCH_PIN2);

  if(digitalRead(LIMIT_SWITCH_PIN1) == HIGH) {
    motorcounter = 1;
    motorMillis = millis();
  }

  if(digitalRead(LIMIT_SWITCH_PIN2) == HIGH) {
    motorcounter = 0;
    motorMillis = millis();
  }

  if (motorcounter == 1) {
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
  }

  if (motorcounter == 0) {
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
  }


}

void startMsg() {

  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);

  tft.fillRect(START_X, START_Y, START_W, START_H,ILI9341_GREEN);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(3);
  tft.setCursor(60, 40);
  tft.println("Presione el");
  tft.setCursor(57, 80);
  tft.println("boton verde");
  tft.setCursor(50, 120);
  tft.println("para comenzar");
  tft.setCursor((START_X+15),(START_Y+15));
  tft.println("COMIENCE");
}


void endMsg()
{
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);

  if(endInit == 0) {
    tft.fillRect(R1_X,R1_Y,R1_W,R1_H,ILI9341_GREEN);
    tft.fillRect(R2_X,R2_Y,R2_W,R2_H,ILI9341_GREEN);
    tft.fillRect(R3_X,R3_Y,R3_W,R3_H,ILI9341_GREEN);
    tft.fillRect(R4_X,R4_Y,R4_W,R4_H,ILI9341_GREEN);
    endInit = 1;
  }

  buzzer.loop();
  if (buzzercounter == 0) {
    if (buzzer.getState() == BUZZER_IDLE) {
      int lengthCompletion = sizeof(noteDurationCompletion) / sizeof(int);
      buzzer.playMelody(melodyCompletion, noteDurationCompletion, lengthCompletion); // playing
    }
    buzzercounter = 1;
  }

  if (buzzercounter == 1 && buzzer.getState() == BUZZER_IDLE) {
    if ((millis()/1000)%2 == 0) {
      tft.fillRect(R1_X,R1_Y,R1_W,R1_H,ILI9341_GREEN);
      tft.fillRect(R2_X,R2_Y,R2_W,R2_H,ILI9341_GREEN);
      tft.fillRect(R3_X,R3_Y,R3_W,R3_H,ILI9341_GREEN);
      tft.fillRect(R4_X,R4_Y,R4_W,R4_H,ILI9341_GREEN);
    } 

    if ((millis()/1000)%2 == 1) {
      tft.fillRect(R1_X,R1_Y,R1_W,R1_H,ILI9341_WHITE);
      tft.fillRect(R2_X,R2_Y,R2_W,R2_H,ILI9341_WHITE);
      tft.fillRect(R3_X,R3_Y,R3_W,R3_H,ILI9341_WHITE);
      tft.fillRect(R4_X,R4_Y,R4_W,R4_H,ILI9341_WHITE);
    } 
  }


  if (redState == HIGH) {
    buttonCounter = 0;
    buzzercounter = 0;
    endInit = 0;
    tft.fillScreen(ILI9341_WHITE);
    if (buzzer.getState() != BUZZER_IDLE) {
      buzzer.stop() ; // stop
    }
    startMsg();
  }
}

void errMsg() {

  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);

  if (errInit == 3) {
    if ((millis()/1000)%2 == 0) {
      tft.fillRect(ERR1_X,ERR1_Y,ERR1_W,ERR1_H,ILI9341_RED);
      tft.fillRect(ERR2_X,ERR2_Y,ERR2_W,ERR2_H,ILI9341_RED);
      tft.fillRect(ERR4_X,ERR4_Y,ERR4_W,ERR4_H,ILI9341_RED);
      tft.fillRect(ERR3_X,ERR3_Y,ERR3_W,ERR3_H,ILI9341_RED);
      tft.fillRect(ERR5_X,ERR5_Y,ERR5_W,ERR5_H,ILI9341_RED);
    } 
    if ((millis()/1000)%2 == 1) {
      tft.fillRect(ERR1_X,ERR1_Y,ERR1_W,ERR1_H,ILI9341_WHITE);
      tft.fillRect(ERR2_X,ERR2_Y,ERR2_W,ERR2_H,ILI9341_WHITE);
      tft.fillRect(ERR4_X,ERR4_Y,ERR4_W,ERR4_H,ILI9341_WHITE);
      tft.fillRect(ERR3_X,ERR3_Y,ERR3_W,ERR3_H,ILI9341_WHITE);
      tft.fillRect(ERR5_X,ERR5_Y,ERR5_W,ERR5_H,ILI9341_WHITE);
    } 
  }

  if (errInit < 3) {
    buzzer.loop();
    if (buzzercounter == 0) {
      if (buzzer.getState() == BUZZER_IDLE) {
        int lengthError = sizeof(noteDurationError) / sizeof(int);
        buzzer.playMelody(melodyError, noteDurationError, lengthError); // playing
        buzzercounter++;
        tft.fillRect(ERR1_X,ERR1_Y,ERR1_W,ERR1_H,ILI9341_RED);
        tft.fillRect(ERR2_X,ERR2_Y,ERR2_W,ERR2_H,ILI9341_RED);
        tft.fillRect(ERR4_X,ERR4_Y,ERR4_W,ERR4_H,ILI9341_RED);
        tft.fillRect(ERR3_X,ERR3_Y,ERR3_W,ERR3_H,ILI9341_RED);
        tft.fillRect(ERR5_X,ERR5_Y,ERR5_W,ERR5_H,ILI9341_RED);
      }
    }
    if (buzzercounter == 1) {
      if (buzzer.getState() == BUZZER_IDLE) {
        int lengthError = sizeof(noteDurationError) / sizeof(int);
        buzzer.playMelody(melodyError, noteDurationError, lengthError); // playing
        buzzercounter++;
        tft.fillRect(ERR1_X,ERR1_Y,ERR1_W,ERR1_H,ILI9341_WHITE);
        tft.fillRect(ERR2_X,ERR2_Y,ERR2_W,ERR2_H,ILI9341_WHITE);
        tft.fillRect(ERR4_X,ERR4_Y,ERR4_W,ERR4_H,ILI9341_WHITE);
        tft.fillRect(ERR3_X,ERR3_Y,ERR3_W,ERR3_H,ILI9341_WHITE);
        tft.fillRect(ERR5_X,ERR5_Y,ERR5_W,ERR5_H,ILI9341_WHITE);
      }
    }
    if (buzzercounter == 2) {
      if (buzzer.getState() == BUZZER_IDLE) {
        int lengthError = sizeof(noteDurationError) / sizeof(int);
        buzzer.playMelody(melodyError, noteDurationError, lengthError); // playing
        buzzercounter++;
        tft.fillRect(ERR1_X,ERR1_Y,ERR1_W,ERR1_H,ILI9341_RED);
        tft.fillRect(ERR2_X,ERR2_Y,ERR2_W,ERR2_H,ILI9341_RED);
        tft.fillRect(ERR4_X,ERR4_Y,ERR4_W,ERR4_H,ILI9341_RED);
        tft.fillRect(ERR3_X,ERR3_Y,ERR3_W,ERR3_H,ILI9341_RED);
        tft.fillRect(ERR5_X,ERR5_Y,ERR5_W,ERR5_H,ILI9341_RED);
      }
    }
    if (buzzercounter == 3) {
      if (buzzer.getState() == BUZZER_IDLE) {
        tft.fillRect(ERR1_X,ERR1_Y,ERR1_W,ERR1_H,ILI9341_WHITE);
        tft.fillRect(ERR2_X,ERR2_Y,ERR2_W,ERR2_H,ILI9341_WHITE);
        tft.fillRect(ERR4_X,ERR4_Y,ERR4_W,ERR4_H,ILI9341_WHITE);
        tft.fillRect(ERR3_X,ERR3_Y,ERR3_W,ERR3_H,ILI9341_WHITE);
        tft.fillRect(ERR5_X,ERR5_Y,ERR5_W,ERR5_H,ILI9341_WHITE);
        buzzercounter++;
        buzzerPauseMillis = millis();
      }
    }
    if (buzzercounter == 4 && (millis() - buzzerPauseMillis) >= 1000) {
      buzzercounter = 0;
      errInit++;
    }
  }


  if (redState == HIGH) {
    buttonCounter = 0;
    buzzercounter = 0;
    errInit = 0;
    tft.fillScreen(ILI9341_WHITE);
    if (buzzer.getState() != BUZZER_IDLE) {
      buzzer.stop() ; // stop
    }
    startMsg();
  }

  if (greenState == HIGH) {
    tft.fillScreen(ILI9341_WHITE);
    buzzercounter = 0;
    errInit = 0;
    if (buzzer.getState() != BUZZER_IDLE) {
      buzzer.stop() ; // stop
    }
    tft.fillRect(TIME_X, TIME_Y, TIME_W, TIME_H,ILI9341_WHITE);
    buttonCounter = 10;
    tft.fillRect(RESUME_X,RESUME_Y,RESUME_W,RESUME_H,ILI9341_WHITE);
    tft.fillRect(PAUSE_X,PAUSE_Y,PAUSE_W,PAUSE_H,ILI9341_YELLOW);
    tft.setCursor((PAUSE_X+10),(PAUSE_Y+15));
    tft.setTextSize(3);
    tft.println("PAUSA");
    tft.fillRect(STOP_X,STOP_Y,STOP_W,STOP_H,ILI9341_RED);
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor((STOP_X+25),(STOP_Y+15));
    tft.println("ALTO");
    elapsedMillis = millis() - pauseMillis;
    startMillis = startMillis + elapsedMillis;
    motorMillis = millis();
  }
  
}

void setup(void)
{
  Serial.begin(9600);
  tft.begin();
  if (!ts.begin(40)) { 
    Serial.println("Unable to start touchscreen.");
  } 
  else { 
    Serial.println("Touchscreen started."); 
  }

  tft.fillScreen(ILI9341_WHITE);
  // origin = left,top landscape (USB left upper)
  tft.setRotation(1); 

  pinMode(greenPin, INPUT_PULLUP);
  pinMode(yellowPin, INPUT_PULLUP);
  pinMode(redPin, INPUT_PULLUP);

  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(LIMIT_SWITCH_PIN1, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH_PIN2, INPUT_PULLUP);

  startMsg();

}


void loop()
{
  greenState = digitalRead(greenPin);
  yellowState = digitalRead(yellowPin);
  redState = digitalRead(redPin);

  // Serial.print(buttonCounter);
  // Serial.print(redState);
  // Serial.println(greenState);
  // Serial.println(yellowState);

  // if statement starts the countdown
  if (greenState == HIGH && buttonCounter == 0) {
    tft.fillScreen(ILI9341_WHITE);
    startMillis = millis();

    tft.setTextSize(3);
    tft.setTextColor(ILI9341_BLACK);

    tft.setCursor(25,20);
    tft.println("Tiempo restante");
    tft.fillRect(PAUSE_X,PAUSE_Y,PAUSE_W,PAUSE_H,ILI9341_YELLOW);
    tft.setCursor((PAUSE_X+10),(PAUSE_Y+15));
    tft.println("PAUSA");
    tft.fillRect(STOP_X,STOP_Y,STOP_W,STOP_H,ILI9341_RED);
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor((STOP_X+25),(STOP_Y+15));
    tft.println("ALTO");
    buttonCounter = 8;

    tft.setTextColor(ILI9341_BLACK);
    tft.setTextSize(13);
    tft.setCursor(CNT1_X,CNT_Y);
    tft.print(9);
    tft.setCursor(CNT2_X,CNT_Y);
    tft.print(":");
    tft.setCursor(CNT3_X,CNT_Y);
    tft.print(5);
    tft.setCursor(CNT4_X,CNT_Y);
    tft.print(9);

    motorMillis = millis();
    previousMillis = millis();
  }

  // if statement runs through the countdown
  if (buttonCounter == 8){
    
    // if (calculateMillis != (599-int((millis()-startMillis)/1000))%60) {
    //   tft.fillRect(TIME_X, TIME_Y, TIME_W, TIME_H,ILI9341_WHITE);
    // }

    if(millis() - previousMillis >= interval) {
      previousMillis = millis();
      calculateMillis = (599-int((millis()-startMillis)/1000))%60;

      tft.fillRect(TIME_X, TIME_Y, TIME_W, TIME_H,ILI9341_WHITE);
      // tft.print(int((599-int((previousMillis-startMillis)/1000))/60), ILI9341_WHITE);

      tft.setTextSize(13);
      tft.setCursor(CNT1_X,CNT_Y);
      tft.print(int((599-int((millis()-startMillis)/1000))/60));
      tft.setCursor(CNT2_X,CNT_Y);
      tft.print(":"); 

      if ((calculateMillis) >= 10) {
        tft.setCursor((CNT3_X),CNT_Y);
        tft.print(calculateMillis);
      }
      
      else {
        tft.setCursor(CNT3_X, CNT_Y);
        tft.print("0");
        tft.setCursor((CNT4_X), CNT_Y);
        tft.print(calculateMillis);
      }
      
      // delay(100);
      

    }

    if ((599-int((millis()-startMillis)/1000)) == 0) {
      buttonCounter = 2;
      tft.fillScreen(ILI9341_WHITE);

      tft.fillRect(END_X,END_Y,END_W,END_H,ILI9341_PURPLE);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(5);
      tft.setCursor(80,50);
      tft.println("MASAJE");
      tft.setCursor(30,130);
      tft.println("TERMINADO");
      tft.fillRect(STOP_X-30,STOP_Y,STOP_W+30,STOP_H,ILI9341_RED);
      // tft.fillRect(STOP_X-30,STOP_Y,3.5,STOP_H,ILI9341_BLACK);
      // tft.fillRect(STOP_X-30,STOP_Y,STOP_W+30,2,ILI9341_BLACK);
      tft.setCursor((STOP_X-20),(STOP_Y+15));
      tft.setTextSize(3);
      tft.println("REGRESA");
    }
  
  motor();

  }

  // if statement checks to see if it is paused. if paused, we take time and hold it until button is resumed again
  if (yellowState == HIGH && buttonCounter >= 7) {
    pauseMillis = millis();

    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);

    if (calculateMillis != (599-int((millis()-startMillis)/1000))%60) {
      tft.fillRect(TIME_X, TIME_Y, TIME_W, TIME_H,ILI9341_WHITE);
    }
    calculateMillis = (599-int((millis()-startMillis)/1000))%60;

    tft.setTextSize(13);
    tft.setCursor(CNT1_X,CNT_Y);
    tft.print(int((599-int((millis()-startMillis)/1000))/60));
    tft.setCursor(CNT2_X, CNT_Y);
    tft.print(":"); 

    if ((calculateMillis) >= 10) {
      tft.setCursor(CNT3_X, CNT_Y);
      tft.print(calculateMillis);
    }
    
    else {
      tft.setCursor(CNT3_X, CNT_Y);
      tft.print("0");
      tft.setCursor(CNT4_X, CNT_Y);
      tft.print(calculateMillis);
    }
    
    // delay(100);

    buttonCounter = 5;
    
    tft.fillRect(PAUSE_X,PAUSE_Y,PAUSE_W,PAUSE_H,ILI9341_WHITE);
    tft.fillRect(RESUME_X,RESUME_Y,RESUME_W,RESUME_H,ILI9341_GREEN);
    tft.setCursor((RESUME_X+8),(RESUME_Y+15));
    tft.setTextSize(3);
    tft.println("RESUME");
    
  }

  if (greenState == HIGH && buttonCounter == 5) {
    // tft.fillScreen(ILI9341_BLUE);
    elapsedMillis = millis() - pauseMillis;
    startMillis = startMillis + elapsedMillis;
    tft.fillRect(TIME_X, TIME_Y, TIME_W, TIME_H,ILI9341_WHITE);
    buttonCounter = 10;
    tft.fillRect(RESUME_X,RESUME_Y,RESUME_W,RESUME_H,ILI9341_WHITE);
    tft.fillRect(PAUSE_X,PAUSE_Y,PAUSE_W,PAUSE_H,ILI9341_YELLOW);
    tft.setCursor((PAUSE_X+10),(PAUSE_Y+15));
    tft.setTextSize(3);
    tft.println("PAUSA");

    motorMillis = millis();
  }

  if (buttonCounter == 10){
    
    if(millis() - previousMillis >= interval) {
      previousMillis = millis();
      calculateMillis = (599-int((millis()-startMillis)/1000))%60;

      tft.fillRect(TIME_X, TIME_Y, TIME_W, TIME_H,ILI9341_WHITE);
      // tft.print(int((599-int((previousMillis-startMillis)/1000))/60), ILI9341_WHITE);

      tft.setTextSize(13);
      tft.setCursor(CNT1_X,CNT_Y);
      tft.print(int((599-int((millis()-startMillis)/1000))/60));
      tft.setCursor(CNT2_X,CNT_Y);
      tft.print(":"); 

      if ((calculateMillis) >= 10) {
        tft.setCursor((CNT3_X),CNT_Y);
        tft.print(calculateMillis);
      }
      
      else {
        tft.setCursor(CNT3_X, CNT_Y);
        tft.print("0");
        tft.setCursor((CNT4_X), CNT_Y);
        tft.print(calculateMillis);
      }
      
      // delay(100);
      

    }

    if ((599-int((millis()-startMillis)/1000)) == 0) {
      buttonCounter = 2;
      tft.fillScreen(ILI9341_WHITE);

      tft.fillRect(END_X,END_Y,END_W,END_H,ILI9341_PURPLE);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(5);
      tft.setCursor(80,50);
      tft.println("MASAJE");
      tft.setCursor(30,130);
      tft.println("TERMINADO");
      tft.fillRect(STOP_X-30,STOP_Y,STOP_W+30,STOP_H,ILI9341_RED);
      // tft.fillRect(STOP_X-30,STOP_Y,3.5,STOP_H,ILI9341_BLACK);
      // tft.fillRect(STOP_X-30,STOP_Y,STOP_W+30,2,ILI9341_BLACK);
      tft.setCursor((STOP_X-20),(STOP_Y+15));
      tft.setTextSize(3);
      tft.println("REGRESA");
    }

  motor();

  }

  if ((buttonCounter == 10 || buttonCounter == 8) && ((millis() - motorMillis) > 3000)) {
    buttonCounter = 6;
    pauseMillis = millis();
    tft.fillScreen(ILI9341_WHITE);

    tft.setCursor(ERR_tx_X,ERR_tx_Y);
    tft.setTextSize(7);
    tft.setTextColor(ILI9341_BLACK);
    tft.println("ERROR");

    tft.fillRect(STOP_X-30,STOP_Y,STOP_W+30,STOP_H,ILI9341_RED);
    tft.fillRect(STOP_X-30,STOP_Y,3.5,STOP_H,ILI9341_BLACK);
    tft.fillRect(STOP_X-30,STOP_Y,STOP_W+30,2,ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor((STOP_X-20),(STOP_Y+15));
    tft.setTextSize(3);
    tft.println("REGRESA");

    tft.fillRect(64,RESUME_Y,RESUME_W,RESUME_H,ILI9341_GREEN);
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(71,RESUME_Y+15);
    tft.setTextSize(3);
    tft.println("RESUME");

  }

  if (buttonCounter == 6) {
    errMsg();
  }

// if stop button is pressed, it immediately goes back to initial screen.
  if (redState == HIGH) {
    buttonCounter = 0;
    tft.fillScreen(ILI9341_WHITE);

    startMsg();
    // endMsg();
  }

  if (buttonCounter == 2) {
    
    endMsg();
  }


}