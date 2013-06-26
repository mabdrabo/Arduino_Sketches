#include <SoftwareSerial.h>   //Software Serial Port
#define RxD 10
#define TxD 11
#define DEBUG_ENABLED  1
SoftwareSerial blueToothSerial(RxD,TxD);

#define ultrasonic_f 23  // front
#define ultrasonic_b 24  // back
float ultra_f = 0;  // front value
float ultra_b = 0;  //back value

#define linesensor_l  31  // left
#define linesensor_ml 33  // middle-left
#define linesensor_m 35  // middle
#define linesensor_mr 37  // middle-right
#define linesensor_r  39  // right
int linefollower_speed = 60;  // max speed when in line-follow mode
int last_pos = -1;

#define led1 40  // signal pin for LEDs
#define led2 42  // signal pin for LEDs
#define led3 44  // signal pin for LEDs
#define led4 46  // signal pin for LEDs

#define motor_left_0 53  // left PWM|Dir
#define motor_left_1 3  // left PWM
#define motor_right_0 52  // right PWM|Dir
#define motor_right_1 2  // right PWM

int motor_left_dir = 0;  // left Dir
int motor_right_dir = 0;  // right Dir
int left_motor_speed = 0;  // left Speed
int right_motor_speed = 0;  // right Speed
boolean dir_pwm_driver = true;  // set false if motor driver is 2-pwm pins & true if 1-pwm 1-dir
int basics, extras;

void setupPins() {
  pinMode(motor_left_0, OUTPUT);
  pinMode(motor_left_1, OUTPUT);
  pinMode(motor_right_0, OUTPUT);
  pinMode(motor_right_1, OUTPUT);
  pinMode(linesensor_l, INPUT);
  pinMode(linesensor_ml, INPUT);
  pinMode(linesensor_m, INPUT);
  pinMode(linesensor_mr, INPUT);
  pinMode(linesensor_r, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
}
 
void setupBlueToothConnection()
{
  blueToothSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
  blueToothSerial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
  blueToothSerial.print("\r\n+STNA=AEMUV-BT\r\n"); //set the bluetooth name as "AEMUV-BT"
  blueToothSerial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
  blueToothSerial.print("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
  delay(2000); // This delay is required.
  blueToothSerial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable
  Serial.println("The slave bluetooth is inquirable!");
  delay(2000); // This delay is required.
  blueToothSerial.flush();
}

void setup() {
  Serial.begin(9600);
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  setupBlueToothConnection();
  setupPins();
}

void loop() {
  if(blueToothSerial.available()){  //check if there's any data sent from the remote bluetooth shield
    basics = blueToothSerial.read();
    extras = blueToothSerial.read();
  }
 
//  if(Serial.available()){  //check if there's any data sent from the local serial terminal
//    blueToothSerial.write(Serial.parseInt());
//  }
 
  update_leds(getValue("led"));

  if (getValue("linefollow"))
      linefollower();
    else
      androidControl();

  check_ultrasonic();
  delayMicroseconds(50);
 
  Serial.print("left_speed: ");
  Serial.println(left_motor_speed);
  Serial.print("right_speed: ");
  Serial.println(right_motor_speed);
  Serial.println();
  Serial.println();
}
//// END OF LOOP ////


int getValue(String control) {
  // BASICS
  if (control == "y-dir")
      return bitRead(basics,0);
  if (control == "y-speed")
      return bin2Dec(bitRead(basics,1), bitRead(basics,2));
  if (control == "x-dir")
      return bitRead(basics,3);
  if (control == "x-speed")
      return bin2Dec(bitRead(basics,4), bitRead(basics,5));
  if (control == "motor-speed")
      return bin2Dec(bitRead(basics,6), bitRead(basics,7));
 
  // EXTRAS
  if (control == "ultrasonic")
      return bitRead(extras,0);
  if (control == "linefollow")
      return bitRead(extras,1);
  if (control == "led")
      return bitRead(extras,2);
}


void update_leds(int enable_led) {
  digitalWrite(led1, enable_led);
  digitalWrite(led2, enable_led);
  digitalWrite(led3, enable_led);
  digitalWrite(led4, enable_led);
}


void update_ultrasonic() {
    pinMode(ultrasonic_f, OUTPUT);
    digitalWrite(ultrasonic_b, LOW);
    delayMicroseconds(2);
    digitalWrite(ultrasonic_b, HIGH);
    delayMicroseconds(5);
    digitalWrite(ultrasonic_b, LOW);
    pinMode(ultrasonic_b, INPUT);
    ultra_b = pulseIn(ultrasonic_b, HIGH, 1160) / 29 / 2;
   
    pinMode(ultrasonic_b, OUTPUT);
    digitalWrite(ultrasonic_f, LOW);
    delayMicroseconds(2);
    digitalWrite(ultrasonic_f, HIGH);
    delayMicroseconds(5);
    digitalWrite(ultrasonic_f, LOW);
    pinMode(ultrasonic_f, INPUT);
    ultra_f = pulseIn(ultrasonic_f, HIGH, 1160) / 29 / 2;
   
}


void check_ultrasonic() {
  update_ultrasonic();
 
  Serial.print("ultra_f: ");
  Serial.println(ultra_f);
  Serial.print("ultra_b: ");
  Serial.println(ultra_b);
 
 
  if ((ultra_f > 0 && ultra_f < 10) || (ultra_b > 0 && ultra_b < 10)) {
    left_motor_speed = 0;
    right_motor_speed = 0;
  }
  else {
    if ((ultra_f > 0 && ultra_f < 10) || (ultra_b > 0 && ultra_b < 10)) {
      left_motor_speed /= 2;
      right_motor_speed /= 2;
    }
  }
 
  update_motor_speed(left_motor_speed, right_motor_speed);
}


void androidControl() {
  int linear_speed = map(getValue("y-speed"),0,3,0,map(getValue("motor-speed"),0,2,0,255));
  int steering_factor = map(getValue("x-speed"),0,3,0,255);
 
  if (getValue("motor-speed")==0) {
    left_motor_speed = steering_factor;
    right_motor_speed = steering_factor;
    if (getValue("x-dir") == 0) {
      motor_left_dir = inverse(getValue("y-dir"));
      motor_right_dir = getValue("y-dir");
    } else {
      motor_left_dir = getValue("y-dir");
      motor_right_dir = inverse(getValue("y-dir"));
    }
  }
  else{
    left_motor_speed = (getValue("x-dir")==0)? ((linear_speed - steering_factor < 0)? 0 : linear_speed - steering_factor) : linear_speed;
    right_motor_speed = (getValue("x-dir")==1)? ((linear_speed - steering_factor < 0)? 0 : linear_speed - steering_factor) : linear_speed;
//    if (getValue("motor-speed")==1) {
//      (getValue("x-dir")==0)?  motor_left_dir = inverse(getValue("y-dir")) : motor_left_dir = getValue("y-dir");
//      (getValue("x-dir")==1)?  motor_right_dir = inverse(getValue("y-dir")) : motor_right_dir = getValue("y-dir");
//    } else {
      motor_left_dir = getValue("y-dir");
      motor_right_dir = getValue("y-dir");
//    }
  }

  Serial.print("motor_speed: ");
  Serial.println(getValue("motor-speed"));
  Serial.print("steering_factor: ");
  Serial.println(steering_factor);
}


void linefollower() {

  boolean m_black = (digitalRead(linesensor_m) == HIGH)? true : false;
  boolean ml_black = (digitalRead(linesensor_ml) == HIGH)? true : false;
  boolean mr_black = (digitalRead(linesensor_mr) == HIGH)? true : false;
  boolean l_black = (digitalRead(linesensor_l) == HIGH)? true : false;
  boolean r_black = (digitalRead(linesensor_r) == HIGH)? true : false;
  Serial.println(last_pos);
  if (m_black)
  {
    // go forward
    Serial.println("middle");
    last_pos = 0;
    motor_left_dir = 0;
    motor_right_dir = 0;
    update_motor_speed(linefollower_speed, linefollower_speed);

  } else
  {
    if (ml_black)
    {
        // go left
        Serial.println("left");
        last_pos = -10;
        motor_left_dir = 0;
        motor_right_dir = 0;
        update_motor_speed(linefollower_speed, 0);

    } else
    {
      if (mr_black)
      {
        // go right
        Serial.println("right");
        last_pos = 10;
        motor_left_dir = 0;
        motor_right_dir = 0;
        update_motor_speed(0, linefollower_speed);

      } else
      {
        if (l_black)
        {
          // go tight left
          Serial.println("tight left");
          last_pos = -20;       
          motor_left_dir = 1;
          motor_right_dir = 0;
          update_motor_speed(linefollower_speed, linefollower_speed);

        } else
        {
          if (r_black)
          {
            // go tight right
            Serial.println("tight right");
            last_pos = 20;
            motor_left_dir = 0;
            motor_right_dir = 1;
            update_motor_speed(linefollower_speed, linefollower_speed);

          }
        }
      }
    }
  }
}


void update_motor_speed(float left_speed, float right_speed) {
  if (!dir_pwm_driver) {
    if (!motor_left_dir) {
      analogWrite(motor_left_0, left_speed);
      analogWrite(motor_left_1, 0);
    } else {
      analogWrite(motor_left_0, 0);
      analogWrite(motor_left_1, left_speed);
    }

    if (!motor_right_dir) {
      analogWrite(motor_right_0, right_speed);
      analogWrite(motor_right_1, 0);
    } else {
      analogWrite(motor_right_0, 0);
      analogWrite(motor_right_1, right_speed);
    }
  }  else {
    digitalWrite(motor_left_0, motor_left_dir);
    analogWrite(motor_left_1, int(left_speed));
    digitalWrite(motor_right_0, motor_right_dir);
    analogWrite(motor_right_1, int(right_speed));
  }
}


int bin2Dec(int b0, int b1) { // only 2-bit binary number
  if (b0 == b1)
    if (b0 == 0)
      return 0;
    else
      return 3;
  else
    if (b0 == 0)
      return 2;
    else
      return 1;
}


int inverse(int b0) { // only 1-bit
  return (b0==0)? 1 : 0;
}
