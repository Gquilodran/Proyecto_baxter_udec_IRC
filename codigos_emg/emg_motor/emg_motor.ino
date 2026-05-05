#include <Servo.h>;

Servo myservo;

int emg_signal;
int servoPin = 3;
int angle;

void setup() {
  Serial.begin(9600);

  myservo.attach(servoPin);
}

void loop() {
  emg_signal = analogRead(A0);
  if(emg_signal >= 200){

    angle = 70;
    myservo.write(angle);
    Serial.print("señal emg: ");
    Serial.print(emg_signal);
    Serial.print("/n");
    Serial.print("ángulo servo: ");
    Serial.println(angle);
    delay(500);
  } else{
    angle = 0;
    myservo.write(angle);
    Serial.print("señal emg: ");
    Serial.print(emg_signal);
    Serial.print("/n");
    Serial.print("ángulo servo: ");
    Serial.println(angle);
    delay(500);
  }
}
