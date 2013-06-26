
#include <Servo.h>
 
Servo myservo;
const int  inputbutton1 = 25;
const int inputbutton2 = 27;
const int inputbutton3 = 29;
const int redled1 = 22;
const int greenled1 = 23;
const int redled2 = 24;
const int greenled2 = 26;
const int redled3 = 30;
const int greenled3 = 28;
const int alarmbutton = 52;
const int alarmled = 50;
int smokeSensorValue = 0;
const int smokeAnalogInPin = A2;
const int smokeAnalogled = 2;
const int ultrasensorOutput = 11;
int ultrasensorled = 12;
int SoundsensorPin = 5;
int Soundsensorled = 4;
int SoundsensorValue = 0;
int buzzer = 53;
int servo = 7;


int inputbutton1_pushcounter = 0;
int inputbutton2_pushcounter = 0;
int inputbutton3_pushcounter = 0;
int inputbutton1_currentState = 0;         
int inputbutton2_currentState = 0;
int inputbutton3_currentState = 0;
int inputbutton1_lastState = 0;
int inputbutton2_lastState = 0;
int inputbutton3_lastState = 0;
int alarmbutton_pushcounter = 0;
int alarmbutton_currentState = 0;
int alarmbutton_lastState = 0;

void setup() {
  Serial.begin(9600);
  pinMode(inputbutton1, INPUT);
  pinMode(inputbutton2,INPUT);
  pinMode(inputbutton3,INPUT);
  pinMode(alarmbutton,INPUT);
  pinMode(alarmled, OUTPUT);  
  pinMode(redled1, OUTPUT);
  pinMode(greenled1, OUTPUT);
  pinMode(redled2,OUTPUT);
  pinMode(greenled2,OUTPUT);
  pinMode(redled3,OUTPUT);
  pinMode(greenled3,OUTPUT);
  pinMode(ultrasensorOutput, OUTPUT); 
  pinMode(ultrasensorled, OUTPUT);
  pinMode(buzzer, OUTPUT);
  myservo.attach(servo);
  myservo.write(0);
}


void loop() {
  // read the pushbutton input pin:
  inputbutton1_currentState = digitalRead(inputbutton1);
  inputbutton2_currentState = digitalRead(inputbutton2);
  inputbutton3_currentState = digitalRead(inputbutton3);
  alarmbutton_currentState = digitalRead(alarmbutton);
  
  if (alarmbutton_currentState != alarmbutton_lastState) {
    if (alarmbutton_currentState == HIGH) {
      alarmbutton_pushcounter++;
    }
  }
  
  // compare the buttonState to its previous state
  if (inputbutton1_currentState != inputbutton1_lastState) {
    // if the state has changed, increment the counter
    if (inputbutton1_currentState == HIGH) {
      inputbutton1_pushcounter++;
      if (inputbutton1_pushcounter > 3) {
        inputbutton1_pushcounter = 0;
      }
    } 
  }
  
  if (inputbutton2_currentState != inputbutton2_lastState) {
    if (inputbutton2_currentState == HIGH) {
      inputbutton2_pushcounter++;
      if (inputbutton2_pushcounter > 3) {
        inputbutton2_pushcounter = 0;
      }
    } 
  }
  
  if (inputbutton3_currentState != inputbutton3_lastState) {
    if (inputbutton3_currentState == HIGH) {
      inputbutton3_pushcounter++;
      if (inputbutton3_pushcounter > 3) {
        inputbutton3_pushcounter = 0;
      }
    } 
  }
  
  // save the current state as the last state, 
  //for next time through the loop
  inputbutton1_lastState = inputbutton1_currentState;
  inputbutton2_lastState = inputbutton2_currentState;
  inputbutton3_lastState = inputbutton3_currentState;
  alarmbutton_lastState = alarmbutton_currentState;

  if (alarmbutton_pushcounter % 2 == 0) {
    digitalWrite(alarmled, LOW);
  } else {
    digitalWrite(alarmled, HIGH);
  }

  if (inputbutton1_pushcounter == 0) {
    digitalWrite(redled1, LOW);
    digitalWrite(greenled1, LOW);
  } else {
    if (inputbutton1_pushcounter == 1) {
      digitalWrite(redled1, HIGH);
      digitalWrite(greenled1, LOW);
    } else {
      if (inputbutton1_pushcounter == 2) {
        digitalWrite(greenled1, HIGH);
        digitalWrite(redled1, LOW);
      } else {
          if (inputbutton1_pushcounter == 3) {
            digitalWrite(greenled1, HIGH);
            digitalWrite(redled1, HIGH);
          }
      }
    }
  }
  
  if (inputbutton2_pushcounter == 0) {
    digitalWrite(redled2, LOW);
    digitalWrite(greenled2, LOW);
  } else {
    if (inputbutton2_pushcounter == 1) {
      digitalWrite(redled2, HIGH);
      digitalWrite(greenled2, LOW);
    } else {
      if (inputbutton2_pushcounter == 2) {
        digitalWrite(greenled2, HIGH);
        digitalWrite(redled2, LOW);
      } else {
          if (inputbutton2_pushcounter == 3) {
            digitalWrite(redled2, HIGH);
            digitalWrite(greenled2, HIGH);
          }
      }
    }
  }
  
  if (inputbutton3_pushcounter == 0) {
    digitalWrite(redled3, LOW);
    digitalWrite(greenled3, LOW);
  } else {
    if (inputbutton3_pushcounter == 1) {
//      wdt_enable(WDTO_30MS);
      digitalWrite(redled3, HIGH);
      digitalWrite(greenled3, LOW);
    } else {
      if (inputbutton3_pushcounter == 2) {
        digitalWrite(greenled3, HIGH);
        digitalWrite(redled3, LOW);
      } else {
          if (inputbutton3_pushcounter == 3) {
            digitalWrite(redled3, HIGH);
            digitalWrite(greenled3, HIGH);
          }
      }
    }
  }
  
  if(inputbutton1_pushcounter == 1 && inputbutton2_pushcounter == 3 && inputbutton3_pushcounter == 2) {
    myservo.write(90);
    delay(20);
  }
  
  //////// SMOKE SENSOR ////////
  smokeSensorValue = analogRead(smokeAnalogInPin);    // read the analog in value:
  analogWrite(smokeAnalogled, smokeAnalogInPin-200);
  if (smokeSensorValue >= 370) {
    // when smoke detected
    digitalWrite(buzzer, HIGH);
    Serial.print("smoke= ");
    Serial.print(smokeSensorValue);
    Serial.println();
  }
  else {
   // when there is no smoke
   digitalWrite(buzzer, LOW);
  }
//  delay(10);
  //////// END SMOKE SENSOR ////////
  
  //////// ULTRASONIC SENSOR ////////
  long duration, cm;
  pinMode(ultrasensorOutput, OUTPUT);
  digitalWrite(ultrasensorOutput, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasensorOutput, HIGH);
  delayMicroseconds(5);
  digitalWrite(ultrasensorOutput, LOW);
  pinMode(ultrasensorOutput, INPUT);
  duration = pulseIn(ultrasensorOutput, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  
  if(cm < 10) {
    // near object detected 
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();
    if (alarmbutton_pushcounter % 2 == 0) {
      digitalWrite(ultrasensorled, HIGH);
      digitalWrite(buzzer, LOW);
    } else {
      digitalWrite(ultrasensorled, LOW);
      digitalWrite(buzzer, HIGH);
    }
  } else {
    if (cm < 100)
      analogWrite(ultrasensorled, 100-cm);
    else
      digitalWrite(ultrasensorled, LOW);
  }
  delay(15);
  //////// END ULTRASONIC SENSOR ////////

  //////// SOUND SENSOR ////////
  SoundsensorValue = analogRead(SoundsensorPin); 
  analogWrite(Soundsensorled, SoundsensorValue-100);
  if (SoundsensorValue>220) {
    if (alarmbutton_pushcounter % 2 == 0)
      digitalWrite(buzzer, LOW);
    else
      digitalWrite(buzzer, HIGH);
      delay(200);
    Serial.print("SoundsensorValue= ");
    Serial.print(SoundsensorValue);
    Serial.println();
  } else {
    digitalWrite(buzzer, LOW);
  }
  //////// END SOUND SENSOR ////////
}


long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

