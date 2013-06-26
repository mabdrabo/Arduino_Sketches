#include <SoftwareSerial.h>   //Software Serial Port
#define RxD 10
#define TxD 11
#define DEBUG_ENABLED  1
SoftwareSerial blueToothSerial(RxD,TxD);

#define motor1_left A0    // right PWM1
#define motor2_left A1    // right PWM2
#define motor1_right A4    // left PWM1
#define motor2_right A5    // left PWM2

int basics;

void setupPins() {
  pinMode(motorD_left, OUTPUT);
  pinMode(motorP_left, OUTPUT);
  pinMode(motorD_right, OUTPUT);
  pinMode(motorP_right, OUTPUT);
}

void setupBlueToothConnection()
{
  blueToothSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
  blueToothSerial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
  blueToothSerial.print("\r\n+STNA=ROV-BT\r\n"); //set the bluetooth name as "ROV"
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
  if(blueToothSerial.available()){ //check if there's any data sent from the remote bluetooth shield
    basics = blueToothSerial.read();
    
    int y_en = getValue("y-en");
    int y_dir = getValue("y-dir");
    int y_sp = getValue("y-speed");
    
    int x_en = getValue("x-en");    
    int x_dir = getValue("x-dir");
    int x_sp = getValue("x-speed");
    
    if(y_en) {
      if (y_dir) {
          digitalWrite(motor1_left,0);
          digitalWrite(motor2_left,1);
          digitalWrite(motor1_right,0);
          digitalWrite(motor2_right,1);
      } else {
          digitalWrite(motor1_left,1);
          digitalWrite(motor2_left,0);
          digitalWrite(motor1_right,1);
          digitalWrite(motor2_right,0);
      }
    } else {
        digitalWrite(motor1_left,0);
        digitalWrite(motor1_right,0);
        digitalWrite(motor2_left,0);
        digitalWrite(motor2_right,0);
    }
    
    if(x_en) {
      if(x_dir) {
          digitalWrite(motorD_right,0);
          digitalWrite(motorP_right,0);
      } else {
          digitalWrite(motorD_left,0);
          digitalWrite(motorP_left,0);
      }
    }
    
  }
}

int getValue(String control) {
  // BASICS
  if (control == "y-en")
      return bitRead(basics,0);
  if (control == "y-dir")
      return bitRead(basics,1);
  if (control == "y-speed")
      return bitRead(basics,2);
  if (control == "x-en")
      return bitRead(basics,3);
  if (control == "x-dir")
      return bitRead(basics,4);
  if (control == "x-speed")
      return bitRead(basics,5);
}
