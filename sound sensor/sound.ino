//SOUND MONITOR for the classroom 

int sensorPin = 5;    //Microphone Sensor Pin
int YellowLED = 12;    
int RedLED = 11;    
int sensorValue = 0; 

void setup() {
  pinMode(YellowLED, OUTPUT); 
 pinMode(RedLED, OUTPUT); 
Serial.begin(9600);
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);    
 Serial.println(sensorValue);
 
   if (sensorValue>245){  //The 'silence' sensor value is 509-511
    digitalWrite(RedLED, HIGH);
    delay(15);            // The red LEDs stay on for 2 seconds
} else {
    digitalWrite(RedLED, LOW);  
}
 
 /* If you feel that the sensor values on the serial monitor
 are going by too quickly for you to read, you can make it 
 so that the values only show up on the serial monitor 
 if you make a noise.  You can replace 
 Serial.println(sensorValue); 
  with: 
 if (sensorValue < 509){
   Serial.println(sensorValue);
 }
 
 */

  digitalWrite(YellowLED, HIGH);  //Yellow is always on.


}